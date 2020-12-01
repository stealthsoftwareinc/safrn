/**
 * Copyright (C) 2020 Stealth Software Technologies, Inc.
 *
 * Developed under IR&D
 *
*/

// Description:
//   Provides implementation of a Linux socket, for establishing a connection
//   between a Sender and Receiver.

#include "Util/socket.h"

#include <cerrno>
#include <netinet/tcp.h> // For TCP_NODELAY.
#include <set>
#include <string>
#include <sys/socket.h>
#include <sys/time.h> // For timeval
#include <sys/types.h> // For fd_set
#include <vector>

#ifndef LINUX_SOCKET_H
#define LINUX_SOCKET_H

namespace networking {

typedef int SOCKET;
const int SOCKET_ERROR = -1;

class LinuxSocket : public Socket {
public:
  // Constructor.
  LinuxSocket() : Socket() {
    socket_type_ = SocketType::OS_TCP;
    SetIsLinuxPlatform();
    Initialize();
  }
  // Constructor for Server, sets IP to default (0.0.0.0) and Port
  LinuxSocket(const unsigned long & port) : Socket(port) {
    socket_type_ = SocketType::OS_TCP;
    SetIsLinuxPlatform();
    Initialize();
  }
  // Constructor for Client, sets IP and Port
  LinuxSocket(const std::string & ip, const unsigned long & port) :
      Socket(ip, port) {
    socket_type_ = SocketType::OS_TCP;
    SetIsLinuxPlatform();
    Initialize();
  }
  // Constructor, sets Role
  LinuxSocket(const bool is_server) : Socket(is_server) {
    socket_type_ = SocketType::OS_TCP;
    SetIsLinuxPlatform();
    Initialize();
  }
  // Constructor, sets Role
  LinuxSocket(const SocketRole role) : Socket(role) {
    socket_type_ = SocketType::OS_TCP;
    SetIsLinuxPlatform();
    Initialize();
  }
  // Constructor, sets Role, IP, and Port
  LinuxSocket(
      const bool is_server,
      const std::string & ip,
      const unsigned long & port) :
      Socket(is_server, ip, port) {
    socket_type_ = SocketType::OS_TCP;
    SetIsLinuxPlatform();
    Initialize();
  }
  // Constructor, sets Role, IP, and Port
  LinuxSocket(
      const SocketRole role,
      const std::string & ip,
      const unsigned long & port) :
      Socket(role, ip, port) {
    socket_type_ = SocketType::OS_TCP;
    SetIsLinuxPlatform();
    Initialize();
  }
  // Destructor.
  ~LinuxSocket() noexcept {
    CloseSocket();
  }
  // Virtual constructor (creation).
  LinuxSocket * create() const {
    return new LinuxSocket();
  }
  // Virtual constructor (copying).
  LinuxSocket * clone() const {
    return new LinuxSocket(*this);
  }

  // Rule of 5: Since I wanted to define ~LinuxSocket, I obey Rule of 5 and
  // implement the following 4 functions, which I otherwise have no need for.
  //  - Copy Constructor.
  LinuxSocket(const LinuxSocket & w) {
    // Socket member variables.
    role_ = w.role_;
    socket_type_ = w.socket_type_;
    is_initialized_ = w.is_initialized_;
    is_socket_initialized_ = w.is_socket_initialized_;
    client_socket_is_bound_ = w.client_socket_is_bound_;
    server_socket_is_bound_and_accepting_connection_requests_ =
        w.server_socket_is_bound_and_accepting_connection_requests_;
    error_msg_ = w.error_msg_;
    stats_ = w.stats_;
    listen_params_ = w.listen_params_;
    connect_params_ = w.connect_params_;
    send_params_ = w.send_params_;
    connections_ = w.connections_;
    received_bytes_per_socket_ = w.received_bytes_per_socket_;
    bytes_to_send_per_socket_ = w.bytes_to_send_per_socket_;

    // LinuxSocket-specific member variables.
    is_linux_platform_ = w.is_linux_platform_;
    client_socket_ = w.client_socket_;
    server_socket_ = w.server_socket_;
  }
  //  - Move Constructor.
  LinuxSocket(LinuxSocket && w) = default;
  //  - Copy Assignment Operator.
  LinuxSocket & operator=(const LinuxSocket & w) {
    LinuxSocket temp(w); // Re-use copy-constructor.
    *this = std::move(temp); // Re-use move-assignment.
    return *this;
  }
  //  - Move Assignment Operator.
  LinuxSocket & operator=(LinuxSocket && other) = default;

  // Override inherited Socket::Initialize().
  bool Initialize();

  // Override inherited Socket::SocketSetSocketParams().
  void SocketSetSocketParams(const SocketParams & params);

private:
  bool is_linux_platform_;
  bool is_initialized_ = false;

  SOCKET client_socket_;
  SOCKET server_socket_;

  // Checks if the platform running the 'main' program is Linux, and sets
  // is_linux_platform_ accordingly.
  void SetIsLinuxPlatform();

  // Returns the SOCKET index of 's'.
  SOCKET GetSocketId(const SocketIdentifier & s);

  // Adds each socket connections in connections_, plus server_socket_ (resp.
  // client_socket_), to the appropriate list.
  void InitializeSocketLists(
      const SOCKET s,
      int * highest_id_to_check,
      fd_set * read_sockets,
      fd_set * write_sockets,
      fd_set * error_sockets);

  // Override inherited Socket::SocketGetClientSocketId().
  SocketIdentifier SocketGetClientSocketId() {
    return SocketIdentifier(client_socket_);
  }
  // Override inherited Socket::SocketGetServerSocketId().
  SocketIdentifier SocketGetServerSocketId() {
    return SocketIdentifier(server_socket_);
  }
  // Override inherited Socket::SocketGetReceiveSocketId().
  // NOTE: Since this only gets called if SocketRole == CLIENT, the appropriate
  // thing to do here is return the client_socket_.
  SocketIdentifier SocketGetReceiveSocketId() {
    return SocketGetClientSocketId();
  }
  // Override inherited Socket::SocketGetSendSocketId().
  // NOTE: Since this only gets called if SocketRole == CLIENT, the appropriate
  // thing to do here is return the client_socket_.
  SocketIdentifier SocketGetSendSocketId() {
    return SocketGetClientSocketId();
  }

  // Override inherited Socket::SocketIsClientSocket().
  bool SocketIsClientSocket(const SocketIdentifier & s) {
    return s.socket_id_ == client_socket_;
  }
  // Override inherited Socket::SocketIsServerSocket().
  bool SocketIsServerSocket(const SocketIdentifier & s) {
    return s.socket_id_ == server_socket_;
  }

  // Override inherited Socket::SocketSetupClientSocket().
  bool SocketSetupClientSocket();
  // Override inherited Socket::SocketSetupServerSocket().
  bool SocketSetupServerSocket();

  /*
  // DEPRECATED. Not used, so removed. Kept here for posterity, in case it is useful...
  // Override inherited Socket::SocketGetConnectedSocketIpAndPort().
  bool SocketGetBoundSocketIpAndPort(
      const SocketIdentifier& s, std::string* ip, unsigned int* port);
  */

  // Override inherited Socket::SocketSet[ | Client | Server]SocketOption().
  bool SocketSetSocketOption(
      const SocketIdentifier & s,
      const int socket_type,
      const int option_code);
  bool SocketSetClientSocketOption(
      const int socket_type, const int option_code);
  bool SocketSetServerSocketOption(
      const int socket_type, const int option_code);

  // Override inherited Socket::SocketSetTlsSocketOption.
  bool SocketSetTlsSocketOption(
      const SocketIdentifier & s, const int option_code);

  // Override inherited Socket::SocketSetNonBlockingSocket.
  bool SocketSetNonBlockingSocket(
      const bool non_blocking, const SocketIdentifier & s);

  // Override inherited Socket::SocketSleep().
  void SocketSleep(const long long int & sleep_micro_sec);

  // Override inherited Socket::SocketGetLastError().
  int SocketGetLastError();

  // Override inherited Socket::SocketGetSocketError.
  int SocketGetSocketError(const SocketIdentifier & s);
  int SocketGetClientSocketError();
  int SocketGetServerSocketError();

  // Override inherited Socket::SocketIsClientSocketInSet.
  bool
  SocketIsClientSocketInSet(const std::set<SocketIdentifier> & input);
  // Override inherited Socket::SocketIsServerSocketInSet.
  bool
  SocketIsServerSocketInSet(const std::set<SocketIdentifier> & input);

  // Override inherited Socket::SocketSocketErrorCode().
  int SocketSocketErrorCode() {
    return SOCKET_ERROR;
  }

  // Override inherited Socket::SocketSocketWouldBlockCode().
  int SocketSocketWouldBlockCode() {
    return EWOULDBLOCK;
  }

  // Override inherited Socket::SocketSocketInProgressCode().
  int SocketSocketInProgressCode() {
    return EINPROGRESS;
  }

  // Override inherited Socket::SocketSocketConnectResetCode().
  int SocketSocketConnectResetCode() {
    return ECONNRESET;
  }

  // Override inherited Socket::SocketSocketConnectionAbortedCode().
  int SocketSocketConnectionAbortedCode() {
    return ECONNABORTED;
  }

  // Override inherited Socket::SocketSocketConnectionRefusedCode().
  int SocketSocketConnectionRefusedCode() {
    return ECONNREFUSED;
  }

  // Override inherited Socket::SocketSocketUnavailableCode().
  int SocketSocketUnavailableCode() {
    return EADDRNOTAVAIL;
  }

  // Override inherited Socket::SocketSocketIntUseCode().
  int SocketSocketIntUseCode() {
    return EADDRINUSE;
  }

  // Override inherited Socket::SocketGetSocketLayerCode().
  int SocketGetSocketLayerCode() {
    return SOL_SOCKET;
  }

  // Override inherited Socket::SocketGetSocketTcpCode().
  int SocketGetSocketTcpCode() {
    return SOL_TCP;
  }

  // Override inherited Socket::SocketGetSocketTcpNoDelayCode().
  int SocketGetSocketTcpNoDelayCode() {
    return TCP_NODELAY;
  }

  // Override inherited Socket::SocketGetSocketLayerCode().
  int SocketGetSocketReusePortCode() {
    return SO_REUSEADDR;
  }

  // Override inherited Socket::SocketSocketOpAlreadyCode().
  int SocketSocketOpAlreadyCode() {
    return EALREADY;
  }

  // Override inherited Socket::SocketGetLocalHost().
  bool SocketGetLocalHost(std::string * ip);
  // Override inherited Socket::SocketGetWithinNetworkIP().
  bool SocketGetWithinNetworkIP(std::string * ip);

  // Override inherited Socket::SocketBind.
  bool SocketBind();

  // Override inherited Socket::SocketServerListen.
  bool SocketServerListen();

  // Override inherited Socket::SocketSelect.
  int SocketSelect(
      const SocketIdentifier & s,
      const uint64_t & timeout_secs,
      const uint64_t & timeout_micro_secs,
      std::set<SocketIdentifier> * read_sockets,
      std::set<SocketIdentifier> * write_sockets,
      std::set<SocketIdentifier> * error_sockets);

  // Override inherited Socket::SocketClientConnect().
  bool SocketClientConnect();

  // Override inherited Socket::SocketOpenNewConnection().
  bool SocketOpenNewConnection(SocketIdentifier * new_connection);

  // Override inherited Socket::SocketReceive().
  int SocketReceive(
      const SocketIdentifier & s,
      const int max_rec_bytes,
      char * rec_buffer);

  // Override inherited Socket::SocketSend().
  int SocketSend(
      const SocketIdentifier & s,
      const char * buffer,
      const size_t & num_chars_in_buffer);

  // Override inherited Socket::SocketShutdownConnection().
  bool SocketShutdownConnection(const SocketIdentifier & s);

  // Override inherited Socket::SocketCloseClientSocket.
  bool SocketCloseClientSocket(const bool connection_already_removed);
  // Override inherited Socket::SocketCloseServerSocket.
  bool SocketCloseServerSocket();
};

} // namespace networking

#endif
