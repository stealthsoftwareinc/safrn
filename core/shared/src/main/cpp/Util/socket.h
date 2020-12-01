/**
 * Copyright (C) 2020 Stealth Software Technologies, Inc.
 *
 * Developed under IR&D
 *
*/

// Description:
//   Provides a base class for establishing a connection between two people.
//   This class is meant to provide a framework for the relevant member
//   variables and functions that can be used; actual implementations are
//   are done by derived classes:
//     - Platform-specific: Use the OS underlying send/receive functionalities.
//                          Currently supported for Linux (in linux_socket.h,cpp)
//                          and Windows (in windows_socket.h,cpp).
//     - External Library:  Uses an external libraries send/receive functionalities.
//                          Currently supported for RabbitMQ (in rabbitmq_socket.h,cpp).
//
// Usage:
//   A socket can be asymmetric (Server vs. Client) or symmetric (both parties
//   act as a "Client"). However, the distinction is only relevant when first
//   creating (constructing) a Socket object. All details concerning the
//   connection (e.g. who connects to who) are handled under-the-hood.
//   Thus, after constructing a Socket of the appropriate type (Client vs. Server),
//   there is no need to explicitly Connect() to the other party; instead,
//   the connection will happen automatically upon calls to SendData() or
//   Listen().
//
//   Therefore, the user should determine the following:
//     - SocketRole (Client vs. Server).
//       This is set at object creation (via use of appropriate constructor).
//     - IP,PORT.
//       This can either be a listen IP/port (for SocketRole::SERVER) or a
//       connect IP/Port (for SocketRole::CLIENT). The IP/Port can be set
//       at creation time (via use of appropriate constructor), or explicitly
//       via a call to e.g. Set[Connect | Listen]IpAndPort().
//     - Connect parameters.
//       While Connect() is called under-the-hood, users can still specify
//       how to handle Connect() calls, e.g. timeouts, errors, etc.
//       (Non-default) connect parameters can be set via SetConnectParams().
//     - Listen parameters. See discussion above ListenParams below.
//       (Non-default) listen parameters can be set via SetListenParams().
//
//  The socket_utils.[h,cpp] file provides a wrapper around creating an
//  actual Socket object (since the Socket class is purely abstract, the
//  object created will have type of one of the instantiating libraries,
//  e.g. currently WindowsSocket, LinuxSocket, CookieSocket, and RabbitMqSocket.
//
//  See socket_test.cpp for concrete usage examples.

#ifndef SOCKET_H
#define SOCKET_H

#include "Util/init_utils.h"
#include "Util/string_utils.h" // For Itoa.
#include "Util/timer_utils.h" // For Timer.

#include <algorithm> // For swap_ranges, copy
#include <cstdint> // For uintptr_t.
#include <cstring> // For memcpy.
#include <map>
#include <memory> // For unique_ptr.
#include <set>
#include <string>
#include <tuple> // For std::tie
#include <vector>

namespace networking {

// Discussion on Sending and Receiving bytes:
// The Socket class relies on some external library to actually control
// communication flow (send/receive); as such, this external library has
// a mechanism to actually send and receive bytes, and specifically, the
// external library will have its own send/receive buffers for temporarily
// storing the bytes as they flow across some network connection.
// Meanwhile, the Socket API will allow access to these temporary buffers
// via SocketSend() and SocketReceive(), whereby the actual bytes to be
// sent (respectively received) are passed-in from (resp. read into)
// Socket's own Send/Receive buffers: DataToSend and ReceivedData,
// which are in turn stored via Socket's bytes_to_send_per_socket_ and
// received_bytes_per_socket_ fields.
//
// For copying bytes from the underlying library's receive buffer to
// Socket's receive buffer, there three values of interest:
//   1) Max bytes to copy per batch/call to SocketReceive();
//   2) Max bytes to allow in Socket's receive buffer.
//   3) Max bytes that the external library allows in its (temporary)
//      receive buffer.
// Note that (1) and (2) above are slightly different, as SocketReceive()
// can be called many times (before the bytes are actually processed/removed
// from the Socket's ReceivedData buffer), slowly growing the buffer size
// until the Socket's max buffer size is reached. If (3) happens to be larger
// than (2), and SocketReceive() is called on a (ReceiveData) buffer that has
// already reached its capacity (as per (2)), then that call to SocketReceive()
// will fail. Also note that (3) is controlled by the external library.
// In the present Socket class, we set default values for (1) and (2) via
// the two fields below.
const uint64_t kMaxBytesTransferrable =
    (uint64_t)5 * 1024 * 1024 * 1024; // 5 GigaBytes.
const int kSocketBufferSize = 1024;

// Default value for ListenParams::select_timeout_ms_, the latter of which
// is used in the call to SocketSelect().
const int kSelectTimeoutMilliSecs = 100;
// Default value for ListenParams::no_event_sleep_ms_, the latter of which
// controls how long to sleep while Listen()ing.
const int kSleepMsBetweenNoEvents = 10;
// Default Listen IP and Local Host Ip.
const char kDefaultListenIp[] = "0.0.0.0";
const char kDefaultLocalHostIp[] = "127.0.0.1";
// DEPRECATED. There isn't a good default IP to use as the local (network) IP,
// as the last number can vary in 0-255.
// const char kDefaultLocalNetworkIp[] = "192.168.1.0";

// The various kind of Socket instantiated classes.
enum class SocketType {
  UNKNOWN,
  OS_TCP, // windows_socket, linux_socket
  RABBITMQ,
  COOKIE // Socket is just two handles to a Send/Receive function (implemented elsewhere)
};

// Will determine if a given Socket instance corresponds to the Server or Client.
enum class SocketRole {
  UNKNOWN,
  SERVER,
  CLIENT,
  NEITHER // For connections that don't have assigned Client/Server roles
  // (e.g. for CookieSocket).
};

enum class SocketState {
  UNKNOWN,
  NOT_CONNECTED,
  CONNECTED,
  MULTIPLE_CONNECTIONS,
};

// Various ways to "Listen" to a socket/connection. See comments in LISTEN
// section below.
enum class ListenType {
  UNKNOWN,
  HANDLE_NEW_CONNECTIONS,

  // DEPRECATED. All disconnections are handled automatically; an error
  // is logged if there was a problem, and caller can regain control by
  // setting a StopListeningCondtion based on this, if desired.
  // HANDLE_DISCONNECTS,

  // DEPRECATED. Errors are logged to the appropriate SocketStats counters, and
  // caller can regain control by setting the appropriate StopListeningCondtion.
  // HANDLE_ERRORS,

  // DEPRECATED. I can't think of a use-case where the socket instance has been
  // told to send something, but then doesn't actually send it...
  // HANDLE_SEND,

  HANDLE_RECEIVE,
  HANDLE_ALL_EVENTS,
  USE_STOP_CONDITION, // Indicates the StopListeningCondition should be used.
};

// Various conditions that may have been encountered while Listening().
enum class ListenReturnCode {
  UNKNOWN,
  OK,

  // ============================== ERRORS =====================================
  // Returned if platform is wrong, or Initialize() hasn't been called, etc.
  BAD_INITIALIZATION_ERROR,
  // Returned when an error was encountered while attempting to bind().
  BIND_ERROR,
  // Returned when an error was encountered while attempting to listen().
  LISTEN_ERROR,
  // Returned when an error was encountered while trying to add a new connection.
  CONNECT_ERROR,
  // Returned when an error was encountered while trying to disconnect a socket.
  DISCONNECT_ERROR,
  // Returned when an error was encountered during select().
  SELECT_ERROR,
  // Returned when Server Socket has an Error Event.
  SERVER_SOCKET_ERROR_EVENT,
  // Returned when Client Socket has an Error Event.
  CLIENT_SOCKET_ERROR_EVENT,
  // Returned when a socket was placed in 'error_sockets' upon call to select().
  ERROR_EVENT,
  // Returned when an unreoverable Receive Error occurs.
  RECEIVE_ERROR,
  // Returned when an unexpected number of bytes were received.
  RECEIVED_UNEXPECTED_BYTES,
  // Returned when an unreoverable Send Error occurs.
  SEND_ERROR,
  // Returned when too many errors were encountered.
  TOO_MANY_ERRORS,

  // ========================= STOPPING CRITERIA ===============================
  // Returned when number of current connections is too high.
  TOO_MANY_CONNECTIONS,
  // Returned when aggregate number of connections reaches some threshold
  CUMULATIVE_CONNECTIONS_THRESHOLD,
  // Returned when a threshold number of bytes have been sent.
  CUMULATIVE_BYTES_SENT,
  // Returned when a threshold number of bytes have been received.
  CUMULATIVE_BYTES_RECEIVED,
  // Returned when a threshold number of Send transactions have occurred.
  CUMULATIVE_SEND_TRANSACTIONS,
  // Returned when a threshold number of Receive transactions have occurred.
  CUMULATIVE_RECEIVE_TRANSACTIONS,
  // Returned when a threshold number of (message terminating) NULL characters
  // have been received.
  CUMULATIVE_RECEIVE_NULL_CHARS,
  // Returned when too much time has passed since the last event.
  NO_ACTIVITY,

  // ============================== EVENTS =====================================
  NEW_CONNECTION,
  RECEIVE,
  // DEPRECATED. I can't think of a use-case where the socket instance has been
  // told to send something, but then doesn't actually send it; so no need for this
  //SEND,
};

// Various conditions that may have been encountered in a call to SendData().
enum class SendReturnCode {
  SUCCESS,
  BAD_SOCKET,
  SOCKET_NOT_READY,
  SOCKET_ERROR,
  UNKNOWN_ERROR,
  UNKNOWN_SOCKET,
  SELECT_ERROR,
  SEND_ERROR,
  ENQUEUE_ERROR,
  BAD_ROLE,
  CANNOT_CONNECT,
};

extern std::string
GetListenReturnCodeString(const ListenReturnCode code);
extern std::set<std::string>
GetListenReturnCodeList(const std::set<ListenReturnCode> & codes);

// To be used to chain together StopListeningCondition(s).
enum class ConditionOperation {
  NONE, // Indicates the StopListeningCondition has not been setup
  IDENTITY, // Use this to indicate no sub-conditions.
  NOT,
  AND,
  OR,
  XOR,
};

// Generic type for identifying a socket. For WindowsSocket instantiations,
// this can be either a SOCKET id (overloaded to 'int' type below, since
// socket.h needs to be ignorant of SOCKET type) or the index within
// connections_. For unix, it can be ...
struct SocketIdentifier {
  int socket_id_;
  int socket_index_; // Within connections_.
  std::string ip_;
  unsigned long port_;
  std::string label_;
  void * void_ptr_;

  SocketIdentifier() {
    socket_id_ = -1;
    socket_index_ = -1;
    ip_ = "";
    port_ = 0;
    label_ = "";
    void_ptr_ = nullptr;
  }
  explicit SocketIdentifier(const int id) {
    socket_id_ = id;
    socket_index_ = -1;
    ip_ = "";
    port_ = 0;
    label_ = "";
    void_ptr_ = nullptr;
  }
  explicit SocketIdentifier(const std::string & label) {
    socket_id_ = -1;
    socket_index_ = -1;
    ip_ = "";
    port_ = 0;
    label_ = label;
    void_ptr_ = nullptr;
  }
  explicit SocketIdentifier(void * caller_owned_ptr) {
    socket_id_ = -1;
    socket_index_ = -1;
    ip_ = "";
    port_ = 0;
    label_ = "";
    void_ptr_ = caller_owned_ptr;
  }
  SocketIdentifier(const std::string & ip, const unsigned long & port) {
    socket_id_ = -1;
    socket_index_ = -1;
    ip_ = ip;
    port_ = port;
    label_ = "";
    void_ptr_ = nullptr;
  }
  SocketIdentifier(
      const std::string & label,
      const std::string & ip,
      const unsigned long & port) {
    socket_id_ = -1;
    socket_index_ = -1;
    ip_ = ip;
    port_ = port;
    label_ = label;
    void_ptr_ = nullptr;
  }
  SocketIdentifier(const int id, const int windows_index) {
    socket_id_ = id;
    socket_index_ = windows_index;
    ip_ = "";
    port_ = 0;
    label_ = "";
    void_ptr_ = nullptr;
  }

  // So that SocketIdentifier can be the Key of a set/map.
  bool operator<(const SocketIdentifier & x) const {
    return std::tie(socket_id_, socket_index_, port_, ip_, label_) <
        std::tie(
               x.socket_id_, x.socket_index_, x.port_, x.ip_, x.label_);
  }

  // So that JoinObject() can be called.
  std::string ToString() const {
    std::string to_return = "";
    std::string sep = "";
    if (socket_id_ != -1) {
      to_return += sep + "ID: " + string_utils::Itoa(socket_id_);
      sep = ",";
    }
    if (socket_index_ != -1) {
      to_return += sep + "Index: " + string_utils::Itoa(socket_index_);
      sep = ",";
    }
    if (!ip_.empty()) {
      to_return += sep + "IP: " + ip_ + ":" + string_utils::Itoa(port_);
      sep = ",";
    }
    if (!label_.empty()) {
      to_return += sep + "Label: " + label_;
      sep = ",";
    }
    if (void_ptr_ != nullptr) {
      to_return += sep + "void_ptr_ address: " +
          string_utils::Itoa(reinterpret_cast<uintptr_t>(void_ptr_));
      sep = ",";
    }
    return to_return;
  }

  bool Equals(const SocketIdentifier & other) const {
    // We say two SocketIdentifiers match if all fields match, and:
    //   1) They have the same (non-default) socket_id_; OR
    //   2) They have the same (non-default) socket_index_; OR
    //   3) They have the same (non-empty) IP and PORT; OR
    //   4) They have the same (non-empty) label
    return socket_id_ == other.socket_id_ &&
        other.socket_index_ == socket_index_ && other.ip_ == ip_ &&
        other.port_ == port_ && other.label_ == label_ &&
        (socket_id_ >= 0 || socket_index_ >= 0 ||
         (!ip_.empty() && port_ > 0 && other.ip_ == ip_ &&
          other.port_ == port_));
  }
};

// Holds the statistics for this socket.
struct SocketStats {
  // Error Stats.
  int num_errors_seen_;
  int num_event_errors_;
  int num_client_socket_errors_;
  int num_server_socket_errors_;
  int num_bind_errors_;
  int num_select_errors_;
  int num_receive_errors_;
  int num_send_errors_;
  int num_connection_errors_;
  int num_disconnect_errors_;

  // Send/Receive stats.
  int num_peak_connections_;
  int num_current_connections_;
  int num_distinct_connections_seen_;
  int num_received_null_characters_;
  uint64_t msec_since_last_event_;
  uint64_t msec_waiting_to_connect_;
  uint64_t msec_waiting_to_connect_last_;
  uint64_t num_sent_bytes_;
  int num_sent_transactions_;
  uint64_t num_received_bytes_;
  int num_received_transactions_;
  // Global counts (not deleted on "Light" Resets).
  uint64_t num_global_sent_bytes_;
  int num_global_sent_transactions_;
  uint64_t num_global_received_bytes_;
  int num_global_received_transactions_;

  // Counts for how many times various functions were called.
  int num_setup_socket_calls_;
  int num_connect_calls_;
  int num_bind_calls_;
  int num_listen_calls_;
  int num_remove_connection_calls_;
  int num_close_socket_calls_;
  int num_times_listen_loop_called_;
  int num_times_in_listen_loop_;
  int num_open_new_connection_calls_;
  int num_server_receive_data_calls_;
  int num_send_data_calls_;
  int num_send_data_calls_temp_;
  int num_inside_send_data_calls_;
  int num_bind_sleep_calls_;
  int num_connect_sleep_calls_;
  int num_failed_connect_sleep_calls_;

  // Timers.
  bool activate_timers_;
  uint64_t bind_sleep_timer_;
  uint64_t connect_sleep_timer_;
  long long int last_connect_call_start_;
  long long int time_from_last_connect_call_;
  long long int time_at_last_failed_connect_call_;
  long long int time_from_last_failed_connect_call_;
  long long int connect_last_timer_;
  uint64_t failed_connect_sleep_timer_;
  test_utils::Timer initialization_timer_;
  test_utils::Timer disconnect_timer_;
  test_utils::Timer setup_socket_timer_;
  test_utils::Timer connect_timer_;
  test_utils::Timer remove_connection_timer_;
  test_utils::Timer close_socket_timer_;
  test_utils::Timer client_close_socket_timer_;
  test_utils::Timer server_close_socket_timer_;
  test_utils::Timer bind_timer_;
  test_utils::Timer listen_timer_;
  test_utils::Timer listen_select_timer_;
  test_utils::Timer listen_loop_timer_;
  test_utils::Timer new_connection_timer_;
  test_utils::Timer server_receive_data_timer_;
  test_utils::Timer server_awaiting_client_timer_;
  test_utils::Timer client_awaiting_server_timer_;
  test_utils::Timer send_data_timer_;
  test_utils::Timer send_data_select_timer_;
  test_utils::Timer send_timer_;

  std::set<ListenReturnCode> abort_listening_with_code_;

  SocketStats() {
    activate_timers_ = false;
    bind_sleep_timer_ = 0;
    connect_sleep_timer_ = 0;
    last_connect_call_start_ = 0;
    time_from_last_connect_call_ = 0;
    time_from_last_failed_connect_call_ = 0;
    time_at_last_failed_connect_call_ = 0;
    connect_last_timer_ = 0;
    failed_connect_sleep_timer_ = 0;
    ResetFunctionCalls();
    ResetErrors();
    ResetSendReceiveCounts();
    ResetGlobal();
    ResetTimerFields();
  }

  void ResetFunctionCalls() {
    num_setup_socket_calls_ = 0;
    num_connect_calls_ = 0;
    num_bind_calls_ = 0;
    num_listen_calls_ = 0;
    num_remove_connection_calls_ = 0;
    num_close_socket_calls_ = 0;
    num_times_listen_loop_called_ = 0;
    num_times_in_listen_loop_ = 0;
    num_open_new_connection_calls_ = 0;
    num_server_receive_data_calls_ = 0;
    num_send_data_calls_ = 0;
    num_send_data_calls_temp_ = 0;
    num_inside_send_data_calls_ = 0;
  }

  void ResetErrorsLight() {
    num_event_errors_ = 0;
    num_client_socket_errors_ = 0;
    num_server_socket_errors_ = 0;
  }

  void ResetErrors() {
    ResetErrorsLight();
    num_errors_seen_ = 0;
    num_bind_errors_ = 0;
    num_select_errors_ = 0;
    num_receive_errors_ = 0;
    num_send_errors_ = 0;
    num_connection_errors_ = 0;
    num_disconnect_errors_ = 0;
  }

  void ResetSendReceiveCounts() {
    num_peak_connections_ = 0;
    num_current_connections_ = 0;
    num_distinct_connections_seen_ = 0;
    num_received_null_characters_ = 0;
    abort_listening_with_code_.clear();
    num_sent_bytes_ = 0;
    num_sent_transactions_ = 0;
    num_received_bytes_ = 0;
    num_received_transactions_ = 0;
  }

  void ResetTimerFields() {
    bind_sleep_timer_ = 0;
    connect_sleep_timer_ = 0;
    last_connect_call_start_ = 0;
    time_from_last_connect_call_ = 0;
    time_from_last_failed_connect_call_ = 0;
    time_at_last_failed_connect_call_ = 0;
    connect_last_timer_ = 0;
    failed_connect_sleep_timer_ = 0;
    msec_since_last_event_ = 0;
    msec_waiting_to_connect_ = 0;
    msec_waiting_to_connect_last_ = 0;
  }

  void ResetTimers() {
    test_utils::ResetTimer(&initialization_timer_);
    test_utils::ResetTimer(&disconnect_timer_);
    test_utils::ResetTimer(&setup_socket_timer_);
    test_utils::ResetTimer(&connect_timer_);
    test_utils::ResetTimer(&remove_connection_timer_);
    test_utils::ResetTimer(&close_socket_timer_);
    test_utils::ResetTimer(&client_close_socket_timer_);
    test_utils::ResetTimer(&server_close_socket_timer_);
    test_utils::ResetTimer(&bind_timer_);
    test_utils::ResetTimer(&listen_timer_);
    test_utils::ResetTimer(&listen_select_timer_);
    test_utils::ResetTimer(&listen_loop_timer_);
    test_utils::ResetTimer(&new_connection_timer_);
    test_utils::ResetTimer(&server_receive_data_timer_);
    test_utils::ResetTimer(&server_awaiting_client_timer_);
    test_utils::ResetTimer(&client_awaiting_server_timer_);
    test_utils::ResetTimer(&send_data_timer_);
    test_utils::ResetTimer(&send_data_select_timer_);
    test_utils::ResetTimer(&send_timer_);
  }

  void ResetAllTimers() {
    ResetTimerFields();
    ResetTimers();
  }

  void ResetGlobal() {
    num_global_sent_bytes_ = 0;
    num_global_sent_transactions_ = 0;
    num_global_received_bytes_ = 0;
    num_global_received_transactions_ = 0;
    num_bind_sleep_calls_ = 0;
    num_connect_sleep_calls_ = 0;
    num_failed_connect_sleep_calls_ = 0;
  }

  void ResetAll() {
    ResetFunctionCalls();
    ResetSendReceiveCounts();
    ResetErrors();
    ResetAllTimers();
    ResetGlobal();
  }

  std::string Print(
      const bool print_errors,
      const bool print_send_receive,
      const bool print_global_send_receive,
      const bool print_num_calls,
      const bool print_timers) const {
    std::string to_return = "";
    if (print_errors) {
      to_return += "\nError Counters:\n";
      if (num_errors_seen_ > 0) {
        to_return += "  num_errors_seen_: " +
            string_utils::Itoa(num_errors_seen_) + "\n";
      }
      if (num_event_errors_ > 0) {
        to_return += "  num_event_errors_: " +
            string_utils::Itoa(num_event_errors_) + "\n";
      }
      if (num_client_socket_errors_ > 0) {
        to_return += "  num_client_socket_errors_: " +
            string_utils::Itoa(num_client_socket_errors_) + "\n";
      }
      if (num_server_socket_errors_ > 0) {
        to_return += "  num_server_socket_errors_: " +
            string_utils::Itoa(num_server_socket_errors_) + "\n";
      }
      if (num_bind_errors_ > 0) {
        to_return += "  num_bind_errors_: " +
            string_utils::Itoa(num_bind_errors_) + "\n";
      }
      if (num_select_errors_ > 0) {
        to_return += "  num_select_errors_: " +
            string_utils::Itoa(num_select_errors_) + "\n";
      }
      if (num_receive_errors_ > 0) {
        to_return += "  num_receive_errors_: " +
            string_utils::Itoa(num_receive_errors_) + "\n";
      }
      if (num_send_errors_ > 0) {
        to_return += "  num_send_errors_: " +
            string_utils::Itoa(num_send_errors_) + "\n";
      }
      if (num_connection_errors_ > 0) {
        to_return += "  num_connection_errors_: " +
            string_utils::Itoa(num_connection_errors_) + "\n";
      }
      if (num_disconnect_errors_ > 0) {
        to_return += "  num_disconnect_errors_: " +
            string_utils::Itoa(num_disconnect_errors_) + "\n";
      }
    }

    // Send/Receive stats.
    if (print_send_receive) {
      to_return += "  \nMost Recent Send/Receive Stats:\n";
      if (num_peak_connections_ > 0) {
        to_return += "  num_peak_connections_: " +
            string_utils::Itoa(num_peak_connections_) + "\n";
      }
      if (num_current_connections_ > 0) {
        to_return += "  num_current_connections_: " +
            string_utils::Itoa(num_current_connections_) + "\n";
      }
      if (num_distinct_connections_seen_ > 0) {
        to_return += "  num_distinct_connections_seen_: " +
            string_utils::Itoa(num_distinct_connections_seen_) + "\n";
      }
      if (num_sent_bytes_ > 0) {
        to_return += "  num_sent_bytes_: " +
            string_utils::Itoa(num_sent_bytes_) + "\n";
      }
      if (msec_since_last_event_ > 0) {
        to_return += "  msec_since_last_event_: " +
            string_utils::Itoa(msec_since_last_event_) + "\n";
      }
      if (msec_waiting_to_connect_ > 0) {
        to_return += "  msec_waiting_to_connect_: " +
            string_utils::Itoa(msec_waiting_to_connect_) + "\n";
      }
      if (msec_waiting_to_connect_last_ > 1000) {
        to_return += "  msec_waiting_to_connect_last_: " +
            string_utils::Itoa(msec_waiting_to_connect_last_ / 1000) +
            "\n";
      }
      if (num_sent_transactions_ > 0) {
        to_return += "  num_sent_transactions_: " +
            string_utils::Itoa(num_sent_transactions_) + "\n";
      }
      if (num_received_bytes_ > 0) {
        to_return += "  num_received_bytes_: " +
            string_utils::Itoa(num_received_bytes_) + "\n";
      }
      if (num_received_transactions_ > 0) {
        to_return += "  num_received_transactions_: " +
            string_utils::Itoa(num_received_transactions_) + "\n";
      }
      if (num_received_null_characters_ > 0) {
        to_return += "  num_received_null_characters_: " +
            string_utils::Itoa(num_received_null_characters_) + "\n";
      }
      if (!abort_listening_with_code_.empty()) {
        to_return +=
            "  Listen Abort Codes: " +
            string_utils::Join(
                GetListenReturnCodeList(abort_listening_with_code_),
                ", ");
      }
    }

    // Global Send/Receive stats.
    if (print_global_send_receive) {
      to_return += "  \nGlobal Send/Receive Stats:\n";
      if (num_global_sent_bytes_ > 0) {
        to_return += "  num_global_sent_bytes_: " +
            string_utils::Itoa(num_global_sent_bytes_) + "\n";
      }
      if (num_global_sent_transactions_ > 0) {
        to_return += "  num_global_sent_transactions_: " +
            string_utils::Itoa(num_global_sent_transactions_) + "\n";
      }
      if (num_global_received_bytes_ > 0) {
        to_return += "  num_global_received_bytes_: " +
            string_utils::Itoa(num_global_received_bytes_) + "\n";
      }
      if (num_global_received_transactions_ > 0) {
        to_return += "  num_global_received_transactions_: " +
            string_utils::Itoa(num_global_received_transactions_) +
            "\n";
      }
      if (num_bind_sleep_calls_ > 0) {
        to_return += "  num_bind_sleep_calls_: " +
            string_utils::Itoa(num_bind_sleep_calls_) + "\n";
      }
      if (num_connect_sleep_calls_ > 0) {
        to_return += "  num_connect_sleep_calls_: " +
            string_utils::Itoa(num_connect_sleep_calls_) + "\n";
      }
      if (num_failed_connect_sleep_calls_ > 0) {
        to_return += "  num_failed_connect_sleep_calls_: " +
            string_utils::Itoa(num_failed_connect_sleep_calls_) + "\n";
      }
    }

    // Counts for how many times various functions were called.
    if (print_num_calls) {
      to_return += "  \nFunction calls:\n";
      if (num_setup_socket_calls_ > 0) {
        to_return += "  num_setup_socket_calls_: " +
            string_utils::Itoa(num_setup_socket_calls_) + "\n";
      }
      if (num_connect_calls_ > 0) {
        to_return += "  num_connect_calls_: " +
            string_utils::Itoa(num_connect_calls_) + "\n";
      }
      if (num_bind_calls_ > 0) {
        to_return += "  num_bind_calls_: " +
            string_utils::Itoa(num_bind_calls_) + "\n";
      }
      if (num_listen_calls_ > 0) {
        to_return += "  num_listen_calls_: " +
            string_utils::Itoa(num_listen_calls_) + "\n";
      }
      if (num_remove_connection_calls_ > 0) {
        to_return += "  num_remove_connection_calls_: " +
            string_utils::Itoa(num_remove_connection_calls_) + "\n";
      }
      if (num_close_socket_calls_ > 0) {
        to_return += "  num_close_socket_calls_: " +
            string_utils::Itoa(num_close_socket_calls_) + "\n";
      }
      if (num_times_listen_loop_called_ > 0) {
        to_return += "  num_times_listen_loop_called_: " +
            string_utils::Itoa(num_times_listen_loop_called_) + "\n";
      }
      if (num_times_in_listen_loop_ > 0) {
        to_return += "  num_times_in_listen_loop_: " +
            string_utils::Itoa(num_times_in_listen_loop_) + "\n";
      }
      if (num_open_new_connection_calls_ > 0) {
        to_return += "  num_open_new_connection_calls_: " +
            string_utils::Itoa(num_open_new_connection_calls_) + "\n";
      }
      if (num_server_receive_data_calls_ > 0) {
        to_return += "  num_server_receive_data_calls_: " +
            string_utils::Itoa(num_server_receive_data_calls_) + "\n";
      }
      if (num_send_data_calls_ > 0) {
        to_return += "  num_send_data_calls_: " +
            string_utils::Itoa(num_send_data_calls_) + "\n";
      }
      if (num_send_data_calls_temp_ > 0) {
        to_return += "  num_send_data_calls_temp_: " +
            string_utils::Itoa(num_send_data_calls_temp_) + "\n";
      }
      if (num_inside_send_data_calls_ > 0) {
        to_return += "  num_inside_send_data_calls_: " +
            string_utils::Itoa(num_inside_send_data_calls_) + "\n";
      }
    }

    // Print timers.
    if (print_timers && activate_timers_) {
      to_return += "  \nTimers:\n";
      const int meaningful_ms = 10;
      if (bind_sleep_timer_ / 1000 > 0 ||
          bind_sleep_timer_ % 1000 > meaningful_ms) {
        to_return += "  bind_sleep_timer_: " +
            test_utils::FormatTime(bind_sleep_timer_) + "\n";
      }
      if (connect_sleep_timer_ / 1000 > 0 ||
          connect_sleep_timer_ % 1000 > meaningful_ms) {
        to_return += "  connect_sleep_timer_: " +
            test_utils::FormatTime(connect_sleep_timer_) + "\n";
      }
      if (connect_last_timer_ > 1000) {
        to_return += "  connect_last_timer_: " +
            test_utils::FormatTime(connect_last_timer_ / 1000) + "\n";
      }
      if (failed_connect_sleep_timer_ / 1000 > 0 ||
          failed_connect_sleep_timer_ % 1000 > meaningful_ms) {
        to_return += "  failed_connect_sleep_timer_: " +
            test_utils::FormatTime(failed_connect_sleep_timer_ / 1000) +
            "\n";
      }
      const int64_t initialization_time =
          GetElapsedTime(initialization_timer_) / 1000;
      if (initialization_time / 1000 > 0 ||
          initialization_time % 1000 > meaningful_ms) {
        to_return += "  initialization_timer_: " +
            test_utils::FormatTime(initialization_time) + "\n";
      }
      const int64_t disconnect_time =
          GetElapsedTime(disconnect_timer_) / 1000;
      if (disconnect_time / 1000 > 0 ||
          disconnect_time % 1000 > meaningful_ms) {
        to_return += "  disconnect_timer_: " +
            test_utils::FormatTime(disconnect_time) + "\n";
      }
      const int64_t setup_client_socket_time =
          GetElapsedTime(setup_socket_timer_) / 1000;
      if (setup_client_socket_time / 1000 > 0 ||
          setup_client_socket_time % 1000 > meaningful_ms) {
        to_return += "  setup_socket_timer_: " +
            test_utils::FormatTime(setup_client_socket_time) + "\n";
      }
      const int64_t connect_time =
          GetElapsedTime(connect_timer_) / 1000;
      if (connect_time / 1000 > 0 ||
          connect_time % 1000 > meaningful_ms) {
        to_return += "  connect_timer_: " +
            test_utils::FormatTime(connect_time) + "\n";
      }
      const int64_t remove_connection_time =
          GetElapsedTime(remove_connection_timer_) / 1000;
      if (remove_connection_time / 1000 > 0 ||
          remove_connection_time % 1000 > meaningful_ms) {
        to_return += "  remove_connection_timer_: " +
            test_utils::FormatTime(remove_connection_time) + "\n";
      }
      const int64_t close_socket_time =
          GetElapsedTime(close_socket_timer_) / 1000;
      if (close_socket_time / 1000 > 0 ||
          close_socket_time % 1000 > meaningful_ms) {
        to_return += "  close_socket_timer_: " +
            test_utils::FormatTime(close_socket_time) + "\n";
      }
      const int64_t server_close_socket_time =
          GetElapsedTime(server_close_socket_timer_) / 1000;
      if (server_close_socket_time / 1000 > 0 ||
          server_close_socket_time % 1000 > meaningful_ms) {
        to_return += "  server_close_socket_timer_: " +
            test_utils::FormatTime(server_close_socket_time) + "\n";
      }
      const int64_t client_close_socket_time =
          GetElapsedTime(client_close_socket_timer_) / 1000;
      if (client_close_socket_time / 1000 > 0 ||
          client_close_socket_time % 1000 > meaningful_ms) {
        to_return += "  client_close_socket_timer_: " +
            test_utils::FormatTime(client_close_socket_time) + "\n";
      }
      const int64_t bind_time = GetElapsedTime(bind_timer_) / 1000;
      if (bind_time / 1000 > 0 || bind_time % 1000 > meaningful_ms) {
        to_return +=
            "  bind_timer_: " + test_utils::FormatTime(bind_time) +
            "\n";
      }
      const int64_t listen_time = GetElapsedTime(listen_timer_) / 1000;
      if (listen_time / 1000 > 0 ||
          listen_time % 1000 > meaningful_ms) {
        to_return +=
            "  listen_timer_: " + test_utils::FormatTime(listen_time) +
            "\n";
      }
      const int64_t listen_select_time =
          GetElapsedTime(listen_select_timer_) / 1000;
      if (listen_select_time / 1000 > 0 ||
          listen_select_time % 1000 > meaningful_ms) {
        to_return += "  listen_select_timer_: " +
            test_utils::FormatTime(listen_select_time) + "\n";
      }
      const int64_t listen_loop_time =
          GetElapsedTime(listen_loop_timer_) / 1000;
      if (listen_loop_time / 1000 > 0 ||
          listen_loop_time % 1000 > meaningful_ms) {
        to_return += "  listen_loop_timer_: " +
            test_utils::FormatTime(listen_loop_time) + "\n";
      }
      const int64_t new_connection_time =
          GetElapsedTime(new_connection_timer_) / 1000;
      if (new_connection_time / 1000 > 0 ||
          new_connection_time % 1000 > meaningful_ms) {
        to_return += "  new_connection_timer_: " +
            test_utils::FormatTime(new_connection_time) + "\n";
      }
      const int64_t server_receive_data_time =
          GetElapsedTime(server_receive_data_timer_) / 1000;
      if (server_receive_data_time / 1000 > 0 ||
          server_receive_data_time % 1000 > meaningful_ms) {
        to_return += "  server_receive_data_timer_: " +
            test_utils::FormatTime(server_receive_data_time) + "\n";
      }
      const int64_t server_awaiting_client_time =
          GetElapsedTime(server_awaiting_client_timer_) / 1000;
      if (server_awaiting_client_time / 1000 > 0 ||
          server_awaiting_client_time % 1000 > meaningful_ms) {
        to_return += "  server_awaiting_client_timer_: " +
            test_utils::FormatTime(server_awaiting_client_time) + "\n";
      }
      const int64_t client_awaiting_server_time =
          GetElapsedTime(client_awaiting_server_timer_) / 1000;
      if (client_awaiting_server_time / 1000 > 0 ||
          client_awaiting_server_time % 1000 > meaningful_ms) {
        to_return += "  client_awaiting_server_timer_: " +
            test_utils::FormatTime(client_awaiting_server_time) + "\n";
      }
      const int64_t send_data_time =
          GetElapsedTime(send_data_timer_) / 1000;
      if (send_data_time / 1000 > 0 ||
          send_data_time % 1000 > meaningful_ms) {
        to_return += "  send_data_timer_: " +
            test_utils::FormatTime(send_data_time) + "\n";
      }
      const int64_t send_data_select_time =
          GetElapsedTime(send_data_select_timer_) / 1000;
      if (send_data_select_time / 1000 > 0 ||
          send_data_select_time % 1000 > meaningful_ms) {
        to_return += "  send_data_select_timer_: " +
            test_utils::FormatTime(send_data_select_time) + "\n";
      }
      const int64_t send_time = GetElapsedTime(send_timer_) / 1000;
      if (send_time / 1000 > 0 || send_time % 1000 > meaningful_ms) {
        to_return +=
            "  send_timer_: " + test_utils::FormatTime(send_time) +
            "\n";
      }
    }

    return to_return;
  }

  std::string Print() const {
    return Print(true, true, true, true, true);
  }
};

// This structure holds information about the data received (across a given
// connection).
struct ReceivedData {
  std::vector<char> buffer_;
  // Whether the buffer_ is currently receiving data (i.e. don't use it yet).
  bool is_receiving_data_;

  ReceivedData() {
    is_receiving_data_ = false;
  }
};

struct DataToSend {
  std::vector<char> buffer_;
  uint64_t num_chars_sent_;

  // Whether the buffer_ is currently sending data (i.e. don't use it).
  bool is_sending_data_;

  DataToSend() {
    is_sending_data_ = false;
    num_chars_sent_ = 0;
  }
};

// Fields can be used to create conditions under which calls to Listen()
// should return; e.g. when enough connections are formed, or when a
// certain number of back/forth transactions have occurred, etc.
// These may be chained together to form arbitrary expressions,
// for example for desired Stop Condition:
//    ((distinct_connections_seen >= 10) && (current_connections == 0)) ||
//    (error_seen)
// we would have the "root" StopListeningCondition:
//   - op_ = OR;
//   - left_condition_ = A;
//   - right_condition_ = B;
// with left_condition_ A:
//   - op_ = AND;
//   - left_condition_ = C;
//   - right_condition_ = D;
// with right_condition_ B:
//   - op_ = IDENTITY;
//   - num_errors = 1;
// with left_condition_ C:
//   - op_ = IDENTITY;
//   - distinct_connections_seen_ = 10;
// with right_condition_ D:
//   - op_ = IDENTITY;
//   - num_current_connections_ = 0;
// In particular, a given StopListeningCondition object should either have op_
// indicate IDENTITY (in which case exactly one field has a non-default value),
// or else all fields should have default value (and are ignored) and op_ is
// used to combine the left/right_condition_.
struct StopListeningCondition {
  std::unique_ptr<StopListeningCondition> left_condition_;
  std::unique_ptr<StopListeningCondition> right_condition_;
  ConditionOperation op_;

  // Possible fields to check; at most one should be set from non-default values.
  //   - Abort if this many errors have been encountered
  unsigned int num_errors_seen_;
  //   - Abort if this many Event Errors have been encountered
  unsigned int num_event_errors_seen_;
  //   - Abort if this many select() Errors have been encountered
  unsigned int num_select_errors_seen_;
  //   - Abort if this many send_errors have been encountered
  unsigned int num_send_errors_seen_;
  //   - Abort if this many receive errors have been encountered
  unsigned int num_receive_errors_seen_;
  //   - Abort if this many errors on the Server Socket have been encountered
  unsigned int num_server_errors_seen_;
  //   - Abort if this many errors on the Client Socket have been encountered
  unsigned int num_client_errors_seen_;
  //   - Abort if this many connection errors have been encountered
  unsigned int num_connection_errors_seen_;
  //   - Abort if this many disconnect errors have been encountered
  unsigned int num_disconnect_errors_seen_;
  //   - Abort if the amount of time from the last transaction exceeds this.
  //     NOTE: This only is applicable/usable if select_timeout_ms_ is set
  //     (positive), since otherwise select() is BLOCKING until an Event occurs.
  uint64_t num_msec_since_last_event_;
  //   - Abort if at any time the number of connections meets this
  int num_current_connections_;
  //   - Abort if the maximum number of distinct connections (not nec. at same time) meets this
  int num_distinct_connections_seen_;
  //   - Abort if the number of bytes sent (across all connections) meets this
  int num_sent_bytes_;
  //   - Abort if the number of messages sent (across all connections) meets this
  int num_sent_transactions_;
  //   - Abort if the number of bytes received (across all connections) meets this
  int num_received_bytes_;
  //   - Abort if the number of messages received (across all connections) meets this
  int num_received_transactions_;
  //   - Abort if the number of null chars '\0' received (across all connections) meets this
  //     For example, if you just have one connection and you want to Stop Listening
  //     when it receives the null character, set this value to '1'.
  //     NOTE/WARNING: For connections that utilize the null character to mark the
  //     end of a message, note that this character will be stored (as the last
  //     character) in the received data buffer (received_bytes_per_socket_).
  //     It is up to the user to ignore/delete this character when parsing
  //     the received data buffer (i.e. the character is not automatically
  //     deleted).
  int num_received_null_characters_;

  StopListeningCondition() {
    // Default Stop Condition will be to abort if any error occurs.
    Reset(1);
  }
  explicit StopListeningCondition(const unsigned int num_errors) {
    Reset(num_errors);
  }

  void Reset(const unsigned int num_errors) {
    left_condition_ = nullptr;
    right_condition_ = nullptr;
    op_ = ConditionOperation::IDENTITY;
    num_errors_seen_ = num_errors;
    num_select_errors_seen_ = 0;
    num_event_errors_seen_ = 0;
    num_send_errors_seen_ = 0;
    num_receive_errors_seen_ = 0;
    num_server_errors_seen_ = 0;
    num_client_errors_seen_ = 0;
    num_connection_errors_seen_ = 0;
    num_disconnect_errors_seen_ = 0;
    num_msec_since_last_event_ = 0;
    num_current_connections_ = -1;
    num_distinct_connections_seen_ = -1;
    num_sent_bytes_ = -1;
    num_sent_transactions_ = -1;
    num_received_bytes_ = -1;
    num_received_transactions_ = -1;
    num_received_null_characters_ = -1;
  }
  // For users who call Reset without any parameters, default behavior will be
  // to set num_errors_seen to zero.
  void Reset() {
    Reset(0);
  }

  // ========================== Rule of 5 Functions ============================
  // Copy Constructor.
  StopListeningCondition(const StopListeningCondition & other) {
    op_ = other.op_;
    num_errors_seen_ = other.num_errors_seen_;
    num_select_errors_seen_ = other.num_select_errors_seen_;
    num_event_errors_seen_ = other.num_event_errors_seen_;
    num_send_errors_seen_ = other.num_send_errors_seen_;
    num_receive_errors_seen_ = other.num_receive_errors_seen_;
    num_server_errors_seen_ = other.num_server_errors_seen_;
    num_client_errors_seen_ = other.num_client_errors_seen_;
    num_connection_errors_seen_ = other.num_connection_errors_seen_;
    num_disconnect_errors_seen_ = other.num_disconnect_errors_seen_;
    num_msec_since_last_event_ = other.num_msec_since_last_event_;
    num_current_connections_ = other.num_current_connections_;
    num_distinct_connections_seen_ =
        other.num_distinct_connections_seen_;
    num_sent_bytes_ = other.num_sent_bytes_;
    num_sent_transactions_ = other.num_sent_transactions_;
    num_received_bytes_ = other.num_received_bytes_;
    num_received_transactions_ = other.num_received_transactions_;
    num_received_null_characters_ = other.num_received_null_characters_;
    if (other.left_condition_ != nullptr) {
      left_condition_ = std::unique_ptr<StopListeningCondition>(
          new StopListeningCondition());
      left_condition_->clone(*other.left_condition_);
    }
    if (other.right_condition_ != nullptr) {
      right_condition_ = std::unique_ptr<StopListeningCondition>(
          new StopListeningCondition());
      right_condition_->clone(*other.right_condition_);
    }
  }
  // Move Constructor.
  StopListeningCondition(StopListeningCondition && other) noexcept {
    op_ = other.op_;
    num_errors_seen_ = other.num_errors_seen_;
    num_select_errors_seen_ = other.num_select_errors_seen_;
    num_event_errors_seen_ = other.num_event_errors_seen_;
    num_send_errors_seen_ = other.num_send_errors_seen_;
    num_receive_errors_seen_ = other.num_receive_errors_seen_;
    num_server_errors_seen_ = other.num_server_errors_seen_;
    num_client_errors_seen_ = other.num_client_errors_seen_;
    num_connection_errors_seen_ = other.num_connection_errors_seen_;
    num_disconnect_errors_seen_ = other.num_disconnect_errors_seen_;
    num_msec_since_last_event_ = other.num_msec_since_last_event_;
    num_current_connections_ = other.num_current_connections_;
    num_distinct_connections_seen_ =
        other.num_distinct_connections_seen_;
    num_sent_bytes_ = other.num_sent_bytes_;
    num_sent_transactions_ = other.num_sent_transactions_;
    num_received_bytes_ = other.num_received_bytes_;
    num_received_transactions_ = other.num_received_transactions_;
    num_received_null_characters_ = other.num_received_null_characters_;
    if (other.left_condition_ != nullptr) {
      left_condition_ = std::move(other.left_condition_);
    }
    if (other.right_condition_ != nullptr) {
      right_condition_ = std::move(other.right_condition_);
    }
  }
  // Copy-Assignment.
  StopListeningCondition &
  operator=(const StopListeningCondition & other) {
    StopListeningCondition temp(other); // Re-use copy-constructor.
    *this = std::move(temp); // Re-use move-assignment.
    return *this;
  }
  // Move-Assignment.
  StopListeningCondition &
  operator=(StopListeningCondition && other) noexcept {
    op_ = other.op_;
    num_errors_seen_ = other.num_errors_seen_;
    num_select_errors_seen_ = other.num_select_errors_seen_;
    num_event_errors_seen_ = other.num_event_errors_seen_;
    num_send_errors_seen_ = other.num_send_errors_seen_;
    num_receive_errors_seen_ = other.num_receive_errors_seen_;
    num_server_errors_seen_ = other.num_server_errors_seen_;
    num_client_errors_seen_ = other.num_client_errors_seen_;
    num_connection_errors_seen_ = other.num_connection_errors_seen_;
    num_disconnect_errors_seen_ = other.num_disconnect_errors_seen_;
    num_msec_since_last_event_ = other.num_msec_since_last_event_;
    num_current_connections_ = other.num_current_connections_;
    num_distinct_connections_seen_ =
        other.num_distinct_connections_seen_;
    num_sent_bytes_ = other.num_sent_bytes_;
    num_sent_transactions_ = other.num_sent_transactions_;
    num_received_bytes_ = other.num_received_bytes_;
    num_received_transactions_ = other.num_received_transactions_;
    num_received_null_characters_ = other.num_received_null_characters_;
    if (other.left_condition_ != nullptr) {
      left_condition_ = std::move(other.left_condition_);
    }
    if (other.right_condition_ != nullptr) {
      right_condition_ = std::move(other.right_condition_);
    }
    return *this;
  }

  // Destructor.
  ~StopListeningCondition() noexcept {
  }

  // Deep-Copy from other.
  void clone(const StopListeningCondition & other) {
    op_ = other.op_;
    num_errors_seen_ = other.num_errors_seen_;
    num_select_errors_seen_ = other.num_select_errors_seen_;
    num_event_errors_seen_ = other.num_event_errors_seen_;
    num_send_errors_seen_ = other.num_send_errors_seen_;
    num_receive_errors_seen_ = other.num_receive_errors_seen_;
    num_server_errors_seen_ = other.num_server_errors_seen_;
    num_client_errors_seen_ = other.num_client_errors_seen_;
    num_connection_errors_seen_ = other.num_connection_errors_seen_;
    num_disconnect_errors_seen_ = other.num_disconnect_errors_seen_;
    num_msec_since_last_event_ = other.num_msec_since_last_event_;
    num_current_connections_ = other.num_current_connections_;
    num_distinct_connections_seen_ =
        other.num_distinct_connections_seen_;
    num_sent_bytes_ = other.num_sent_bytes_;
    num_sent_transactions_ = other.num_sent_transactions_;
    num_received_bytes_ = other.num_received_bytes_;
    num_received_transactions_ = other.num_received_transactions_;
    num_received_null_characters_ = other.num_received_null_characters_;
    if (other.left_condition_ != nullptr) {
      left_condition_ = std::unique_ptr<StopListeningCondition>(
          new StopListeningCondition());
      left_condition_->clone(*other.left_condition_);
    }
    if (other.right_condition_ != nullptr) {
      right_condition_ = std::unique_ptr<StopListeningCondition>(
          new StopListeningCondition());
      right_condition_->clone(*other.right_condition_);
    }
  }

  // Prints the non-recursive Stop Condition; i.e. the member variables that have
  // non-default values.
  std::string PrintLocal() const {
    std::string to_return = "";
    std::string sep = "";
    if (num_errors_seen_ > 0) {
      to_return += sep +
          "num_errors_seen_: " + string_utils::Itoa(num_errors_seen_);
      sep = "\n";
    }
    if (num_select_errors_seen_ > 0) {
      to_return += sep + "num_select_errors_seen_: " +
          string_utils::Itoa(num_select_errors_seen_);
      sep = "\n";
    }
    if (num_event_errors_seen_ > 0) {
      to_return += sep + "num_event_errors_seen_: " +
          string_utils::Itoa(num_event_errors_seen_);
      sep = "\n";
    }
    if (num_send_errors_seen_ > 0) {
      to_return += sep + "num_send_errors_seen_: " +
          string_utils::Itoa(num_send_errors_seen_);
      sep = "\n";
    }
    if (num_receive_errors_seen_ > 0) {
      to_return += sep + "num_receive_errors_seen_: " +
          string_utils::Itoa(num_receive_errors_seen_);
      sep = "\n";
    }
    if (num_server_errors_seen_ > 0) {
      to_return += sep + "num_server_errors_seen_: " +
          string_utils::Itoa(num_server_errors_seen_);
      sep = "\n";
    }
    if (num_client_errors_seen_ > 0) {
      to_return += sep + "num_client_errors_seen_: " +
          string_utils::Itoa(num_client_errors_seen_);
      sep = "\n";
    }
    if (num_connection_errors_seen_ > 0) {
      to_return += sep + "num_connection_errors_seen_: " +
          string_utils::Itoa(num_connection_errors_seen_);
      sep = "\n";
    }
    if (num_disconnect_errors_seen_ > 0) {
      to_return += sep + "num_disconnect_errors_seen_: " +
          string_utils::Itoa(num_disconnect_errors_seen_);
      sep = "\n";
    }
    if (num_msec_since_last_event_ > 0) {
      to_return += sep + "num_msec_since_last_event_: " +
          string_utils::Itoa(num_msec_since_last_event_);
      sep = "\n";
    }
    if (num_current_connections_ >= 0) {
      to_return += sep + "num_current_connections_: " +
          string_utils::Itoa(num_current_connections_);
      sep = "\n";
    }
    if (num_distinct_connections_seen_ >= 0) {
      to_return += sep + "num_distinct_connections_seen_: " +
          string_utils::Itoa(num_distinct_connections_seen_);
      sep = "\n";
    }
    if (num_sent_bytes_ >= 0) {
      to_return += sep +
          "num_sent_bytes_: " + string_utils::Itoa(num_sent_bytes_);
      sep = "\n";
    }
    if (num_sent_transactions_ >= 0) {
      to_return += sep + "num_sent_transactions_: " +
          string_utils::Itoa(num_sent_transactions_);
      sep = "\n";
    }
    if (num_received_bytes_ >= 0) {
      to_return += sep + "num_received_bytes_: " +
          string_utils::Itoa(num_received_bytes_);
      sep = "\n";
    }
    if (num_received_transactions_ >= 0) {
      to_return += sep + "num_received_transactions_: " +
          string_utils::Itoa(num_received_transactions_);
      sep = "\n";
    }
    if (num_received_null_characters_ >= 0) {
      to_return += sep + "num_received_null_characters_: " +
          string_utils::Itoa(num_received_null_characters_);
      sep = "\n";
    }
    return to_return;
  }

  // Prints the (full, recursive) Stop Condition.
  std::string Print() const {
    if (op_ == ConditionOperation::IDENTITY) {
      return PrintLocal();
    } else if (op_ == ConditionOperation::NOT) {
      if (left_condition_ == nullptr || right_condition_ != nullptr)
        return "UNKNOWN";
      return "!(" + left_condition_->Print() + ")";
    } else if (op_ == ConditionOperation::AND) {
      if (left_condition_ == nullptr || right_condition_ == nullptr)
        return "UNKNOWN";
      return "(" + left_condition_->Print() + ") && (" +
          right_condition_->Print() + ")";
    } else if (op_ == ConditionOperation::OR) {
      if (left_condition_ == nullptr || right_condition_ == nullptr)
        return "UNKNOWN";
      return "(" + left_condition_->Print() + ") || (" +
          right_condition_->Print() + ")";
    } else if (op_ == ConditionOperation::XOR) {
      if (left_condition_ == nullptr || right_condition_ == nullptr)
        return "UNKNOWN";
      return "(" + left_condition_->Print() + ") ^ (" +
          right_condition_->Print() + ")";
    } else {
      return "UNKNOWN";
    }
  }
};

// Wrapper for function pointers that return bool.
typedef bool (*fn_ptr)();
typedef bool (*fn_w_socket_arg_ptr)(const SocketIdentifier &);
// The following can handle arbitrary parameters (wrapped in an appropriate object (pointer)).
typedef bool (*fn_w_args_ptr)(
    const SocketIdentifier &, ReceivedData *, SocketStats *, void *);

// For Server-instantiations, these instruct the Server what to do if the
// ip_ and or port_ is already in use when a Listen() request is initiated.
struct BindParams {
  // Whether Bind() should be called again on failure, and if so, how
  // many times. Use 0 to indicate not to try again, use negative value to
  // indicate we should try again infinitely often; otherwise the value
  // represents how many times to retry Bind().
  int num_retries_;

  // The amount of time (in milliseconds) to wait between Bind() attempts.
  uint64_t sleep_time_;

  // Determines whether sleep time between retries should gradually increase.
  // Specifically, this gives the number of times to wait sleep_time_
  // milliseconds, before increasing sleep_time_ by a factor of 2 (and so on).
  // This value is ignored if non-positive (effectively meaning 'sleep_time_'
  // milliseconds will always be the sleep time).
  int num_tries_at_current_sleep_time_;

  // The message that will be sent to the terminal of the program running
  // this Socket. If print_default_message_ is true, the default message
  // will be printed; otherwise, 'message_' will be printed (no message
  // printed if print_default_message_ is false and 'message_' is empty).
  std::string message_;
  // The default message is:
  //   Failed to Connect to IP::PORT. Retrying [in X seconds]...
  bool print_default_message_;

  BindParams() {
    // Set default sleep time to 5ms
    sleep_time_ = 5;
    // Set default to 200: Thus, with default sleep_time_ above, will keep
    // sleeping 5ms at a time until a total of 1s has elapsed, then start doubling.
    num_tries_at_current_sleep_time_ = 200;
    // Set default to try 2000 times, waiting 0.005 seconds 200 times (1s total),
    // then 0.01 seconds 200 times (2s), 0.02 (4s), 0.04 (8s), 0.08 (16s), 0.16 (32s),
    // and finally 0.32 (64s). So max time waiting is:
    //   200 * (0.005 + 0.01 + ... + 0.32) ~= 128s.
    num_retries_ = 1400;
    message_ = "";
    print_default_message_ = GetVerbosity() > 2;
  }
};

// The following params specify how Listen() is handled. In addition to the main
// listen() parameters (e.g. ip, port, timeout, etc.), these parameters also control
// how listen() knows when to stop listen()ing; there are three main options:
//   1) (Blocking): Whenever any bytes are received, stop listen()ing and return
//      to caller (caller can then determine if enough bytes were received, and
//      if not, explicitly call Listen() again).
//   2) User provides a callback. In this case, each time there is a socket event
//      (specifically, that select() returns positive value), the callback is called
//      to determine if all bytes have been received.
//   3) User sets the 'stop_params_' field of ListenParams, which indicates when all
//      bytes have been received. In particular, the following fields/options are
//      available for 'stop_params_':
// The determination of which of the above three mechanisms is used to abort calls
// to Listen() is controlled primarily by the 'type_' field:
//   - To do (1) above: Make sure that the Socket class is instructed to *not*
//     handle the received bytes; in particular, make sure that the relevant
//     ListenType's are *not* in 'type_'.
//     For example, to do (1) for Connect() requests: type_ should *not* contain:
//       ListenType::HANDLE_NEW_CONNECTIONS nor ListenType::HANDLE_ALL_EVENTS
//     Meanwhile, to do (1) for all non-connect() requests: type_ should *not* contain:
//       ListenType::HANDLE_RECEIVE nor ListenType::HANDLE_ALL_EVENTS
//   - To do (2) above: Make sure that (1) isn't triggered, and make sure the
//     appropriate function callback field is non-null. For example:
//     To do (2) for Connect() requests: handle_new_connection_ must be non-null,
//     and type_ must contain at least one of:
//       ListenType::HANDLE_NEW_CONNECTIONS or ListenType::HANDLE_ALL_EVENTS
//     To do (2) for all non-connect() requests: receive_data_ must be non-null,
//     and type_ must contain at least one of:
//       ListenType::HANDLE_RECEIVE or ListenType::HANDLE_ALL_EVENTS
//   - To do (3) above: First, make sure that (1) and (2) above are not triggered,
//     and that type_ includes ListenType::USE_STOP_CONDITION, and then that
//     stop_params_ should be set to the appropriate stop conditions. For example:
//     To do (3) for Connect() requests: handle_new_connection_ must be null,
//     and type_ must contain ListenType::USE_STOP_CONDITION plus at least one of:
//       ListenType::HANDLE_NEW_CONNECTIONS or ListenType::HANDLE_ALL_EVENTS
//     To do (3) for all non-connect() requests: receive_data_ must be null,
//     and type_ must contain ListenType::USE_STOP_CONDITION plus at least one of:
//       ListenType::HANDLE_RECEIVE or ListenType::HANDLE_ALL_EVENTS
struct ListenParams {
  std::string ip_;
  unsigned long port_;
  uint64_t select_timeout_ms_;
  uint64_t no_event_sleep_ms_;
  int receive_buffer_max_size_;

  // Instructs socket what it should do when socket events occur:
  // handle them (either via default handling or by using a function
  // callback, see fn_ptr fields below) or return control to caller.
  std::set<ListenType> type_;

  // Specifies what to do when attempting to Listen() on an ip/port
  // that is already in use.
  BindParams bind_params_;

  // Specifies the conditions under which Listening should be stopped
  // (and control returned to caller).
  StopListeningCondition stop_params_;

  // Specifies how the socket should handle Listen Events (in the case
  // that the resposibility for handling such events falls on the socket,
  // as opposed to the caller; as determined by type_ field above).
  fn_w_socket_arg_ptr handle_new_connection_;
  fn_w_args_ptr receive_data_;
  // In case the function for Listening uses extra args, this field will
  // store those extra args.
  void * receive_data_args_;

  // DEPRECATED.
  //fn_ptr handle_error_;
  //fn_ptr handle_disconnect_;
  //fn_ptr send_data_;

  ListenParams() {
    ip_ = "";
    port_ = 0;
    select_timeout_ms_ = kSelectTimeoutMilliSecs;
    no_event_sleep_ms_ = kSleepMsBetweenNoEvents;
    receive_buffer_max_size_ = kSocketBufferSize;
    handle_new_connection_ = nullptr;
    receive_data_ = nullptr;
    receive_data_args_ = nullptr;
    // DEPRECATED.
    //handle_error_ = nullptr;
    //handle_disconnect_ = nullptr;
    //send_data_ = nullptr;

    // The default behavior for Listen() will be to handle Event Types:
    // New Connection, Disconnect, Send, Errors; i.e. everything except
    // Receive.
    type_.insert(ListenType::HANDLE_NEW_CONNECTIONS);
    // Warning: This means default behavior is to have Listen() keep
    // listening indefinately. You'll need to either override type_ to
    // not have HANDLE_RECEIVE in type_ (so that caller handles all received
    // data Events), or use one of the stopping conditions (e.g.
    // num_received_transactions_ or num_received_bytes_).
    type_.insert(ListenType::HANDLE_RECEIVE);
    // DEPRECATED.
    // type_.insert(ListenType::HANDLE_DISCONNECTS);
    // type_.insert(ListenType::HANDLE_ERRORS);
    // type_.insert(ListenType::HANDLE_SEND);
  }
};

// For Client-instantiations, these instruct the Client what to do if a
// Connect() attempts fails (e.g. abort, or sleep for awhile then try again).
struct ConnectParams {
  std::string ip_;
  unsigned long port_;

  // Timeout: Stop trying to connect after atempting this many milliseconds.
  // This value only enforced if non-zero (default is 0; i.e. no timeout).
  uint64_t timeout_ms_;

  // connect() can be called in 2 modes: blocking, or non-blocking. The issue
  // is that if a Client attempts to connect() before the Server is listening
  // (via select()), then the connect() request will pend, and then retry
  // a little later, where 'little' is determined by the underlying external
  // library, and is not (easily) configurable. For example, this retry time
  // appears to be ~0.5s on my Windows machine. This retry time may be the
  // bottleneck. If the below field is false, then proceed with the ordinary
  // (blocking) connect(), i.e. the retry cost will be incurred if Server
  // is not listening when Client calls connect(). Otherwise, do non-blocking
  // connect, which will treat each connect() call as a failure, and sleep
  // between them in accordance with sleep_time_ and num_retries_ below.
  // See more discussion above HandleNonBlockingConnect() in windows_socket.cpp.
  bool non_blocking_;

  // Whether Connect() should be called again on failure, and if so, how
  // many times. Use 0 to indicate not to try again, use negative value to
  // indicate we should try again infinitely often; otherwise the value
  // represents how many times to retry Connect().
  int num_retries_;

  // The amount of time (in milliseconds) to wait between (failed) Connect() attempts.
  uint64_t sleep_time_;

  // Determines whether sleep time between retries should gradually increase.
  // Specifically, this gives the number of times to wait sleep_time_
  // milliseconds, before increasing sleep_time_ by a factor of 2 (and so on).
  // This value is ignored if non-positive (effectively meaning 'sleep_time_'
  // milliseconds will always be the sleep time).
  int num_tries_at_current_sleep_time_;

  // The message that will be sent to the terminal of the program running
  // this Socket. If print_default_message_ is true, the default message
  // will be printed; otherwise, 'message_' will be printed (no message
  // printed if print_default_message_ is false and 'message_' is empty).
  std::string message_;
  // The default message is:
  //   Failed to Connect to IP::PORT. Retrying [in X seconds]...
  bool print_default_message_;
  // Controls whether important messages relating to connect() attempts
  // will be printed to terminal.
  bool print_messages_;

  ConnectParams() {
    ip_ = "";
    port_ = 0;
    timeout_ms_ = 0;
    // Set default sleep time to 5ms
    sleep_time_ = 5;
    // Set default to 200: Thus, with default sleep_time_ above, will keep
    // sleeping 5ms at a time until a total of 1s has elapsed, then start doubling.
    num_tries_at_current_sleep_time_ = 200;
    // Set default to try 2000 times, waiting 0.005 seconds 200 times (1s total),
    // then 0.01 seconds 200 times (2s), 0.02 (4s), 0.04 (8s), 0.08 (16s), 0.16 (32s),
    // and finally 0.32 (64s). So max time waiting is:
    //   200 * (0.005 + 0.01 + ... + 0.32) ~= 128s.
    num_retries_ = 1400;
    message_ = "";
    print_default_message_ = GetVerbosity() > 2;
    print_messages_ = true;
    non_blocking_ = false;
  }
};

// Parameters for how to handled failed SendData() attempts (e.g. abort or sleep
// for awhile then try again).
struct SendParams {
  // Whether SendData() should be called again on failure, and if so, how
  // many times. Use 0 to indicate not to try again, use negative value to
  // indicate we should try again infinitely often; otherwise the value
  // represents how many times to retry SendData().
  int num_retries_;

  // The amount of time (in milliseconds) to wait between SendData() attempts.
  uint64_t sleep_time_;

  // Determines whether sleep time between retries should gradually increase.
  // Specifically, this gives the number of times to wait sleep_time_
  // milliseconds, before increasing sleep_time_ by a factor of 2 (and so on).
  // This value is ignored if non-positive (effectively meaning 'sleep_time_'
  // milliseconds will always be the sleep time).
  int num_tries_at_current_sleep_time_;

  // The message that will be sent to the terminal of the program running
  // this Socket. If print_default_message_ is true, the default message
  // will be printed; otherwise, 'message_' will be printed (no message
  // printed if print_default_message_ is false and 'message_' is empty).
  std::string message_;
  // The default message is:
  //   Failed to SendData across IP::PORT. Retrying [in X seconds]...
  bool print_default_message_;

  SendParams() {
    // Set default sleep time to 5ms
    sleep_time_ = 5;
    // Set default to 200: Thus, with default sleep_time_ above, will keep
    // sleeping 5ms at a time until a total of 1s has elapsed, then start doubling.
    num_tries_at_current_sleep_time_ = 200;
    // Set default to try 2000 times, waiting 0.005 seconds 200 times (1s total),
    // then 0.01 seconds 200 times (2s), 0.02 (4s), 0.04 (8s), 0.08 (16s), 0.16 (32s),
    // and finally 0.32 (64s). So max time waiting is:
    //   200 * (0.005 + 0.01 + ... + 0.32) ~= 128s.
    num_retries_ = 1400;
    message_ = "";
    print_default_message_ = GetVerbosity() > 3;
  }
};

struct SocketParams {
  // The following field is a quick way to check if a given GmwConnectionProperties
  // object has the default values or not.
  bool is_default_;

  SocketType type_;

  // No more default (base) variables.
  // Instantiating/derived classes of Socket should extend this base-struct
  // and add whatever variables are desired, and then extend/implement the
  // (pure) virtual function SetSocketParameters() appropriately.

  SocketParams() {
    is_default_ = true;
    type_ = SocketType::OS_TCP;
  }
};

struct TcpSocketParams : public SocketParams {
  SocketRole role_;
  std::string connect_ip_;
  std::string listen_ip_;
  unsigned long port_;

  TcpSocketParams() : SocketParams() {
    role_ = SocketRole::UNKNOWN;
    connect_ip_ = "";
    listen_ip_ = "";
    port_ = 0;
  }
  TcpSocketParams(
      const SocketRole role,
      const std::string & connect_ip,
      const std::string & listen_ip,
      const unsigned long port) :
      SocketParams() {
    role_ = role;
    connect_ip_ = connect_ip;
    listen_ip_ = listen_ip;
    port_ = port;
  }
};

// ============================== HELPER FUNCTIONS =============================
// ===================== GENERIC RECEIVE CALLBACK FUNCTIONS ====================
// The following are general callbacks to use when receiving bytes across a
// connection, where the number of bytes to be received will be the first bytes
// sent in the communication. Then, all bytes have been received when this
// number of bytes (plus however many bytes were needed to express this number)
// have been received. NOTES:
//   - The socket_id and not_used parameters are ignored (they are included in
//     the API so that this is a valid callback for ListenParams.receive_data_).
//   - There is an API for Int32 and for Int64 (use the former unless the number
//     of bytes to be received cannot be expressed in int32_t).
//   - Each function should always return true (unless there was a socket error),
//     independent of whether the number of bytes was actually received.
//     Thus, to know when that number of bytes was received, don't use the
//     return value, but instead look at stats->abort_listening_with_code_,
//     which will be set appropriately when all the bytes have been received.
//   - Casting an integer as a sequence of bytes can be done in different ways:
//       a) System-dependent: Use the OS way of storing an int in memory as bytes
//       b) System-independent: Always do Big (or Little) Endian.
//     Original code did (a), but probably better (system independent) to do (b).
//     Indeed, only advantage of (a) is code simplicity and perhaps minor
//     efficiency gain, but this gain is almost certainly negligible, since
//     the overhead of communication in general will dwarf any efficiency
//     loss of doing (b) instead of (a).
//     Note that in terms of usage, Sender/Receiver cast/parse an int as bytes via:
//       a) Sender:   memcpy(to_send, (char*) &value, sizeof(int64_t));
//          Receiver: memcpy((char*) &value, received_bytes, sizeof(int64_t));
//       b) Sender:   ValueToByteString<int64_t>(value, to_send);
//          Receiver: const int64_t value =
//                    ByteStringToValue<int64_t>(sizeof(int64_t), received_bytes);
// TODO(paul): Deprecate 'ReceiveInt[64,32]Bytes': Update all code to instead call
// the Big-Endian variants, with surrounding code using the ValueToByteString(),
// ByteStringToValue() functions to cast int <-> bytes.
extern bool ReceiveInt64Bytes(
    const SocketIdentifier & socket_id,
    ReceivedData * data,
    SocketStats * stats,
    void * not_used);
extern bool ReceiveInt32Bytes(
    const SocketIdentifier & socket_id,
    ReceivedData * data,
    SocketStats * stats,
    void * not_used);
// Same as above, except the first bytes received are read as Big-Endian
// (above, the bytes are read as the system interprets them, and so is
// Big/Little-Endian, depending on the OS).
extern bool ReceiveBigEndianInt64Bytes(
    const SocketIdentifier & socket_id,
    ReceivedData * data,
    SocketStats * stats,
    void * not_used);
extern bool ReceiveBigEndianInt32Bytes(
    const SocketIdentifier & socket_id,
    ReceivedData * data,
    SocketStats * stats,
    void * not_used);

// The following Callbacks can be used when the number of bytes to be received
// is known ahead of time (and can be pre-loaded in the 'not_used' parameter);
// hence, the first bytes received do NOT specify the number of bytes to expect
// (unlike the API's above). The num_bytes argument should have type uint64_t*.
extern bool ReceiveNumBytes(
    const SocketIdentifier & socket_id,
    ReceivedData * data,
    SocketStats * stats,
    void * num_bytes);
// =================== END GENERIC RECEIVE CALLBACK FUNCTIONS ==================
// ============================ END HELPER FUNCTIONS ===========================

// =============================== SOCKET CLASS ================================
class Socket {
public:
  // ============================== CONSTRUCTOR ================================
  // Default Constructor.
  Socket() {
    role_ = SocketRole::UNKNOWN;
    socket_type_ = SocketType::UNKNOWN;
    is_initialized_ = false;
    is_socket_initialized_ = false;
    server_socket_is_bound_and_accepting_connection_requests_ = false;
  }
  // Since typical/default behavior for a Server is to listen at ip 0.0.0.0,
  // we overload a Constructor with only Port provided to mean the
  // user will be acting as a Server, and to use the default ip of 0.0.0.0
  Socket(const unsigned long & port) : Socket() {
    role_ = SocketRole::SERVER;
    socket_type_ = SocketType::UNKNOWN;
    listen_params_.ip_ = kDefaultListenIp;
    listen_params_.port_ = port;
  }
  // Since typical/default behavior for a Server is to listen at ip 0.0.0.0,
  // we overload a Constructor with only IP and Port provided to mean the
  // user will be acting as a Client, and the ip is the other user's address.
  // If you want to use a Socket() constructor for the *Server* that specifies
  // the ip and port, you must use the API below that explicitly takes in the
  // role (which would be SERVER).
  Socket(const std::string & ip, const unsigned long & port) :
      Socket() {
    role_ = SocketRole::CLIENT;
    socket_type_ = SocketType::OS_TCP;
    connect_params_.ip_ = ip;
    connect_params_.port_ = port;
  }
  // Constructor with Role (Server or Client) specified.
  Socket(const bool is_server) : Socket() {
    role_ = is_server ? SocketRole::SERVER : SocketRole::CLIENT;
    socket_type_ = SocketType::OS_TCP;
  }
  // Constructor with Role specified.
  Socket(const SocketRole role) : Socket() {
    role_ = role;
    socket_type_ = SocketType::OS_TCP;
  }
  // Constructor with Role, IP, and Port provided.
  Socket(
      const bool is_server,
      const std::string & ip,
      const unsigned long & port) :
      Socket() {
    role_ = is_server ? SocketRole::SERVER : SocketRole::CLIENT;
    socket_type_ = SocketType::OS_TCP;
    if (is_server) {
      listen_params_.ip_ = ip;
      listen_params_.port_ = port;
    } else {
      connect_params_.ip_ = ip;
      connect_params_.port_ = port;
    }
  }
  // Constructor with Role, IP, and Port provided.
  Socket(
      const SocketRole role,
      const std::string & ip,
      const unsigned long & port) :
      Socket() {
    role_ = role;
    socket_type_ = SocketType::OS_TCP;
    // Unless a specific role (Client vs. Server) was specified, use provided ip
    // and port for both listening and connecting.
    if (role != SocketRole::CLIENT) {
      listen_params_.ip_ = ip;
      listen_params_.port_ = port;
    }
    if (role != SocketRole::SERVER) {
      connect_params_.ip_ = ip;
      connect_params_.port_ = port;
    }
  }

  // Destructor
  virtual ~Socket() {
  }

  virtual Socket * create() const = 0; // Virtual constructor (creation)
  virtual Socket * clone() const = 0; // Virtual constructor (copying)
  // ============================ END CONSTRUCTOR ==============================

  // ============================ INITIALIZATION ===============================
  // If a derived class (platform) requires initialization, this function
  // should be called to perform it (e.g. for Windows, calls WSAStartup()).
  virtual bool Initialize() = 0;

  // Initializes the external library's socket Socket (e.g. for WindowsSocket,
  // calls 'socket()' on the appropriate SOCKET).
  virtual bool SetupSocket();

  // Resets the socket (closes all existing connections_; clears send/receive
  // buffers and socket stats_; marks client/server initialized status to false);
  // leaves Socket settings (ip, port, [Connect | Listen | Send]Params) in place.
  virtual void Reset(const bool full_reset) {
    ClearSendBuffer();
    ClearReceivedBuffer();
    if (full_reset) {
      ResetSocketStats();
    } else {
      ResetSocketStatsLight();
    }
    CloseSocket();
    ClearConnections();
    is_socket_initialized_ = false;
    server_socket_is_bound_and_accepting_connection_requests_ = false;
    error_msg_ = "";
  }
  // Same as above, with default 'full_reset' = true.
  virtual void Reset() {
    Reset(true);
  }

  // Returns the SocketType of this socket.
  SocketType GetSocketType() const {
    return socket_type_;
  }

  // External library code for setting options as per SocketParams.
  virtual void SocketSetSocketParams(const SocketParams & params) = 0;

  // Get/Set Role (Client vs. Server), IP, and PORT.
  void SetRole(const bool is_server) {
    role_ = is_server ? SocketRole::SERVER : SocketRole::CLIENT;
  }
  void SetRole(const SocketRole role) {
    role_ = role;
  }
  void SetListenIp(const std::string & ip) {
    listen_params_.ip_ = ip.empty() ? kDefaultListenIp : ip;
  }
  void SetListenPort(const unsigned long & port) {
    listen_params_.port_ = port;
  }
  void SetConnectIp(const std::string & ip) {
    connect_params_.ip_ = ip;
  }
  void SetConnectPort(const unsigned long & port) {
    connect_params_.port_ = port;
  }
  void SetListenIpAndPort(
      const std::string & ip, const unsigned long & port) {
    SetListenIp(ip);
    SetListenPort(port);
  }
  void SetConnectIpAndPort(
      const std::string & ip, const unsigned long & port) {
    SetConnectIp(ip);
    SetConnectPort(port);
  }
  void
  GetListenIpAndPort(std::string * ip, unsigned long * port) const {
    *ip = listen_params_.ip_;
    *port = listen_params_.port_;
  }
  void
  GetConnectIpAndPort(std::string * ip, unsigned long * port) const {
    *ip = connect_params_.ip_;
    *port = connect_params_.port_;
  }
  void GetRoleIpAndPort(
      SocketRole * role, std::string * ip, unsigned long * port) const {
    *role = role_;
    if (role_ != SocketRole::CLIENT) {
      GetListenIpAndPort(ip, port);
    } else {
      GetConnectIpAndPort(ip, port);
    }
  }
  // Get the IP address of "localhost" (127.x.x.x, often 127.0.0.1); i.e. the IP
  // address of your computer (on the Internet); can be used by remote computers
  // to access your computer.
  virtual bool GetLocalHost(std::string * ip);
  // Get the IP address within local (work, home) network (computers
  // external to your local (work, home) network can not use this
  // address to connect to your computer. 192.168.x.x
  virtual bool GetWithinNetworkIP(std::string * ip);

  // Set listen timeout (for waiting for a connection or data from other end).
  void SetListenTimeout(const uint64_t & ms);
  // Similar to above, but to apply to all fields related to Server.
  void SetServerTimeout(const uint64_t & ms) {
    SetListenTimeout(ms);
  }
  // Ditto, for Client.
  void SetClientTimeout(const uint64_t & ms) {
    // Update ConnectParams with this timeout.
    connect_params_.timeout_ms_ = ms;
    SetListenTimeout(ms);
  }

  // Get/Set listen_params_.
  ListenParams GetListenParams() {
    return listen_params_;
  }
  virtual void SetListenParams(const ListenParams & params);
  // Similar to above, but just sets the receive_data_ field (leaving the rest
  // of the listen_params_ as-is).
  void
  SetListenReceiveDataCallback(fn_w_args_ptr fn_callback, void * args);
  // Same as above, with nullptr for the extra (void*) parameter.
  void SetListenReceiveDataCallback(fn_w_args_ptr fn_callback);

  // Get/Set connect_params_.
  void SetConnectParams(const ConnectParams & params) {
    connect_params_ = params;
  }
  void SetConnectNonBlocking(const bool is_non_blocking) {
    connect_params_.non_blocking_ = is_non_blocking;
  }
  ConnectParams GetConnectParams() {
    return connect_params_;
  }

  // Get/Set send_params_.
  void SetSendParams(const SendParams & params) {
    send_params_ = params;
  }
  SendParams GetSendParams() {
    return send_params_;
  }
  // ============================ END INITIALIZATION ===========================

  // =============================== STATISTICS ================================
  // Returns whether a connection exists between this Socket and the input one.
  bool IsConnected(const SocketIdentifier & socket) const;
  // The following API should be used only if there is only one possible
  // connection that this socket could have, which will always be the case
  // for CLIENT sockets, and may or may not be the case for SERVER sockets.
  // Caller is responsible for ensuring that this API is only called when appropriate.
  bool IsConnected() const {
    return connections_.size() == 1;
  }

  // Return error message.
  std::string GetErrorMessage() const {
    return error_msg_;
  }

  // Get/Set Socket Statistics.
  SocketStats GetSocketStats() const {
    return stats_;
  }
  std::string PrintSocketStats() const {
    return stats_.Print();
  }
  std::string PrintSocketStats(
      const bool print_errors,
      const bool print_send_receive,
      const bool print_global_send_receive,
      const bool print_num_calls,
      const bool print_timers) const {
    return stats_.Print(
        print_errors,
        print_send_receive,
        print_global_send_receive,
        print_num_calls,
        print_timers);
  }
  void ResetSocketStats() {
    stats_.ResetAll();
  }
  // Similar to above, but maintains some of the counter/field values.
  void ResetSocketStatsLight() {
    stats_.ResetSendReceiveCounts();
    stats_.ResetErrorsLight();
  }
  // Just clears errors.
  void ResetSocketErrorsAndAbortSignals() {
    stats_.ResetErrors();
    stats_.abort_listening_with_code_.clear();
  }
  void ActivateTimers(const bool activate) {
    stats_.activate_timers_ = activate;
  }
  // ============================= END STATISTICS ==============================

  // =============================== CLEAN-UP ==================================
  void ResetForReceive();
  // Same as above, but also initialize received_bytes_per_socket_
  // with the provided bytes.
  bool ResetForReceive(std::vector<char> & extra_bytes);
  // ============================= END CLEAN-UP ================================

  // ================================ CONNECT ==================================
  // For "Client-side" instantiations, this function attempts to make a
  // connection to (a Listening server at) the indicated ip_ and port_.
  // Note: "Server" calls to connect are also possible, but only 'under-the-hood';
  // see ServerConnect() below.
  // UPDATE: Connect() is now handled under-the-hood, so users never need to
  // use this API. It should be moved under 'private', but is kept public for
  // backwards-compatibility.
  bool Connect();

  // For "Client-side" instantiations, this function closes its connection
  // to the Server, and clears all socket stats, etc.
  bool Disconnect();
  // ============================== END CONNECT ================================

  // ================================= SEND ====================================
  // Sends 'data' across the indicated socket. API toggles:
  //   - vector vs. array
  //   - char vs. unsigned char
  SendReturnCode SendData(
      const SocketIdentifier & socket_info,
      const char * data,
      const size_t & size);
  SendReturnCode SendData(
      const SocketIdentifier & socket_info,
      const unsigned char * data,
      const size_t & size) {
    return SendData(socket_info, (const char *)data, size);
  }
  SendReturnCode SendData(
      const SocketIdentifier & socket_info,
      const std::vector<char> & data) {
    return SendData(socket_info, data.data(), data.size());
  }
  SendReturnCode SendData(
      const SocketIdentifier & socket_info,
      const std::vector<unsigned char> & data) {
    return SendData(socket_info, data.data(), data.size());
  }

  // Send data across the i^th connection in connections_; same API toggles.
  SendReturnCode SendData(
      const int connection_index,
      const char * data,
      const size_t & size) {
    if (connection_index < 0 ||
        connections_.size() <= connection_index) {
      return SendReturnCode::UNKNOWN_SOCKET;
    }
    return SendData(connections_[connection_index], data, size);
  }
  SendReturnCode
  SendData(const int connection_index, const std::vector<char> & data) {
    return SendData(connection_index, data.data(), data.size());
  }
  SendReturnCode SendData(
      const int connection_index,
      const unsigned char * data,
      const size_t & size) {
    if (connection_index < 0 ||
        connections_.size() <= connection_index) {
      return SendReturnCode::UNKNOWN_SOCKET;
    }
    return SendData(connections_[connection_index], data, size);
  }
  SendReturnCode SendData(
      const int connection_index,
      const std::vector<unsigned char> & data) {
    return SendData(connection_index, data.data(), data.size());
  }

  // Send data across default connection:
  //   - If role_ = CLIENT: Use socket returned by SocketGetClientSocketId().
  //   - If role_ = SERVER: Use the first connection in connections_;
  //                        if no connections exist, but ip and port are set,
  //                        attempt to connect, and then send data.
  SendReturnCode SendData(const char * data, const size_t & size);
  SendReturnCode
  SendData(const unsigned char * data, const size_t & size) {
    return SendData((const char *)data, size);
  }
  SendReturnCode SendData(const std::vector<char> & data) {
    return SendData(data.data(), data.size());
  }
  SendReturnCode SendData(const std::vector<unsigned char> & data) {
    return SendData(data.data(), data.size());
  }
  // ============================= SEND, NO FLUSH ==============================
  // Mirror-copy of all the SendData() APIs above, but enqueus data instead of
  // sending it right away.
  bool SendDataNoFlush(
      const SocketIdentifier & socket_info,
      const std::vector<char> & data);
  bool SendDataNoFlush(
      const SocketIdentifier & socket_info,
      const std::vector<unsigned char> & data);
  bool SendDataNoFlush(
      const SocketIdentifier & socket_info,
      const char * data,
      const size_t & size);
  bool SendDataNoFlush(
      const SocketIdentifier & socket_info,
      const unsigned char * data,
      const size_t & size);
  bool SendDataNoFlush(
      const int connection_index,
      const char * data,
      const size_t & size) {
    if (connection_index < 0 ||
        connections_.size() <= connection_index) {
      return false;
    }
    return SendDataNoFlush(connections_[connection_index], data, size);
  }
  bool SendDataNoFlush(
      const int connection_index, const std::vector<char> & data) {
    return SendDataNoFlush(connection_index, data.data(), data.size());
  }
  bool SendDataNoFlush(
      const int connection_index,
      const unsigned char * data,
      const size_t & size) {
    if (connection_index < 0 ||
        connections_.size() <= connection_index) {
      return false;
    }
    return SendDataNoFlush(connections_[connection_index], data, size);
  }
  bool SendDataNoFlush(
      const int connection_index,
      const std::vector<unsigned char> & data) {
    return SendDataNoFlush(connection_index, data.data(), data.size());
  }
  bool SendDataNoFlush(const std::vector<char> & data);
  bool SendDataNoFlush(const std::vector<unsigned char> & data);
  bool SendDataNoFlush(const char * data, const size_t & size);
  bool SendDataNoFlush(const unsigned char * data, const size_t & size);
  // ========================== FLUSH (SEND ENQUEUED) ==========================
  // Sends the bytes in the provided buffer along the corresponding socket.
  // Note that this API does *not* clear the input buffer (caller should do so).
  SendReturnCode FlushSendBuffer(
      const SocketIdentifier & socket_info,
      const std::vector<char> & to_flush);
  // Sends the bytes in the DataToSend.buffer_ along the corresponding socket,
  // clearing the buffer when done.
  SendReturnCode FlushSendBuffer(
      const SocketIdentifier & socket_info, DataToSend * to_flush);
  // Same as above, but looks up the appropriate DataToSend buffer
  // (within bytes_to_send_per_socket_).
  SendReturnCode FlushSendBuffer(const SocketIdentifier & socket_info);
  // Same as above, where we lookup the DataToSend buffer (and socket to send
  // across) within connections_.
  SendReturnCode FlushSendBuffer(const int connection_index);

  // Send (flush) data across default connection:
  //   - If role_ = CLIENT: Use socket returned by SocketGetClientSocketId().
  //   - If role_ = SERVER: Use the first connection in connections_.
  //                        If no connections exist, do nothing (return true).
  SendReturnCode FlushSendBuffer();

  // Flush send buffers across all connections.
  SendReturnCode FlushSendBuffers();
  // =============================== END SEND ==================================

  // ================================ LISTEN ===================================
  // Default Listen().
  // The 'clear_receive_buffer' flag signals to check to see if any of the
  // existing connections_ have non-empty ReceivedData.buffer_, and if so,
  // to process those bytes before listen()ing for more events.
  std::set<ListenReturnCode> Listen(const bool clear_receive_buffer);
  // Same as above, default 'do_preliminary_read' to false.
  std::set<ListenReturnCode> Listen() {
    return Listen(false);
  }
  // Listen(), with specification of which Events the socket should handle,
  // and which to pass control back to caller.
  std::set<ListenReturnCode> Listen(const ListenType & type) {
    listen_params_.type_.insert(type);
    return Listen(false);
  }
  // Same as above, but allows greater control.
  std::set<ListenReturnCode>
  Listen(const std::set<ListenType> & types) {
    listen_params_.type_ = types;
    return Listen(false);
  }
  // Listen(), with specification of which Events the socket should handle
  // and how it should handle each type of Event.
  std::set<ListenReturnCode> Listen(const ListenParams & params) {
    listen_params_ = params;
    return Listen(false);
  }
  // ============================== END LISTEN =================================

  // =============================== RECEIVE ===================================
  const std::map<SocketIdentifier, ReceivedData>
  GetReceivedBytes() const {
    return received_bytes_per_socket_;
  }

  // The following two methods just return a (const) pointer to the relevant
  // 'Received Bytes' vector. Be sure if you use these that underlying
  // Received Bytes vector will not change throughout caller's usage of it.
  const std::vector<char> *
  GetReceivedBytesVector(const SocketIdentifier & s);
  const std::vector<char> *
  GetReceivedBytesVector(const uint64_t & socket_index);

  // Copies the received data buffer for connection 's' into the provided vector.
  // In reality, swap() is used, for efficiency (so after the call, the received
  // data buffer for connection 's' will contain the original contents of
  // 'output', which is likely empty).
  bool
  SwapBuffer(const SocketIdentifier & s, std::vector<char> * output);
  // Same as above, but API gives a location within received_bytes_per_socket_
  // instead of a SocketIdentifier.
  bool SwapBuffer(const uint64_t & index, std::vector<char> * output);
  /*
  // DEPRECATED: The following functions are not supported; see comments in the .cpp.
  // Same as above, but only swaps the tail end of buffer_ (starting at
  // start_index).
  bool SwapBuffer(const SocketIdentifier& s, const uint64_t& start_index,
                  std::vector<char>* output);
  // Same as above, but API gives a location within received_bytes_per_socket_
  // instead of a SocketIdentifier.
  bool SwapBuffer(const uint64_t& socket_index, const uint64_t& start_index,
                  std::vector<char>* output);
  */

  // Copies the receive buffer of socket 's' (starting at indicated start index)
  // into the provided container (which should be empty on input).
  bool CopyBuffer(
      const SocketIdentifier & s,
      const uint64_t & start_index,
      std::vector<char> * output);
  // Same as above, but API gives a location within received_bytes_per_socket_
  // instead of a SocketIdentifier.
  bool CopyBuffer(
      const uint64_t & socket_index,
      const uint64_t & start_index,
      std::vector<char> * output);
  // Same as above, for direct memory API ('output' should have already
  // allocated proper space/size).
  // There are 3 options for 'num_copied_bytes':
  //   - Pass in nullptr, if you just want to copy everything and DON'T
  //     need to know how many bytes were copied.
  //   - Pass in ptr to '0' if you want to copy everything, and DO
  //     need to know how many bytes were copied.
  //   - Pass in ptr to number of bytes you want to copy, if you don't
  //     want to copy everything (then this will return false if this
  //     many bytes are not available).
  bool CopyBuffer(
      const SocketIdentifier & s,
      const uint64_t & start_index,
      uint64_t * num_copied_bytes,
      char * output);
  // Same as above, with no container to store number of bytes copied.
  bool CopyBuffer(
      const SocketIdentifier & s,
      const uint64_t & start_index,
      char * output) {
    return CopyBuffer(s, start_index, nullptr, output);
  }
  // Same as above, but API gives a location within received_bytes_per_socket_
  // instead of a SocketIdentifier.
  bool CopyBuffer(
      const uint64_t & socket_index,
      const uint64_t & start_index,
      uint64_t * num_copied_bytes,
      char * output);
  // Same as above, with no container to store number of bytes copied.
  bool CopyBuffer(
      const uint64_t & socket_index,
      const uint64_t & start_index,
      char * output) {
    return CopyBuffer(socket_index, start_index, nullptr, output);
  }

  // ============================= END RECEIVE =================================

protected:
  // ============================= MEMBER VARIABLES ============================
  SocketRole role_;
  SocketType socket_type_;

  bool is_initialized_;
  bool is_socket_initialized_;
  // For sockets utilizing non-blocking connect(), we re-use the same port
  // for every connect() call (so that Server doesn't think there are lots
  // of connection requests); in order to use the same (Client) port for
  // each connect call, we need to bind the client_socket_ to a specific
  // port. This field indicates if a port has been selected (and bound) yet.
  bool client_socket_is_bound_;

  // Keeps track of whether Server Socket has been created (via call to socket())
  // and has had bind() and listen() already called on it.
  bool server_socket_is_bound_and_accepting_connection_requests_;
  std::string error_msg_;

  // Statistics for this socket.
  SocketStats stats_;

  // Parameters for how this socket should "Listen" to events across a socket.
  ListenParams listen_params_;

  // Parameters for how this socket should "Connect()".
  ConnectParams connect_params_;

  // Parameters for how this socket should "SendData()".
  SendParams send_params_;

  // Stores details of each (socket) connection this socket has established.
  std::vector<SocketIdentifier> connections_;

  // The following data structure holds the data received across each socket.
  std::map<SocketIdentifier, ReceivedData> received_bytes_per_socket_;
  // The following data structure holds the data to be sent across each socket.
  std::map<SocketIdentifier, DataToSend> bytes_to_send_per_socket_;

  // ================== MEMBER FUNCTIONS ACCESSIBLE TO CHILDREN ================
  // Sets error_msg_ to the provided string.
  void SetErrorMessage(
      const bool print,
      const size_t & line,
      const std::string & message);
  // Same as above, with default 'print' set to true.
  void
  SetErrorMessage(const size_t & line, const std::string & message) {
    SetErrorMessage(true, line, message);
  }

  // Function to call upon delete; cleans up connections_, and calls either
  // CloseServerSocket() or CloseClientSocket(), as appropriate.
  bool CloseSocket();

private:
  // "Connection" request by the Server. This is invoked when the protocol
  // dictates that the Server should send something to the Client *first* (or
  // perhaps if there was disconnection, and then the Server wants to send
  // something to the Client, and needs to (re-)establish the connection first.
  // This doesn't actually have the Server call Connect() (and thus the Server's
  // connect_params_ aren't used); but rather has the Server call Listen()
  // with instructions to abort as soon as the Client's Connect() request
  // arrives, at which point code returns to caller, whereby the Server
  // can complete his SendData() effort.
  bool ServerConnect();

  // Closes the connection pointed to by itr->socket_, and removes *itr from
  // connections_. Returns false if there was an error trying to close
  // itr->socket_.
  bool RemoveConnection(std::vector<SocketIdentifier>::iterator & itr);

  // Closes the indicated socket connection.
  bool ShutdownConnection(const SocketIdentifier & s);
  // Closes all connections in connections_, and clears that vector.
  // Also clears received_bytes_per_socket_.
  bool ShutdownAllConnections();

  void ClearConnections() {
    connections_.clear();
  }
  void ClearSendBuffer() {
    bytes_to_send_per_socket_.clear();
  }
  void ClearReceivedBuffer() {
    received_bytes_per_socket_.clear();
  }

  std::set<ListenReturnCode> CallThreadListen(const bool as_server);

  // The actual function the listening thread will perform: An infinite loop
  // that continually checks for new connection requests.
  std::set<ListenReturnCode> ThreadListen();

  // Uses a PThread to call ThreadListen().
  static void * CallPThreadListen(void * windows_socket_class_instance);

  // Returns true if the StopListeningCondition specified in
  // [sever | client]listen_params_.stop_params_ have not been met.
  bool ShouldKeepListening(std::set<ListenReturnCode> * return_codes);
  // Same as above, but passes in the StopListeningCondition and SocketStats.
  static bool ShouldKeepListening(
      const SocketStats & stats,
      const StopListeningCondition & stop_params,
      std::set<ListenReturnCode> * return_codes);

  // Removes all connections_, and calls closesocket() on Server Socket.
  bool CloseServerSocket();
  // Removes all connections_, and calls closesocket() on Client Socket.
  bool CloseClientSocket();

  // Computes how much time to sleep before retrying a failed connect().
  int64_t ComputeConnectSleepTime(const bool print_message);

  // Checks whether Client's call to connect() has completed, by testing
  // Client Socket for readiness to write (via a call to select()).
  bool CheckConnectStatus(bool * is_connected, int * socket_error_code);

  // Handles request for a new connection. Returns true if successful,
  // otherwise, returns false and sets stats_.last_listen_error_
  // appropriately, as well as setting error_msg_.
  bool HandleNewConnection();

  // If (this) socket is responsible for handling new connections, and no
  // caller-provided callback function is available (via listen_params_.
  // handle_new_connection_), then this is the function that gets called
  // when in the Listen() state and a New Connection Event occurs.
  // This function opens a new SOCKET for the new connection, and adds it
  // to connections_.
  bool OpenNewConnection();

  // Checks each socket in connections_ to see if an Event occurred (based
  // on that connection's presence in '[read | write | error]_sockets').
  // Returns true if all events were successfully handled, or false if an
  // error was encountered, in which case the appropriate SocketStats error
  // counters, and stats_.last_listen_error_, have been updated (on next
  // iteration through while loop in ThreadListen(),
  // CheckStopListeningCondition() will then determine if control should be
  // returned to caller).
  bool HandleSelectEvents(
      std::set<SocketIdentifier> * read_sockets,
      std::set<SocketIdentifier> * error_sockets);

  // Reads data coming in across connection; more precisely, looks up
  // 'connection' within 'received_bytes_per_socket_', adding a new
  // ReceivedData object to it if not already present; then writes to that
  // ReceivedData's buffer_.
  bool ReceiveData(const SocketIdentifier & s);

  // A sub-routine of ReceiveData; takes appropriate action on bytes
  // received across the connection, which may include:
  //   - returning control to user (after a call to [Server | Client]Listen):
  //     if ListenParams.type_ does not include HANDLE_RECEIVE nor HANDLE_ALL_EVENTS
  //   - call the user-provided fn callback of ListenParams.receive_data_:
  //     if that field is non-null
  //   - do nothing: Otherwise
  // Returns true in all cases, except in the middle case, if the user-provided
  // function returned false.
  bool HandleReceivedBytes(
      const ListenParams & listen_params,
      const SocketIdentifier & s,
      ReceivedData * received_data);

  // Send Data. Called by both Server and Client, as a sub-routine of one
  // of the public SendData() methods.
  SendReturnCode SendDataNow(
      const SocketIdentifier & s,
      const char * data,
      const size_t & size);
  SendReturnCode SendBlob(
      const SocketIdentifier & s,
      const char * buffer,
      const size_t & num_chars_in_buffer,
      int * num_chars_sent);
  // Same as above, but actually calls send() (the above API's will call this one
  // as a sub-routine, after making sure SOCKET 's' is ready by calling select() on it).
  SendReturnCode SendBlobNow(
      const SocketIdentifier & s,
      const char * buffer,
      const size_t & num_chars_in_buffer,
      int * num_chars_sent);

  // A socket error occurred. Determine the error, and handle it appropriately.
  // Returns false if the error is related to the server itself (i.e. the error
  // will affect all sockets/connections, not just the one passed in), indicating
  // the program to abort (i.e. Listen() will fail). Otherwise, when appropriate,
  // will remove the socket passed in from connections_, and set itr to point to
  // the next socket in connections_. The 'event_type' is used for error logging;
  // on input it contains the context of the error (Send, Receive, etc.), and on
  // output, it also contains (when this function returns false) the error code.
  bool HandleGeneralSocketError(
      std::vector<SocketIdentifier>::iterator & itr);

  // Handles a failed bind() attempt, based on listen_params_.bind_params_.
  std::set<ListenReturnCode> HandleBindError();

  // Handles a failed Connect() attempt, based on connect_params_.
  bool HandleConnectError(const int last_error_code);
  // Same as above, but for client sockets that had specified a non-blocking
  // connect() (via ConnectParams.blocking_connect_ == false).
  bool HandleNonBlockingConnect(const int last_error_code);

  // An Error occurred while trying to receive data across socket s. Depending
  // on the error, the socket may have to be destroyed. Returns false if the
  // socket should be closed and removed from connections_.
  bool HandleReceiveError(const SocketIdentifier & s);
  // Similar to above, for send errors.
  bool HandleSendError(const SocketIdentifier & s);

  // ========================= EXTERNAL LIBRARY FUNCTIONS ======================

  // ====================== EXTERNAL LIBRARY: INIT FUNCTIONS ====================
  // External library code for client/server socket setup.
  virtual bool SocketSetupClientSocket() = 0;
  virtual bool SocketSetupServerSocket() = 0;

  // Returns the External library identifier of the Client/Server Socket.
  virtual SocketIdentifier SocketGetClientSocketId() = 0;
  virtual SocketIdentifier SocketGetServerSocketId() = 0;
  // Returns the External library identifier of the Send/Receive Socket.
  virtual SocketIdentifier SocketGetReceiveSocketId() = 0;
  virtual SocketIdentifier SocketGetSendSocketId() = 0;

  // DEPRECATED. Not used, so removed. Kept here for posterity, in case it is useful...
  // External library code for getting the IP and Port of an existing (connected) Socket.
  /*
  virtual bool SocketGetBoundSocketIpAndPort(
      const SocketIdentifier& s, std::string* ip, unsigned int* port) = 0;
  */

  // Returns whether the indicated socket id corresponds to the client/server socket.
  virtual bool SocketIsClientSocket(const SocketIdentifier & s) = 0;
  virtual bool SocketIsServerSocket(const SocketIdentifier & s) = 0;

  virtual bool SocketGetLocalHost(std::string * ip) = 0;
  virtual bool SocketGetWithinNetworkIP(std::string * ip) = 0;

  // External library code for setting an option (code) to a given socket.
  virtual bool SocketSetSocketOption(
      const SocketIdentifier & s,
      const int socket_type,
      const int option_code) = 0;
  virtual bool SocketSetClientSocketOption(
      const int socket_type, const int option_code) = 0;
  virtual bool SocketSetServerSocketOption(
      const int socket_type, const int option_code) = 0;
  // External library code for setting an option (code) to a given TLS socket.
  virtual bool SocketSetTlsSocketOption(
      const SocketIdentifier & s, const int option_code) = 0;
  // External library code for setting a (TLS) socket to be non-blocking.
  virtual bool SocketSetNonBlockingSocket(
      const bool non_blocking, const SocketIdentifier & s) = 0;

  // ====================== EXTERNAL LIBRARY: CORE FUNCTIONS ===================
  // External-Library implementation of send(). Returns the number of chars sent.
  virtual int SocketSend(
      const SocketIdentifier & s,
      const char * buffer,
      const size_t & num_chars_in_buffer) = 0;

  // External-Library implementation of bind().
  virtual bool SocketBind() = 0;

  // External-Library implementation of select().
  virtual int SocketSelect(
      const SocketIdentifier & s,
      const uint64_t & timeout_secs,
      const uint64_t & timeout_micro_secs,
      std::set<SocketIdentifier> * read_sockets,
      std::set<SocketIdentifier> * write_sockets,
      std::set<SocketIdentifier> * error_sockets) = 0;

  // External-Library implementation of recv() function, storing the bytes in
  // the provided buffer.
  // NOTE: Caller is responsible for making sure receive_buffer has already
  // been allocated space for 'max_rec_bytes'.
  virtual int SocketReceive(
      const SocketIdentifier & s,
      const int max_rec_bytes,
      char * rec_buffer) = 0;

  // External-Library implementation of listen() (on the Server socket).
  virtual bool SocketServerListen() = 0;

  // External-Library implementation of connect() (on the Client Socket).
  virtual bool SocketClientConnect() = 0;

  // External-Library implementation for opening a new connection (i.e. for 'accept()').
  virtual bool
  SocketOpenNewConnection(SocketIdentifier * new_connection) = 0;

  // External-Library implementation for closing a specified connection.
  virtual bool SocketShutdownConnection(const SocketIdentifier & s) = 0;

  // External-Library implementation to shut down the client/server socket.
  virtual bool
  SocketCloseClientSocket(const bool connection_already_removed) = 0;
  virtual bool SocketCloseServerSocket() = 0;

  // ================== EXTERNAL LIBRARY: ERROR (CODE) FUNCTIONS ===============
  // OS-Specific code for getting the last encountered error code.
  virtual int SocketGetLastError() = 0;

  // OS-Specific code for getting the last encountered error on a socket.
  virtual int SocketGetSocketError(const SocketIdentifier & s) = 0;
  virtual int SocketGetClientSocketError() = 0;
  virtual int SocketGetServerSocketError() = 0;

  // Returns the external library code for SOCKET_ERROR on a socket.
  virtual int SocketSocketErrorCode() = 0;

  // Returns the external library code for whether a given socket function call
  // would block.
  virtual int SocketSocketWouldBlockCode() = 0;

  // Returns the external library code for if a given socket function call is in progress.
  virtual int SocketSocketInProgressCode() = 0;

  // Returns the external library code for a socket being reset by the Client.
  virtual int SocketSocketConnectResetCode() = 0;

  // Returns the external library code for a socket being aborted by the Client.
  virtual int SocketSocketConnectionAbortedCode() = 0;

  // Returns the external library code for a socket refusing a connection attempt.
  virtual int SocketSocketConnectionRefusedCode() = 0;

  // Returns the external library code indicating an invalid socket.
  virtual int SocketSocketUnavailableCode() = 0;

  // Returns the external library code indicating a socket is in-use.
  virtual int SocketSocketIntUseCode() = 0;

  // Returns the external library code for the socket layer.
  virtual int SocketGetSocketLayerCode() = 0;

  // Returns the external library code for IPPROTO_TCP.
  virtual int SocketGetSocketTcpCode() = 0;

  // Returns the external library code for the TCP_NODELAY.
  virtual int SocketGetSocketTcpNoDelayCode() = 0;

  // Returns the external library code for the option of allowing a socket to bind
  // to a port in use by another socket.
  virtual int SocketGetSocketReusePortCode() = 0;

  // Returns the external library code for the operations already in progress.
  virtual int SocketSocketOpAlreadyCode() = 0;

  // ================== EXTERNAL LIBRARY: MISCELLANEOUS FUNCTIONS ===============
  // External library implementation for (thread) Sleep().
  virtual void SocketSleep(const long long int & sleep_micro_sec) = 0;

  // Returns whether the Client/Server Socket is in the input set.
  virtual bool SocketIsClientSocketInSet(
      const std::set<SocketIdentifier> & input) = 0;
  virtual bool SocketIsServerSocketInSet(
      const std::set<SocketIdentifier> & input) = 0;

  // ====================== END EXTERNAL LIBRARY FUNCTIONS =====================
};

} // namespace networking

#endif
