/**
 * Copyright (C) 2020 Stealth Software Technologies, Inc.
 *
 * Developed under IR&D
 *
*/

#include "Util/linux_socket.h"

#include "Util/map_utils.h"

#include <ff/logging.h>

#include <arpa/inet.h> // For inet_ntoa.
#include <cerrno> // For errno.
#include <fcntl.h> // For fcntl.
#include <ifaddrs.h> // For ifaddrs.
#include <netdb.h> // For addrinfo.
#include <netinet/in.h> // For IPPROTO_TCP
#include <netinet/tcp.h> // For TCP_NODELAY.
#include <string>
#include <sys/param.h> // For HOST_NAME_MAX.
#include <sys/select.h> // For fd_set.
#include <sys/socket.h>
#include <sys/time.h> // For timeval.
#include <unistd.h> // For usleep.
#include <vector>

using namespace map_utils;
using namespace string_utils;
using namespace test_utils;
using namespace std;

namespace networking {

void LinuxSocket::SetIsLinuxPlatform() {
#if defined(WINDOWS) || defined(__WIN32__) || defined(__WIN64__) || \
    defined(_WIN32) || defined(_WIN64)
  is_linux_platform_ = false;
#else
  is_linux_platform_ = true;
#endif
}

SOCKET LinuxSocket::GetSocketId(const SocketIdentifier & s) {
  if (s.socket_id_ >= 0) {
    return (SOCKET)s.socket_id_;
  } else if (s.socket_index_ >= 0) {
    if (connections_.size() <= s.socket_index_) {
      log_fatal(
          "Unable to SendData: Unable to find socket index %d",
          s.socket_index_);
    }
    return (SOCKET)connections_[s.socket_index_].socket_id_;
  }

  log_fatal("Unrecgonized socket.");
  return 0;
}

bool LinuxSocket::Initialize() {
  if (is_initialized_)
    return true;

  if (stats_.activate_timers_) {
    StartTimer(&stats_.initialization_timer_);
  }

  if (!is_linux_platform_) {
    SetErrorMessage(
        __LINE__,
        "Failed to Initialize() LinuxSocket: Non-Linux platform.");
    if (stats_.activate_timers_) {
      StopTimer(&stats_.initialization_timer_);
    }
    return false;
  }

  // Initialize client_socket_ and server_socket_ to be invalid (they will
  // be initialized later, as needed).
  client_socket_ = -1;
  client_socket_is_bound_ = false;
  server_socket_ = -1;
  server_socket_is_bound_and_accepting_connection_requests_ = false;
  is_initialized_ = true;

  // TODO(PHB): For Windows' equivalent of the Initialize() function, I had to do
  // more stuff (e.g. call WSAEStartup()). Determine if I need to do something
  // similar for Linux, or if I'm good to go with just setting the above fields.

  if (stats_.activate_timers_) {
    StopTimer(&stats_.initialization_timer_);
  }

  return true;
}

void LinuxSocket::InitializeSocketLists(
    const SOCKET s,
    int * highest_id_to_check,
    fd_set * read_sockets,
    fd_set * write_sockets,
    fd_set * error_sockets) {
  // If 'select'ing for error events, add the socket to error_sockets.
  if (error_sockets != nullptr) {
    FD_ZERO(error_sockets);
    FD_SET(s, error_sockets);
  }

  // If 'select'ing for write events, add the socket to write_sockets.
  if (write_sockets != nullptr) {
    FD_ZERO(write_sockets);
    FD_SET(s, write_sockets);
  }

  // If 'select'ing for read events, add the socket to read_sockets.
  if (read_sockets != nullptr) {
    FD_ZERO(read_sockets);
    FD_SET(s, read_sockets);

    // We overload the call to SocketSelect for 'read' events, when 's' is
    // the main (Client/Server) socket: We check both the main socket for
    // events, as well as all sockets in connections_.
    for (vector<SocketIdentifier>::iterator itr = connections_.begin();
         itr != connections_.end();
         ++itr) {
      FD_SET((SOCKET)itr->socket_id_, read_sockets);
      if (error_sockets != nullptr) {
        FD_SET((SOCKET)itr->socket_id_, error_sockets);
      }
      if (itr->socket_id_ > *highest_id_to_check) {
        *highest_id_to_check = itr->socket_id_;
      }
    }
  }
}

bool LinuxSocket::SocketSetupClientSocket() {
  client_socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (client_socket_ == -1)
    return false;
  return true;
}

bool LinuxSocket::SocketSetupServerSocket() {
  server_socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (server_socket_ == -1)
    return false;
  return true;
}

void LinuxSocket::SocketSetSocketParams(const SocketParams & params) {
  // TODO(paul): Implement this. Namely, if we ever want/need to set parameters
  // specific to LinuxSocket, extend SocketParams via a new struct
  // 'LinuxSocketParams', and add the desired parameters (member variables)
  // there; and then implement this function appropriately.
  // See RabbitMqSocket for an example.
}

bool LinuxSocket::SocketSetSocketOption(
    const SocketIdentifier & s,
    const int socket_type,
    const int option_code) {
  SOCKET sock = GetSocketId(s);
  int optval = 1;
  return 0 ==
      setsockopt(
             sock, socket_type, option_code, &optval, sizeof(optval));
}

bool LinuxSocket::SocketSetTlsSocketOption(
    const SocketIdentifier & s, const int option_code) {
  SOCKET sock = GetSocketId(s);
  int flags = fcntl(sock, F_GETFL, 0);
  if (flags < 0)
    return false;
  flags |= option_code;
  return 0 == fcntl(sock, F_SETFL, flags);
}

bool LinuxSocket::SocketSetClientSocketOption(
    const int socket_type, const int option_code) {
  int optval = 1;
  return 0 ==
      setsockopt(
             client_socket_,
             socket_type,
             option_code,
             &optval,
             sizeof(optval));
}

bool LinuxSocket::SocketSetServerSocketOption(
    const int socket_type, const int option_code) {
  int optval = 1;
  return 0 ==
      setsockopt(
             server_socket_,
             socket_type,
             option_code,
             &optval,
             sizeof(optval));
}

bool LinuxSocket::SocketSetNonBlockingSocket(
    const bool non_blocking, const SocketIdentifier & s) {
  SOCKET sock = GetSocketId(s);
  int flags = fcntl(sock, F_GETFL, 0);
  if (flags < 0)
    return false;
  if (non_blocking)
    flags |= O_NONBLOCK;
  else
    flags &= ~O_NONBLOCK;
  return 0 == fcntl(sock, F_SETFL, flags);
}

void LinuxSocket::SocketSleep(const long long int & sleep_micro_sec) {
  usleep(sleep_micro_sec);
}

int LinuxSocket::SocketGetLastError() {
  return errno;
}

bool LinuxSocket::SocketGetLocalHost(string * ip) {
  // Get a list of all known IP's.
  ifaddrs * all_ips = nullptr;
  getifaddrs(&all_ips);

  bool found_localhost_ip = false;
  for (ifaddrs * current_ip = all_ips; current_ip != nullptr;
       current_ip = current_ip->ifa_next) {
    // Check if current IP is IP4.
    if (current_ip->ifa_addr->sa_family == AF_INET) {
      void * tmpAddrPtr =
          &((sockaddr_in *)current_ip->ifa_addr)->sin_addr;
      char addressBuffer[INET_ADDRSTRLEN];
      inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
      if (string(current_ip->ifa_name) == "lo") {
        *ip = string(addressBuffer);
        found_localhost_ip = true;
        break;
      }
    } else if (current_ip->ifa_addr->sa_family == AF_INET6) {
      // The current IP is IP6. Currently, we only are interested
      // in IP4 addresses; but we leave the code (commented-out)
      // below in case this changes in the future...
      /*
      void* tmpAddrPtr = &((sockaddr_in6*)current_ip->ifa_addr)->sin6_addr;
      char addressBuffer[INET6_ADDRSTRLEN];
      inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
      if (string(current_ip->ifa_name) == "lo") {
        *ip = string(addressBuffer);
        found_localhost_ip = true;
        break;
      }
      */
    }
  }
  if (all_ips != nullptr)
    freeifaddrs(all_ips);

  if (!found_localhost_ip) {
    log_warn(
        "Unable to find IP for localhost. Using default:\n%s",
        kDefaultLocalHostIp);
    *ip = string(kDefaultLocalHostIp);
  }

  return true;
}

bool LinuxSocket::SocketGetWithinNetworkIP(string * ip) {
  // Get a list of all known IP's.
  ifaddrs * all_ips = nullptr;
  getifaddrs(&all_ips);

  bool found_network_ip = false;
  vector<string> network_names;
  for (ifaddrs * current_ip = all_ips; current_ip != nullptr;
       current_ip = current_ip->ifa_next) {
    if (current_ip->ifa_addr == nullptr)
      continue;
    const string ifa_name = string(current_ip->ifa_name);
    network_names.push_back(ifa_name);
    // Check if current IP is IP4.
    if (current_ip->ifa_addr->sa_family == AF_INET) {
      if (ifa_name == "eth0" || ifa_name == "wlp113s0" ||
          /* VirtualBox uses different naming convention */
          ifa_name == "enp0s3") {
        void * tmpAddrPtr =
            &((sockaddr_in *)current_ip->ifa_addr)->sin_addr;
        char addressBuffer[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
        *ip = string(addressBuffer);
        found_network_ip = true;
        break;
      }
    } else if (current_ip->ifa_addr->sa_family == AF_INET6) {
      // The current IP is IP6. Currently, we only are interested
      // in IP4 addresses; but we leave the code (commented-out)
      // below in case this changes in the future...
      /*
      if (ifa_name == "eth0") {
        void* tmpAddrPtr = &((sockaddr_in6*)current_ip->ifa_addr)->sin6_addr;
        char addressBuffer[INET6_ADDRSTRLEN];
        inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
        *ip = string(addressBuffer);
        found_network_ip = true;
        break;
      }
      */
    }
  }
  if (all_ips != nullptr)
    freeifaddrs(all_ips);

  if (!found_network_ip) {
    log_fatal(
        "Unable to find (Local) Network IP: None of the found network "
        "names "
        "matched the expected 'eth0' (or 'enp0s3' for running Linux "
        "via "
        "VirtualBox on Windows):\n\t%s",
        Join(network_names, ", ").c_str());
  }

  return true;
}

bool LinuxSocket::SocketBind() {
  // Grab IP and Port.
  const string & ip = role_ == SocketRole::SERVER ? listen_params_.ip_ :
                                                    connect_params_.ip_;
  const unsigned long & port = role_ == SocketRole::SERVER ?
      listen_params_.port_ :
      connect_params_.port_;

  // The addrinfo structure specifies the known properties of the address we
  // want to listen on (IPv4 TCP/IP). The unknown property is the actual IP
  // address, as we may be trying to listen on a host name, in which case it
  // must be translated to an IP.
  addrinfo hints = {};
  hints.ai_flags =
      AI_PASSIVE; // Ensures the address is suitable for bind()
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;

  // The getaddrinfo function does any necessary address translation and
  // returns a list of at least one matching address.
  addrinfo * addr;
  const int s1 =
      getaddrinfo(ip.c_str(), Itoa(port).c_str(), &hints, &addr);
  if (s1 != 0) {
    return false;
  }

  // Use the first match.
  const int s2 =
      ::bind(server_socket_, addr[0].ai_addr, addr[0].ai_addrlen);

  // Free the list regardless of success.
  freeaddrinfo(addr);

  return s2 == 0;
}

bool LinuxSocket::SocketServerListen() {
  return listen(server_socket_, 1) != SOCKET_ERROR;
}

int LinuxSocket::SocketSelect(
    const SocketIdentifier & s,
    const uint64_t & timeout_secs,
    const uint64_t & timeout_micro_secs,
    set<SocketIdentifier> * read_sockets,
    set<SocketIdentifier> * write_sockets,
    set<SocketIdentifier> * error_sockets) {
  SOCKET sock = GetSocketId(s);

  // Sanity-check the socket is ready, provided it is the main socket.
  if (!is_socket_initialized_) {
    log_fatal("Client socket not initialized.");
  }

  const bool check_write_events = write_sockets != nullptr;
  const bool check_read_events = read_sockets != nullptr;
  const bool check_error_events = error_sockets != nullptr;

  // We have (potentially) multiple sockets connected at the same time.
  // select() will determine the state of each socket: Read, Write, or Error.
  fd_set linux_read_sockets, linux_write_sockets, linux_error_sockets;

  // Add all of the sockets in connections_, plus the server_socket_, to
  // the appropriate fd_sets. select() will then test all of these sockets
  // to see if there was an event on it.
  int highest_id_to_check = sock;
  InitializeSocketLists(
      sock,
      &highest_id_to_check,
      (check_read_events ? &linux_read_sockets : nullptr),
      (check_write_events ? &linux_write_sockets : nullptr),
      (check_error_events ? &linux_error_sockets : nullptr));

  // Set timeout for select().
  timeval timeout;
  timeout.tv_sec = timeout_secs;
  timeout.tv_usec = timeout_micro_secs;

  const int to_return = select(
      highest_id_to_check + 1,
      (check_read_events ? &linux_read_sockets : nullptr),
      (check_write_events ? &linux_write_sockets : nullptr),
      (check_error_events ? &linux_error_sockets : nullptr),
      (timeout_secs > 0 || timeout_micro_secs > 0) ? &timeout : 0);

  // As long as there was no error, update file descriptor sets to indicate which
  // socket there is an event on.
  if (to_return >= 0) {
    if (check_write_events) {
      write_sockets->clear();
      if (FD_ISSET(sock, &linux_write_sockets)) {
        write_sockets->insert(s);
      }
    }
    if (check_read_events) {
      // Check/update Read Sockets.
      read_sockets->clear();
      // Check if 's' is among the read sockets.
      if (FD_ISSET(sock, &linux_read_sockets)) {
        read_sockets->insert(s);
      }
      // Check all connections, to see if they are among the read sockets.
      for (vector<SocketIdentifier>::iterator itr =
               connections_.begin();
           itr != connections_.end();
           ++itr) {
        if (FD_ISSET(itr->socket_id_, &linux_read_sockets)) {
          read_sockets->insert(*itr);
        }
      }
    }
    if (check_error_events) {
      // Check/update Error Sockets.
      error_sockets->clear();
      // Check if 's' is among the error sockets.
      if (FD_ISSET(sock, &linux_error_sockets)) {
        error_sockets->insert(s);
      }
      // Check all connections, to see if they are among the read sockets.
      for (vector<SocketIdentifier>::iterator itr =
               connections_.begin();
           itr != connections_.end();
           ++itr) {
        if (FD_ISSET(itr->socket_id_, &linux_error_sockets)) {
          error_sockets->insert(*itr);
        }
      }
    }
  }

  return to_return;
}

/* DEPRECATED.
bool LinuxSocket::SocketGetBoundSocketIpAndPort(
      const SocketIdentifier& s, string* ip, unsigned int* port) {
  SOCKET sock = GetSocketId(s);
  sockaddr local_address;
  unsigned int addr_size = sizeof(local_address);
  if (getsockname(sock, &local_address, &addr_size) == SOCKET_ERROR) {
    LOG_ERROR("Unable to get socket information: " + Itoa(SocketGetLastError()));
    return false;
  }

  // Grab the IP.
  *ip = string(inet_ntoa(((sockaddr_in*) &local_address)->sin_addr));

  // Grab the port:
  // Windows stores the port bytes (unsigned short, so 2 bytes) in reverse order.
  // Swap the order of the bytes to get the actual port number.
  // TODO(PHB): Verify Linux also stores the bytes in reverse order.
  const unsigned short reverse_bytes_port = ((sockaddr_in*) &local_address)->sin_port;
  vector<unsigned char> forward_bytes_port(2);
  forward_bytes_port[0] = *(((const unsigned char*) &reverse_bytes_port) + 1);
  forward_bytes_port[1] = *(((const unsigned char*) &reverse_bytes_port));
  *port = *((const unsigned short*) forward_bytes_port.data());

  return true;
}
*/

bool LinuxSocket::SocketIsClientSocketInSet(
    const set<SocketIdentifier> & input) {
  return input.find(SocketIdentifier(client_socket_)) != input.end();
}

bool LinuxSocket::SocketIsServerSocketInSet(
    const set<SocketIdentifier> & input) {
  return input.find(SocketIdentifier(server_socket_)) != input.end();
}

int LinuxSocket::SocketGetSocketError(const SocketIdentifier & s) {
  SOCKET sock = GetSocketId(s);
  int error_code;
  socklen_t error_code_length = sizeof(error_code);
  getsockopt(
      sock,
      SOL_SOCKET,
      SO_ERROR,
      (char *)&error_code,
      &error_code_length);
  return error_code;
}

int LinuxSocket::SocketGetClientSocketError() {
  int error_code;
  socklen_t error_code_length = sizeof(error_code);
  getsockopt(
      client_socket_,
      SOL_SOCKET,
      SO_ERROR,
      (char *)&error_code,
      &error_code_length);
  return error_code;
}

int LinuxSocket::SocketGetServerSocketError() {
  int error_code;
  socklen_t error_code_length = sizeof(error_code);
  getsockopt(
      server_socket_,
      SOL_SOCKET,
      SO_ERROR,
      (char *)&error_code,
      &error_code_length);
  return error_code;
}

bool LinuxSocket::SocketClientConnect() {
  // If this is non-blocking connect, we find a free port and bind it, so
  // that all subsequent connect() calls use the same port.
  // UPDATE: This is not necessary for Linux sockets. This is because
  // Linux sockets handle non-blocking connect() calls differently than
  // Windows: "old" connect calls that are abandoned will not make it
  // to the Server's "stack" of socket events (once the Server finally
  // connects), and thus, we don't have to worry about re-using the
  // same (Client-side) port for all connect() calls, because only
  // the port of the most recent call will be seen by the Server.
  // NOTE: Compare/contrast this code to the code in windows_socket.cpp,
  // which has a bind() call here.

  // The addrinfo structure specifies the known properties of the address we
  // want to connect to (IPv4 TCP/IP). The unknown property is the actual IP
  // address, as we may be trying to connect to a host name, in which case it
  // must be translated to an IP.
  addrinfo hints = {};
  hints.ai_flags = 0; // Ensures the address is suitable for connect()
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;

  // The getaddrinfo function does any necessary address translation and
  // returns a list of at least one matching address.
  addrinfo * addr;
  const int s1 = getaddrinfo(
      connect_params_.ip_.c_str(),
      Itoa(connect_params_.port_).c_str(),
      &hints,
      &addr);
  if (s1 != 0) {
    return false;
  }

  // Use the first match.
  const int s2 =
      connect(client_socket_, addr[0].ai_addr, addr[0].ai_addrlen);

  // Free the list regardless of success.
  freeaddrinfo(addr);

  return s2 == 0;
}

bool LinuxSocket::SocketOpenNewConnection(
    SocketIdentifier * new_connection) {
  sockaddr client_address;
  socklen_t client_address_length = sizeof(client_address);

  SOCKET linux_new_connection =
      accept(server_socket_, &client_address, &client_address_length);

  if (linux_new_connection == -1)
    return false;

  *new_connection = SocketIdentifier(linux_new_connection);
  return true;
}

int LinuxSocket::SocketReceive(
    const SocketIdentifier & s,
    const int max_rec_bytes,
    char * rec_buffer) {
  return recv(GetSocketId(s), rec_buffer, max_rec_bytes, 0);
}

int LinuxSocket::SocketSend(
    const SocketIdentifier & s,
    const char * buffer,
    const size_t & num_chars_in_buffer) {
  SOCKET send_socket = GetSocketId(s);
  return send(send_socket, buffer, num_chars_in_buffer, 0);
}

bool LinuxSocket::SocketShutdownConnection(const SocketIdentifier & s) {
  SOCKET sock = GetSocketId(s);
  return close(sock) != SOCKET_ERROR;
}

bool LinuxSocket::SocketCloseClientSocket(
    const bool connection_already_removed) {
  if (!connection_already_removed &&
      (connections_.empty() ||
       connections_[0].socket_id_ != client_socket_)) {
    const string msg = connections_.empty() ?
        "Client Socket not present among connections_ (which is "
        "empty)" :
        "Client Socket is not the first (and only) item in "
        "connections_: " +
            Itoa(GetSocketId(connections_[0]));
    SetErrorMessage(
        __LINE__,
        "Failed to Close Client Socket (" + Itoa(client_socket_) +
            "): " + msg);
    return false;
  }

  if ((shutdown(client_socket_, SHUT_RDWR) == SOCKET_ERROR ||
       close(client_socket_) == SOCKET_ERROR) &&
      !connection_already_removed) {
    SetErrorMessage(
        __LINE__,
        "Failed to Close Client Socket (" + Itoa(client_socket_) +
            "): " + Itoa(SocketGetLastError()));
    return false;
  }

  client_socket_ = -1;
  client_socket_is_bound_ = false;
  return true;
}

bool LinuxSocket::SocketCloseServerSocket() {
  close(server_socket_);
  server_socket_ = -1;
  return true;
}

} // namespace networking
