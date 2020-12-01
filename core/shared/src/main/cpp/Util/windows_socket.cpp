/**
 * Copyright (C) 2020 Stealth Software Technologies, Inc.
 *
 * Developed under IR&D
 *
*/

#include "windows_socket.h"

#include "Util/map_utils.h"
#include <ff/logging.h>

#include <mutex>
#include <string>
#include <unistd.h> // For usleep
#include <vector>

#include <Winsock2.h>
#include <windows.h>
#include <winsock.h>
#include <ws2tcpip.h> // For addrinfo.

using namespace map_utils;
using namespace string_utils;
using namespace test_utils;
using namespace std;

namespace networking {

void WindowsSocket::SetIsWindowsPlatform() {
#if defined(WINDOWS) || defined(__WIN32__) || defined(__WIN64__) || \
    defined(_WIN32) || defined(_WIN64)
  is_windows_platform_ = true;
#else
  is_windows_platform_ = false;
#endif
}

SOCKET WindowsSocket::GetSocketId(const SocketIdentifier & s) {
  if (s.socket_id_ >= 0) {
    return (SOCKET)s.socket_id_;
  } else if (s.socket_index_ >= 0) {
    if (connections_.size() <= s.socket_index_) {
      log_fatal("Unable to find socket index %d", s.socket_index_);
    }
    return (SOCKET)connections_[s.socket_index_].socket_id_;
  }

  log_fatal("Unrecgonized socket.");
  return INVALID_SOCKET;
}

bool WindowsSocket::CallWSAStartup() {
  // WSAStartup should only be called once per program run, so we use a static
  // variable rather than the member variable is_initialized_ (using the latter
  // would cause every instance of this class to call WSAStartup).

  bool to_return = true;
  static std::once_flag wsa_flag;
  std::call_once(wsa_flag, [&]() {
    // Set version to be 2.0.
    WORD version = MAKEWORD(2, 0);

    // Create unused WSADATA object 'wsa_data': WSAStartup requires passing in a
    // (pointer to a) WSADATA object, but we don't need access to it.
    WSADATA wsa_data;
    const int return_code = WSAStartup(version, &wsa_data);

    // Sanity-Check initialization was successful.
    if (return_code != NO_ERROR) {
      if (return_code == WSASYSNOTREADY) {
        SetErrorMessage(
            __LINE__,
            "Failed to Initialize() WindowsSocket: WSASYSNOTREADY.");
      } else if (return_code == WSAVERNOTSUPPORTED) {
        SetErrorMessage(
            __LINE__,
            "Failed to Initialize() WindowsSocket: "
            "WSAVERNOTSUPPORTED.");
      } else if (return_code == WSAEINPROGRESS) {
        SetErrorMessage(
            __LINE__,
            "Failed to Initialize() WindowsSocket: WSAEINPROGRESS.");
      } else if (return_code == WSAEPROCLIM) {
        SetErrorMessage(
            __LINE__,
            "Failed to Initialize() WindowsSocket: WSAEPROCLIM.");
      } else if (return_code == WSAEFAULT) {
        SetErrorMessage(
            __LINE__,
            "Failed to Initialize() WindowsSocket: WSAEFAULT.");
      } else {
        SetErrorMessage(
            __LINE__,
            "Failed to Initialize() WindowsSocket: Unknown Error.");
      }
      to_return = false;
    }
  });

  return to_return;
}

bool WindowsSocket::Initialize() {
  if (is_initialized_)
    return true;

  if (stats_.activate_timers_) {
    StartTimer(&stats_.initialization_timer_);
  }

  if (!is_windows_platform_) {
    SetErrorMessage(
        __LINE__,
        "Failed to Initialize() WindowsSocket: Non-Windows platform.");
    if (stats_.activate_timers_) {
      StopTimer(&stats_.initialization_timer_);
    }
    return false;
  }

  // Initialize client_socket_ and server_socket_ to be invalid (they will
  // be initialized later, as needed).
  client_socket_ = INVALID_SOCKET;
  client_socket_is_bound_ = false;
  server_socket_ = INVALID_SOCKET;
  server_socket_is_bound_and_accepting_connection_requests_ = false;

  // Run WSAStartup.
  if (!CallWSAStartup()) {
    if (stats_.activate_timers_) {
      StopTimer(&stats_.initialization_timer_);
    }
    return false;
  }

  // Stop Timer.
  if (stats_.activate_timers_) {
    StopTimer(&stats_.initialization_timer_);
  }

  is_initialized_ = true;
  return true;
}

void WindowsSocket::InitializeSocketLists(
    const SOCKET s,
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
    }
  }
}

bool WindowsSocket::SocketSetupClientSocket() {
  client_socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (client_socket_ == INVALID_SOCKET)
    return false;
  return true;
}

bool WindowsSocket::SocketSetupServerSocket() {
  server_socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (server_socket_ == INVALID_SOCKET)
    return false;
  return true;
}

void WindowsSocket::SocketSetSocketParams(const SocketParams & params) {
  // TODO(paul): Implement this. Namely, if we ever want/need to set parameters
  // specific to WindowsSocket, extend SocketParams via a new struct
  // 'WindowsSocketParams', and add the desired parameters (member variables)
  // there; and then implement this function appropriately.
  // See RabbitMqSocket for an example.
}

bool WindowsSocket::SocketSetSocketOption(
    const SocketIdentifier & s,
    const int socket_type,
    const int option_code) {
  SOCKET sock = GetSocketId(s);
  char optval = 1;
  return 0 ==
      setsockopt(
             sock, socket_type, option_code, &optval, sizeof(optval));
}

bool WindowsSocket::SocketSetTlsSocketOption(
    const SocketIdentifier & s, const int option_code) {
  SOCKET sock = GetSocketId(s);
  unsigned long no_blocking_val = 1;
  return 0 == ioctlsocket(sock, FIONBIO, &no_blocking_val);
}

bool WindowsSocket::SocketSetClientSocketOption(
    const int socket_type, const int option_code) {
  char optval = 1;
  return 0 ==
      setsockopt(
             client_socket_,
             socket_type,
             option_code,
             &optval,
             sizeof(optval));
}

bool WindowsSocket::SocketSetServerSocketOption(
    const int socket_type, const int option_code) {
  char optval = 1;
  return 0 ==
      setsockopt(
             server_socket_,
             socket_type,
             option_code,
             &optval,
             sizeof(optval));
}

bool WindowsSocket::SocketSetNonBlockingSocket(
    const bool non_blocking, const SocketIdentifier & s) {
  SOCKET sock = GetSocketId(s);
  unsigned long no_blocking_val = non_blocking;
  return 0 == ioctlsocket(sock, FIONBIO, &no_blocking_val);
}

void WindowsSocket::SocketSleep(const long long int & sleep_micro_sec) {
  usleep(sleep_micro_sec);
}

int WindowsSocket::SocketGetLastError() {
  return WSAGetLastError();
}

bool WindowsSocket::SocketGetLocalHost(string * ip) {
  // Lookup IP of "localhost".
  HOSTENT * HostInfo = gethostbyname("localhost");
  if (HostInfo == NULL)
    return false;

  *ip = inet_ntoa(*(in_addr *)(HostInfo->h_addr_list[0]));

  return true;
}

bool WindowsSocket::SocketGetWithinNetworkIP(string * ip) {
  // Get the (string) name of the computer running the program.
  char my_computer_name[255];
  gethostname(my_computer_name, 255);

  // Use computer name to lookup its IP on local network.
  HOSTENT * HostInfo = gethostbyname(my_computer_name);
  if (HostInfo == NULL)
    return false;

  *ip = inet_ntoa(*(in_addr *)(HostInfo->h_addr_list[0]));

  return true;
}

bool WindowsSocket::SocketBind() {
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
  if (getaddrinfo(ip.c_str(), Itoa(port).c_str(), &hints, &addr) != 0) {
    return false;
  }

  // Use the first match.
  const int s2 =
      bind(server_socket_, addr[0].ai_addr, addr[0].ai_addrlen);

  // Free the list regardless of success.
  freeaddrinfo(addr);

  return s2 == 0;
}

bool WindowsSocket::SocketServerListen() {
  return listen(server_socket_, 1) != SOCKET_ERROR;
}

int WindowsSocket::SocketSelect(
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

  // We have (potentially) multiple sockets connected at the same time.
  // select() will determine the state of each socket: Read, Write, or Error.
  fd_set windows_read_sockets, windows_write_sockets,
      windows_error_sockets;

  const bool check_read_events = read_sockets != nullptr;
  const bool check_write_events = write_sockets != nullptr;
  const bool check_error_events = error_sockets != nullptr;

  // Add all of the sockets in connections_, plus the server_socket_, to
  // the appropriate fd_sets. select() will then test all of these sockets
  // to see if there was an event on it.
  InitializeSocketLists(
      sock,
      (check_read_events ? &windows_read_sockets : nullptr),
      (check_write_events ? &windows_write_sockets : nullptr),
      (check_error_events ? &windows_error_sockets : nullptr));

  // Set timeout for select().
  timeval timeout;
  timeout.tv_sec = timeout_secs;
  timeout.tv_usec = timeout_micro_secs;

  const int to_return = select(
      0 /* Ignored */,
      (check_read_events ? &windows_read_sockets : nullptr),
      (check_write_events ? &windows_write_sockets : nullptr),
      (check_error_events ? &windows_error_sockets : nullptr),
      (timeout_secs > 0 || timeout_micro_secs > 0) ? &timeout : 0);

  // As long as there was no error, update file descriptor sets to indicate which
  // socket there is an event on.
  if (to_return > 0) {
    if (check_write_events) {
      write_sockets->clear();
      for (unsigned int i = 0; i < windows_write_sockets.fd_count;
           ++i) {
        if (windows_write_sockets.fd_array[i] == sock) {
          write_sockets->insert(s);
        } else {
          write_sockets->insert(
              SocketIdentifier(windows_write_sockets.fd_array[i]));
        }
      }
    }
    if (check_read_events) {
      read_sockets->clear();
      for (unsigned int i = 0; i < windows_read_sockets.fd_count; ++i) {
        if (windows_read_sockets.fd_array[i] == sock) {
          read_sockets->insert(s);
        } else {
          read_sockets->insert(
              SocketIdentifier(windows_read_sockets.fd_array[i]));
        }
      }
    }
    if (check_error_events) {
      error_sockets->clear();
      for (unsigned int i = 0; i < windows_error_sockets.fd_count;
           ++i) {
        if (windows_error_sockets.fd_array[i] == sock) {
          error_sockets->insert(s);
        } else {
          error_sockets->insert(
              SocketIdentifier(windows_error_sockets.fd_array[i]));
        }
      }
    }
  }

  return to_return;
}

/* DEPRECATED. Not used, so removed. Kept here for posterity, in case it is useful...
bool WindowsSocket::SocketGetBoundSocketIpAndPort(
      const SocketIdentifier& s, string* ip, unsigned int* port) {
  SOCKET sock = GetSocketId(s);
  sockaddr local_address;
  int addr_size = sizeof(local_address);
  if (getsockname(sock, &local_address, &addr_size) == SOCKET_ERROR) {
    LOG_ERROR("Unable to get socket information: " + Itoa(SocketGetLastError()));
    return false;
  }

  // Grab the IP.
  *ip = string(inet_ntoa(((sockaddr_in*) &local_address)->sin_addr));

  // Grab the port:
  // Windows stores the port bytes (unsigned short, so 2 bytes) in reverse order.
  // Swap the order of the bytes to get the actual port number.
  const unsigned short reverse_bytes_port = ((sockaddr_in*) &local_address)->sin_port;
  vector<unsigned char> forward_bytes_port(2);
  forward_bytes_port[0] = *(((const unsigned char*) &reverse_bytes_port) + 1);
  forward_bytes_port[1] = *(((const unsigned char*) &reverse_bytes_port));
  *port = *((const unsigned short*) forward_bytes_port.data());

  return true;
}
*/

bool WindowsSocket::SocketIsClientSocketInSet(
    const set<SocketIdentifier> & input) {
  return input.find(SocketIdentifier(client_socket_)) != input.end();
}

bool WindowsSocket::SocketIsServerSocketInSet(
    const set<SocketIdentifier> & input) {
  return input.find(SocketIdentifier(server_socket_)) != input.end();
}

int WindowsSocket::SocketGetSocketError(const SocketIdentifier & s) {
  SOCKET sock = GetSocketId(s);
  int error_code;
  int error_code_length = sizeof(error_code);
  getsockopt(
      sock,
      SOL_SOCKET,
      SO_ERROR,
      (char *)&error_code,
      &error_code_length);
  return error_code;
}

int WindowsSocket::SocketGetClientSocketError() {
  int error_code;
  int error_code_length = sizeof(error_code);
  getsockopt(
      client_socket_,
      SOL_SOCKET,
      SO_ERROR,
      (char *)&error_code,
      &error_code_length);
  return error_code;
}

int WindowsSocket::SocketGetServerSocketError() {
  int error_code;
  int error_code_length = sizeof(error_code);
  getsockopt(
      server_socket_,
      SOL_SOCKET,
      SO_ERROR,
      (char *)&error_code,
      &error_code_length);
  return error_code;
}

bool WindowsSocket::SocketClientConnect() {
  // If this is non-blocking connect, we find a free port and bind it, so
  // that all subsequent connect() calls use the same port.
  if (connect_params_.non_blocking_ && !client_socket_is_bound_) {
    client_socket_is_bound_ = true;
    sockaddr_in client_addr;
    client_addr.sin_family = AF_INET;
    client_addr.sin_port =
        htons(0); // Setting port to 0 means 'use any free port'.
    const string zero_ip = "0.0.0.0";
    client_addr.sin_addr.S_un.S_addr = inet_addr(zero_ip.c_str());
    if (bind(
            client_socket_,
            (sockaddr *)&client_addr,
            sizeof(client_addr)) == SOCKET_ERROR) {
      log_error(
          "Unable to bind client socket: %d", SocketGetLastError());
      return false;
    }
  }

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
  if (getaddrinfo(
          connect_params_.ip_.c_str(),
          Itoa(connect_params_.port_).c_str(),
          &hints,
          &addr) != 0) {
    return false;
  }

  // Use the first match.
  const int s2 =
      connect(client_socket_, addr[0].ai_addr, addr[0].ai_addrlen);

  // Free the list regardless of success.
  freeaddrinfo(addr);

  return s2 == 0;
}

bool WindowsSocket::SocketOpenNewConnection(
    SocketIdentifier * new_connection) {
  sockaddr client_address;
  int client_address_length = sizeof(client_address);

  // The accept() function permits an incoming connection attempt on a socket. See:
  // https://msdn.microsoft.com/en-us/library/windows/desktop/ms737526(v=vs.85).aspx
  SOCKET windows_new_connection =
      accept(server_socket_, &client_address, &client_address_length);

  if (windows_new_connection == INVALID_SOCKET)
    return false;

  *new_connection = SocketIdentifier(windows_new_connection);
  return true;
}

int WindowsSocket::SocketReceive(
    const SocketIdentifier & s,
    const int max_rec_bytes,
    char * rec_buffer) {
  return recv(GetSocketId(s), rec_buffer, max_rec_bytes, 0);
}

int WindowsSocket::SocketSend(
    const SocketIdentifier & s,
    const char * buffer,
    const size_t & num_chars_in_buffer) {
  SOCKET send_socket = GetSocketId(s);
  return send(send_socket, buffer, num_chars_in_buffer, 0);
}

bool WindowsSocket::SocketShutdownConnection(
    const SocketIdentifier & s) {
  SOCKET sock = GetSocketId(s);
  return closesocket(sock) != SOCKET_ERROR;
}

bool WindowsSocket::SocketCloseClientSocket(
    const bool connection_already_removed) {
  if (!connection_already_removed &&
      (connections_.empty() ||
       connections_[0].socket_id_ != client_socket_)) {
    return false;
  }

  if ((shutdown(client_socket_, SD_BOTH) == SOCKET_ERROR ||
       closesocket(client_socket_) == SOCKET_ERROR) &&
      !connection_already_removed) {
    return false;
  }

  client_socket_is_bound_ = false;
  client_socket_ = INVALID_SOCKET;
  return true;
}

bool WindowsSocket::SocketCloseServerSocket() {
  closesocket(server_socket_);
  server_socket_ = INVALID_SOCKET;
  return true;
}

} // namespace networking
