/**
 * Copyright (C) 2020 Stealth Software Technologies, Inc.
 *
 * Developed under IR&D
 *
*/

// Description:
//   Provides implementation of a Windows socket, for establishing a connection
//   between a Sender and Receiver.

#ifndef WINDOWS_SOCKET_H
#define WINDOWS_SOCKET_H

#include "socket.h"

#include <memory> // For unique_ptr
#include <set>
#include <string>

#include <windows.h>
#include <winsock.h>
#include <winsock2.h>

namespace networking {

class WindowsSocket : public Socket {
public:
  // Constructor.
  WindowsSocket() : Socket() {
    socket_type_ = SocketType::OS_TCP;
    SetIsWindowsPlatform();
    Initialize();
  }
  // Constructor for Server, sets IP to default (0.0.0.0) and Port
  WindowsSocket(const unsigned long & port) : Socket(port) {
    socket_type_ = SocketType::OS_TCP;
    SetIsWindowsPlatform();
    Initialize();
  }
  // Constructor for Client, sets IP and Port
  WindowsSocket(const std::string & ip, const unsigned long & port) :
      Socket(ip, port) {
    socket_type_ = SocketType::OS_TCP;
    SetIsWindowsPlatform();
    Initialize();
  }
  // Constructor, sets Role
  WindowsSocket(const bool is_server) : Socket(is_server) {
    socket_type_ = SocketType::OS_TCP;
    SetIsWindowsPlatform();
    Initialize();
  }
  // Constructor, sets Role
  WindowsSocket(const SocketRole role) : Socket(role) {
    socket_type_ = SocketType::OS_TCP;
    SetIsWindowsPlatform();
    Initialize();
  }
  // Constructor, sets Role, IP, and Port
  WindowsSocket(
      const bool is_server,
      const std::string & ip,
      const unsigned long & port) :
      Socket(is_server, ip, port) {
    socket_type_ = SocketType::OS_TCP;
    SetIsWindowsPlatform();
    Initialize();
  }
  // Constructor, sets Role, IP, and Port
  WindowsSocket(
      const SocketRole role,
      const std::string & ip,
      const unsigned long & port) :
      Socket(role, ip, port) {
    socket_type_ = SocketType::OS_TCP;
    SetIsWindowsPlatform();
    Initialize();
  }
  // Destructor.
  ~WindowsSocket() noexcept {
    CloseSocket();
  }
  // Virtual constructor (creation).
  WindowsSocket * create() const {
    return new WindowsSocket();
  }
  // Virtual constructor (copying).
  WindowsSocket * clone() const {
    return new WindowsSocket(*this);
  }

  // Rule of 5: Since I wanted to define ~WindowsSocket, I obey Rule of 5 and
  // implement the following 4 functions, which I otherwise have no need for.
  //  - Copy Constructor.
  WindowsSocket(const WindowsSocket & w) {
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

    // WindowsSocket-specific member variables.
    is_windows_platform_ = w.is_windows_platform_;
    client_socket_ = w.client_socket_;
    server_socket_ = w.server_socket_;
  }
  //  - Move Constructor.
  WindowsSocket(WindowsSocket && w) = default;
  //  - Copy Assignment Operator.
  WindowsSocket & operator=(const WindowsSocket & w) {
    WindowsSocket temp(w); // Re-use copy-constructor.
    *this = std::move(temp); // Re-use move-assignment.
    return *this;
  }
  //  - Move Assignment Operator.
  WindowsSocket & operator=(WindowsSocket && other) = default;

private:
  bool is_windows_platform_;

  SOCKET client_socket_;
  SOCKET server_socket_;

  // Checks if the platform running the 'main' program is Windows, and sets
  // is_windows_platform_ accordingly.
  void SetIsWindowsPlatform();

  // Thread-safe: only called once per program (statically).
  bool CallWSAStartup();

  // Returns the SOCKET index of 's'.
  SOCKET GetSocketId(const SocketIdentifier & s);

  // Adds each socket connections in connections_, plus server_socket_ (resp.
  // client_socket_), to the appropriate list.
  void InitializeSocketLists(
      const SOCKET s,
      fd_set * read_sockets,
      fd_set * write_sockets,
      fd_set * error_sockets);
  // Override inherited Socket::Initialize().
  bool Initialize();

  // Override inherited Socket::SocketSetSocketParams().
  void SocketSetSocketParams(const SocketParams & params);

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

  // DEPRECATED. Not used, so removed. Kept here for posterity, in case it is useful...
  /*
  bool SocketGetBoundSocketIpAndPort(
      const SocketIdentifier& s, string* ip, unsigned int* port);
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
    return WSAEWOULDBLOCK;
  }

  // Override inherited Socket::SocketSocketInProgressCode().
  int SocketSocketInProgressCode() {
    return WSAEINPROGRESS;
  }

  // Override inherited Socket::SocketSocketConnectResetCode().
  int SocketSocketConnectResetCode() {
    return WSAECONNRESET;
  }

  // Override inherited Socket::SocketSocketConnectionAbortedCode().
  int SocketSocketConnectionAbortedCode() {
    return WSAECONNABORTED;
  }

  // Override inherited Socket::SocketSocketConnectionRefusedCode().
  int SocketSocketConnectionRefusedCode() {
    return WSAECONNREFUSED;
  }

  // Override inherited Socket::SocketSocketUnavailableCode().
  int SocketSocketUnavailableCode() {
    return WSAENOTSOCK;
  }

  // Override inherited Socket::SocketSocketIntUseCode().
  int SocketSocketIntUseCode() {
    return WSAEADDRINUSE;
  }

  // Override inherited Socket::SocketGetSocketLayerCode().
  int SocketGetSocketLayerCode() {
    return SOL_SOCKET;
  }

  // Override inherited Socket::SocketGetSocketTcpCode().
  int SocketGetSocketTcpCode() {
    return IPPROTO_TCP;
  }

  // Override inherited Socket::SocketGetSocketTcpNoDelayCode().
  int SocketGetSocketTcpNoDelayCode() {
    return TCP_NODELAY;
  }

  // Override inherited Socket::SocketGetSocketReusePortCode().
  int SocketGetSocketReusePortCode() {
    return SO_REUSEADDR;
  }

  // Override inherited Socket::SocketSocketOpAlreadyCode().
  int SocketSocketOpAlreadyCode() {
    return WSAEALREADY;
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
