/**
 * Copyright (C) 2020 Stealth Software Technologies, Inc.
 *
 * Developed under IR&D
 *
*/

#include "Util/socket_utils.h"

#include "Util/socket.h"
#include "Util/string_utils.h"

#if defined(WINDOWS) || defined(__WIN32__) || defined(__WIN64__) || \
    defined(_WIN32) || defined(_WIN64)
#include "Util/windows_socket.h"
#else
#include "Util/linux_socket.h"
#endif

#include <memory> // For unique_ptr

using namespace string_utils;
using namespace std;

namespace networking {

static const char kLocalHostKeyword[] = "localhost";
static const char kNetworkIpKeyword[] = "network";

namespace {
void ParseIpKeyword(
    const SocketRole & role,
    const string & ip,
    unique_ptr<Socket> * s) {
  string actual_ip = ip;
  if (ip == string(kLocalHostKeyword)) {
    (*s)->GetLocalHost(&actual_ip);
  } else if (ip == string(kNetworkIpKeyword)) {
    (*s)->GetWithinNetworkIP(&actual_ip);
  }
  if (actual_ip != ip) {
    if (role == SocketRole::SERVER) {
      (*s)->SetListenIp(actual_ip);
    }
    if (role == SocketRole::CLIENT) {
      (*s)->SetConnectIp(actual_ip);
    }
  }
}
} // namespace

void CreateSocket(const SocketParams & params, unique_ptr<Socket> * s) {
  // Now call appropriate Socket() constructor, based on SocketType.
  const SocketType type = params.type_;
  if (type == SocketType::OS_TCP) {
    const TcpSocketParams * tcp_params = (TcpSocketParams *)&params;
    const string & ip = tcp_params->role_ == SocketRole::CLIENT ?
        tcp_params->connect_ip_ :
        tcp_params->listen_ip_;
#if defined(WINDOWS) || defined(__WIN32__) || defined(__WIN64__) || \
    defined(_WIN32) || defined(_WIN64)
    *s = unique_ptr<WindowsSocket>(
        new WindowsSocket(tcp_params->role_, ip, tcp_params->port_));
#else
    *s = unique_ptr<LinuxSocket>(
        new LinuxSocket(tcp_params->role_, ip, tcp_params->port_));
#endif
    ParseIpKeyword(tcp_params->role_, ip, s);
  }
}

} // namespace networking
