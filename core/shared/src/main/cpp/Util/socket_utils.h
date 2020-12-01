/**
 * Copyright (C) 2020 Stealth Software Technologies, Inc.
 *
 * Developed under IR&D
 *
*/

// Description:
//   Helper functions to define generic Sockets, so that caller doesn't
//   need to worry about what OS they're on.

#ifndef SOCKET_UTILS_H
#define SOCKET_UTILS_H

#include "Util/socket.h"

#include <memory> // For unique_ptr.

namespace networking {

// Creates a socket as per params.
// The SocketParams object must have the appropriate instantiatied type, as
// determined by the params.type_ field (e.g. TcpSocketParams).
extern void
CreateSocket(const SocketParams & params, std::unique_ptr<Socket> * s);

// Creates a OS_TCP socket.
inline void CreateSocket(
    const SocketRole role,
    const std::string & ip,
    const unsigned long & port,
    std::unique_ptr<Socket> * s) {
  TcpSocketParams params(
      role,
      role == SocketRole::CLIENT ? ip : "",
      role == SocketRole::SERVER ? ip : "",
      port);

  CreateSocket(params, s);
}
// Same as above, for OS_TCP socket.
inline void CreateSocket(
    const bool is_server,
    const std::string & ip,
    const unsigned long & port,
    std::unique_ptr<Socket> * s) {
  CreateSocket(
      is_server ? SocketRole::SERVER : SocketRole::CLIENT, ip, port, s);
}

} // namespace networking
#endif
