/**
 * Copyright (C) 2020 Stealth Software Technologies, Inc.
 *
 * Developed under IR&D
 *
*/
#include "socket.h"

#include "Util/char_casting_utils.h"
#include "Util/map_utils.h"
#include "Util/string_utils.h"

#include <ff/logging.h>

#include <string>

using namespace map_utils;
using namespace string_utils;
using namespace test_utils;
using namespace std;

namespace networking {

namespace {

// A dummy method used to create return a set<ListenReturnCode> that
// has one element. This function can be used for code efficiency:
// instead of writing three lines:
//   set<ListenReturnCode> to_return;
//   to_return.insert(foo);
//   return to_return;
// we can write the one line:
//   return CreateSet(foo);
// Note that even though the returned object is on the stack, the returned
// values are valid, since it will be copied by calling method.
set<ListenReturnCode> CreateSet(const ListenReturnCode & code) {
  set<ListenReturnCode> to_return;
  to_return.insert(code);
  return to_return;
}

// Returns a string representation of the provided socket (id).
string PrintSocketId(const SocketIdentifier & s) {
  string to_return = "";
  if (s.socket_id_ != -1) {
    to_return += "Socket Id: " + Itoa(s.socket_id_);
  }
  if (s.socket_index_ >= 0) {
    if (!to_return.empty())
      to_return += ", ";
    to_return += "Socket Index: " + Itoa(s.socket_index_);
  }
  if (!s.ip_.empty()) {
    if (!to_return.empty())
      to_return += ", ";
    to_return += "IP: " + s.ip_ + ", Port: " + Itoa(s.port_);
  }
  return to_return;
}

} // namespace

static const StopListeningCondition kDefaultStopListeningCondition =
    StopListeningCondition();

// ========================= GENERIC UTIL FUNCTIONS ============================

string GetListenReturnCodeString(const ListenReturnCode code) {
  if (code == ListenReturnCode::UNKNOWN)
    return "UNKNOWN";
  if (code == ListenReturnCode::OK)
    return "OK";
  if (code == ListenReturnCode::BAD_INITIALIZATION_ERROR) {
    return "BAD_INITIALIZATION_ERROR";
  }
  if (code == ListenReturnCode::BIND_ERROR)
    return "BIND_ERROR";
  if (code == ListenReturnCode::LISTEN_ERROR)
    return "LISTEN_ERROR";
  if (code == ListenReturnCode::CONNECT_ERROR)
    return "CONNECT_ERROR";
  if (code == ListenReturnCode::DISCONNECT_ERROR)
    return "DISCONNECT_ERROR";
  if (code == ListenReturnCode::SELECT_ERROR)
    return "SELECT_ERROR";
  if (code == ListenReturnCode::SERVER_SOCKET_ERROR_EVENT) {
    return "SERVER_SOCKET_ERROR_EVENT";
  }
  if (code == ListenReturnCode::CLIENT_SOCKET_ERROR_EVENT) {
    return "CLIENT_SOCKET_ERROR_EVENT";
  }
  if (code == ListenReturnCode::ERROR_EVENT)
    return "ERROR_EVENT";
  if (code == ListenReturnCode::RECEIVE_ERROR)
    return "RECEIVE_ERROR";
  if (code == ListenReturnCode::RECEIVED_UNEXPECTED_BYTES) {
    return "RECEIVED_UNEXPECTED_BYTES";
  }
  if (code == ListenReturnCode::SEND_ERROR)
    return "SEND_ERROR";
  if (code == ListenReturnCode::TOO_MANY_ERRORS)
    return "TOO_MANY_ERRORS";
  if (code == ListenReturnCode::TOO_MANY_CONNECTIONS) {
    return "TOO_MANY_CONNECTIONS";
  }
  if (code == ListenReturnCode::CUMULATIVE_CONNECTIONS_THRESHOLD) {
    return "CUMULATIVE_CONNECTIONS_THRESHOLD";
  }
  if (code == ListenReturnCode::CUMULATIVE_BYTES_SENT) {
    return "CUMULATIVE_BYTES_SENT";
  }
  if (code == ListenReturnCode::CUMULATIVE_BYTES_RECEIVED) {
    return "CUMULATIVE_BYTES_RECEIVED";
  }
  if (code == ListenReturnCode::CUMULATIVE_SEND_TRANSACTIONS) {
    return "CUMULATIVE_SEND_TRANSACTIONS";
  }
  if (code == ListenReturnCode::CUMULATIVE_RECEIVE_TRANSACTIONS) {
    return "CUMULATIVE_RECEIVE_TRANSACTIONS";
  }
  if (code == ListenReturnCode::CUMULATIVE_RECEIVE_NULL_CHARS) {
    return "CUMULATIVE_RECEIVE_NULL_CHARS";
  }
  if (code == ListenReturnCode::NO_ACTIVITY)
    return "NO_ACTIVITY";
  if (code == ListenReturnCode::NEW_CONNECTION)
    return "NEW_CONNECTION";
  if (code == ListenReturnCode::RECEIVE)
    return "RECEIVE";
  return "Unrecognized ListenReturnCode: " +
      Itoa(static_cast<int>(code));
}
set<string>
GetListenReturnCodeList(const set<ListenReturnCode> & codes) {
  set<string> to_return;
  for (const ListenReturnCode & code : codes) {
    to_return.insert(GetListenReturnCodeString(code));
  }
  return to_return;
}

bool ReceiveInt64Bytes(
    const SocketIdentifier & socket_id,
    ReceivedData * data,
    SocketStats * stats,
    void * not_used) {
  if (data == nullptr || stats == nullptr) {
    log_fatal("Null input to ReceiveInt64Bytes.");
  }
  if (data->is_receiving_data_) {
    log_fatal("Synchronization problem in ReceiveInt64Bytes. "
              "This should never happen.");
  }

  const vector<char> & buffer = data->buffer_;

  // The first things sent are the number of bytes to expect (not counting
  // these first bytes).
  // Return (will keep Listening) if these haven't been received yet.
  if (buffer.size() < sizeof(uint64_t))
    return true;

  uint64_t num_bytes;
  memcpy((char *)&num_bytes, buffer.data(), sizeof(uint64_t));

  // Return (will keep listening) if not all the expected bytes have been received:
  //   sizeof(uint64_t) +  // To express 'num_bytes'
  //   num_bytes           // All of the bytes
  if (buffer.size() < sizeof(uint64_t) + num_bytes) {
    // Now that we know expected size of buffer, reserve this size, to
    // minimize the number of resizes that occur.
    data->buffer_.reserve(sizeof(uint64_t) + num_bytes);
    return true;
  }

  // We've received enough bytes. Alert Socket to stop listening on this connection
  // (set the return code to be OK if the exact number of expected bytes were read;
  // otherwise alert that there was an ERROR.).
  if (buffer.size() > sizeof(uint64_t) + num_bytes) {
    stats->abort_listening_with_code_.insert(
        ListenReturnCode::RECEIVED_UNEXPECTED_BYTES);
  } else {
    stats->abort_listening_with_code_.insert(ListenReturnCode::OK);
  }

  return true;
}

bool ReceiveInt32Bytes(
    const SocketIdentifier & socket_id,
    ReceivedData * data,
    SocketStats * stats,
    void * not_used) {
  if (data == nullptr || stats == nullptr) {
    log_fatal("Null input to ReceiveInt32Bytes.");
  }
  if (data->is_receiving_data_) {
    log_fatal("Synchronization problem in ReceiveInt32Bytes. "
              "This should never happen.");
  }

  const vector<char> & buffer = data->buffer_;

  // The first things sent are the number of bytes to expect (not counting
  // these first bytes).
  // Return (will keep Listening) if these haven't been received yet.
  if (buffer.size() < sizeof(uint32_t))
    return true;

  uint32_t num_bytes;
  memcpy((char *)&num_bytes, buffer.data(), sizeof(uint32_t));

  // Return (will keep listening) if not all the expected bytes have been received:
  //   sizeof(uint32_t) +  // To express 'num_bytes'
  //   num_bytes           // All of the bytes
  if (buffer.size() < sizeof(uint32_t) + num_bytes) {
    // Now that we know expected size of buffer, reserve this size, to
    // minimize the number of resizes that occur.
    data->buffer_.reserve(sizeof(uint32_t) + num_bytes);
    return true;
  }

  // We've received enough bytes. Alert Socket to stop listening on this connection
  // (set the return code to be OK if the exact number of expected bytes were read;
  // otherwise alert that there was an ERROR.).
  if (buffer.size() > sizeof(uint32_t) + num_bytes) {
    stats->abort_listening_with_code_.insert(
        ListenReturnCode::RECEIVED_UNEXPECTED_BYTES);
  } else {
    stats->abort_listening_with_code_.insert(ListenReturnCode::OK);
  }
  return true;
}

bool ReceiveBigEndianInt64Bytes(
    const SocketIdentifier & socket_id,
    ReceivedData * data,
    SocketStats * stats,
    void * not_used) {
  if (data == nullptr || stats == nullptr) {
    log_fatal("Null input to ReceiveBigEndianInt64Bytes.");
  }
  if (data->is_receiving_data_) {
    log_fatal("Synchronization problem in ReceiveBigEndianInt64Bytes. "
              "This should never happen.");
  }

  const vector<char> & buffer = data->buffer_;

  // The first things sent are the number of bytes to expect (not counting
  // these first bytes).
  // Return (will keep Listening) if these haven't been received yet.
  if (buffer.size() < sizeof(uint64_t))
    return true;

  uint64_t num_bytes = ByteStringToValue<uint64_t>(
      sizeof(uint64_t), (const unsigned char *)buffer.data());

  // Return (will keep listening) if not all the expected bytes have been received:
  //   sizeof(uint64_t) +  // To express 'num_bytes'
  //   num_bytes           // All of the bytes
  if (buffer.size() < sizeof(uint64_t) + num_bytes) {
    // Now that we know expected size of buffer, reserve this size, to
    // minimize the number of resizes that occur.
    data->buffer_.reserve(sizeof(uint64_t) + num_bytes);
    return true;
  }

  // We've received enough bytes. Alert Socket to stop listening on this connection
  // (set the return code to be OK if the exact number of expected bytes were read;
  // otherwise alert that there was an ERROR.).
  if (buffer.size() > sizeof(uint64_t) + num_bytes) {
    stats->abort_listening_with_code_.insert(
        ListenReturnCode::RECEIVED_UNEXPECTED_BYTES);
  } else {
    stats->abort_listening_with_code_.insert(ListenReturnCode::OK);
  }

  return true;
}

bool ReceiveBigEndianInt32Bytes(
    const SocketIdentifier & socket_id,
    ReceivedData * data,
    SocketStats * stats,
    void * not_used) {
  if (data == nullptr || stats == nullptr) {
    log_fatal("Null input to ReceiveBigEndianInt32Bytes.");
  }
  if (data->is_receiving_data_) {
    log_fatal("Synchronization problem in ReceiveBigEndianInt32Bytes. "
              "This should never happen.");
  }

  const vector<char> & buffer = data->buffer_;

  // The first things sent are the number of bytes to expect (not counting
  // these first bytes).
  // Return (will keep Listening) if these haven't been received yet.
  if (buffer.size() < sizeof(uint32_t))
    return true;

  uint32_t num_bytes = ByteStringToValue<uint32_t>(
      sizeof(uint32_t), (const unsigned char *)buffer.data());

  // Return (will keep listening) if not all the expected bytes have been received:
  //   sizeof(uint32_t) +  // To express 'num_bytes'
  //   num_bytes           // All of the bytes
  if (buffer.size() < sizeof(uint32_t) + num_bytes) {
    // Now that we know expected size of buffer, reserve this size, to
    // minimize the number of resizes that occur.
    data->buffer_.reserve(sizeof(uint32_t) + num_bytes);
    return true;
  }

  // We've received enough bytes. Alert Socket to stop listening on this connection
  // (set the return code to be OK if the exact number of expected bytes were read;
  // otherwise alert that there was an ERROR.).
  if (buffer.size() > sizeof(uint32_t) + num_bytes) {
    stats->abort_listening_with_code_.insert(
        ListenReturnCode::RECEIVED_UNEXPECTED_BYTES);
  } else {
    stats->abort_listening_with_code_.insert(ListenReturnCode::OK);
  }
  return true;
}

bool ReceiveNumBytes(
    const SocketIdentifier & socket_id,
    ReceivedData * data,
    SocketStats * stats,
    void * num_bytes) {
  if (data == nullptr || stats == nullptr || num_bytes == nullptr) {
    log_fatal("Null input to ReceiveNumBytes.");
  }
  if (data->is_receiving_data_) {
    log_fatal("Synchronization problem in ReceiveNumBytes. "
              "This should never happen.");
  }

  const uint64_t parsed_num_bytes = *((const uint64_t *)num_bytes);
  const vector<char> & buffer = data->buffer_;

  if (buffer.size() < parsed_num_bytes) {
    // Now that we know expected size of buffer, reserve this size, to
    // minimize the number of resizes that occur.
    data->buffer_.reserve(parsed_num_bytes);
  }

  // Return (will keep Listening) if haven't received all bytes yet.
  if (buffer.size() < parsed_num_bytes)
    return true;

  // We've received enough bytes. Alert Socket to stop listening on this connection
  // (set the return code to be OK if the exact number of expected bytes were read;
  // otherwise alert that there was an ERROR.).
  if (buffer.size() > parsed_num_bytes) {
    stats->abort_listening_with_code_.insert(
        ListenReturnCode::RECEIVED_UNEXPECTED_BYTES);
  } else {
    stats->abort_listening_with_code_.insert(ListenReturnCode::OK);
  }

  return true;
}

// ======================= END GENERIC UTIL FUNCTIONS ==========================

// ========================= SOCKET MEMBER FUNCTIONS ===========================

// Updates error_msg_ with the provided message, and also DLOG's error.
void Socket::SetErrorMessage(
    const bool print, const size_t & line, const string & error_msg) {
  error_msg_ += error_msg + "\n";

  // Now print message to terminal, if appropriate.
  if (!print)
    return;
  log_error("%s", error_msg.c_str());
}

void Socket::SetListenTimeout(const uint64_t & ms) {
  // Update the Listen() stop condition, to enforce a timeout.
  if (listen_params_.type_.find(ListenType::USE_STOP_CONDITION) ==
      listen_params_.type_.end()) {
    // The old ListenParams did not use a stop condition. Add a new one.
    listen_params_.type_.insert(ListenType::USE_STOP_CONDITION);
    listen_params_.stop_params_.op_ = ConditionOperation::IDENTITY;
    listen_params_.stop_params_.num_msec_since_last_event_ = ms;
    listen_params_.stop_params_.num_errors_seen_ = 0;
  } else {
    // The old ListenParams already used a stop condition. Add another to it.
    const StopListeningCondition old_params =
        listen_params_.stop_params_;
    listen_params_.stop_params_.Reset();
    listen_params_.stop_params_.op_ = ConditionOperation::OR;
    listen_params_.stop_params_.left_condition_ =
        unique_ptr<StopListeningCondition>(
            new StopListeningCondition());
    listen_params_.stop_params_.left_condition_->clone(old_params);
    listen_params_.stop_params_.right_condition_ =
        unique_ptr<StopListeningCondition>(
            new StopListeningCondition());
    listen_params_.stop_params_.right_condition_->op_ =
        ConditionOperation::IDENTITY;
    listen_params_.stop_params_.right_condition_
        ->num_msec_since_last_event_ = ms;
    listen_params_.stop_params_.right_condition_->num_errors_seen_ = 0;
  }
}

void Socket::SetListenParams(const ListenParams & params) {
  // User may only want to reset some of the params. Make sure unset
  // fields (namely, ip and port) retain their old value.
  const string old_ip = listen_params_.ip_;
  const unsigned long old_port = listen_params_.port_;
  listen_params_ = params;
  if (listen_params_.ip_.empty())
    listen_params_.ip_ = old_ip;
  if (listen_params_.port_ == 0)
    listen_params_.port_ = old_port;
}

void Socket::SetListenReceiveDataCallback(
    fn_w_args_ptr fn_callback, void * args) {
  listen_params_.receive_data_ = fn_callback;
  listen_params_.receive_data_args_ = args;
  // The callback will never be called if listen_params_.type_ does
  // not indicate to handle receive events. Be sure it is set to handle them.
  if (fn_callback != nullptr) {
    if (listen_params_.type_.find(ListenType::HANDLE_RECEIVE) ==
            listen_params_.type_.end() &&
        listen_params_.type_.find(ListenType::HANDLE_ALL_EVENTS) ==
            listen_params_.type_.end()) {
      listen_params_.type_.insert(ListenType::HANDLE_RECEIVE);
    }
  }
}

void Socket::SetListenReceiveDataCallback(fn_w_args_ptr fn_callback) {
  listen_params_.receive_data_ = fn_callback;
  // Caller will pass in null-ptr to indicate to return control to caller
  // as soon as bytes are received, rather than have the socket call a
  // helper function to determine how to receive the bytes.
  // In this case, be sure that listen_params.type_ does not include
  // ListenType::HANDLE_RECEIVE nor HANDLE_ALL_EVENTS; otherwise, be
  // sure it *does* include at least one of these.
  if (fn_callback == nullptr) {
    listen_params_.type_.erase(ListenType::HANDLE_RECEIVE);
    listen_params_.type_.erase(ListenType::HANDLE_ALL_EVENTS);
  } else if (
      listen_params_.type_.find(ListenType::HANDLE_RECEIVE) ==
          listen_params_.type_.end() &&
      listen_params_.type_.find(ListenType::HANDLE_ALL_EVENTS) ==
          listen_params_.type_.end()) {
    listen_params_.type_.insert(ListenType::HANDLE_RECEIVE);
  }
}

bool Socket::ShouldKeepListening(
    const SocketStats & stats,
    const StopListeningCondition & stop_params,
    set<ListenReturnCode> * return_code) {
  // Return false if stats.abort_listening_with_code_ is set.
  if (!stats.abort_listening_with_code_.empty()) {
    *return_code = stats.abort_listening_with_code_;
    return false;
  }

  // Return true if stop_params haven't been set.
  if (stop_params.op_ == ConditionOperation::NONE) {
    return true;
  }

  // If this is a parent/root condition, parse the sub-conditions and return
  // appropriately.
  if (stop_params.op_ != ConditionOperation::IDENTITY) {
    if (stop_params.left_condition_ == nullptr) {
      log_fatal("Parent StopListeningCondition has null leaf.");
    }
    if ((stop_params.op_ != ConditionOperation::NOT &&
         stop_params.right_condition_ == nullptr)) {
      log_fatal("Parent StopListeningCondition has null leaf.");
    }
    if (stop_params.op_ == ConditionOperation::NOT) {
      return !ShouldKeepListening(
          stats, *stop_params.left_condition_, return_code);
    } else if (stop_params.op_ == ConditionOperation::AND) {
      return (
          ShouldKeepListening(
              stats, *stop_params.left_condition_, return_code) ||
          ShouldKeepListening(
              stats, *stop_params.right_condition_, return_code));
    } else if (stop_params.op_ == ConditionOperation::OR) {
      return (
          ShouldKeepListening(
              stats, *stop_params.left_condition_, return_code) &&
          ShouldKeepListening(
              stats, *stop_params.right_condition_, return_code));
    } else if (stop_params.op_ == ConditionOperation::XOR) {
      return (
          ShouldKeepListening(
              stats, *stop_params.left_condition_, return_code) !=
          ShouldKeepListening(
              stats, *stop_params.right_condition_, return_code));
    } else {
      string msg(
          "Unexpected ConditionOperation: " +
          Itoa(static_cast<int>(stop_params.op_)));
      log_fatal("%s", msg.c_str());
    }
  }

  // This is a 'leaf' condition (op_ == IDENTITY). Find the non-default field,
  // and compare it to 'stats' to see if the STOP criterion has been met.
  if (stop_params.num_errors_seen_ > 0) {
    if (stats.num_errors_seen_ >= stop_params.num_errors_seen_) {
      return_code->insert(ListenReturnCode::TOO_MANY_ERRORS);
      return false;
    }
    return true;
  }
  if (stop_params.num_client_errors_seen_ > 0) {
    if (stats.num_client_socket_errors_ >=
        stop_params.num_client_errors_seen_) {
      return_code->insert(ListenReturnCode::CLIENT_SOCKET_ERROR_EVENT);
      return false;
    }
    return true;
  }
  if (stop_params.num_server_errors_seen_ > 0) {
    if (stats.num_server_socket_errors_ >=
        stop_params.num_server_errors_seen_) {
      return_code->insert(ListenReturnCode::SERVER_SOCKET_ERROR_EVENT);
      return false;
    }
    return true;
  }
  if (stop_params.num_select_errors_seen_ > 0) {
    if (stats.num_select_errors_ >=
        stop_params.num_select_errors_seen_) {
      return_code->insert(ListenReturnCode::SELECT_ERROR);
      return false;
    }
    return true;
  }
  if (stop_params.num_event_errors_seen_ > 0) {
    if (stats.num_event_errors_ >= stop_params.num_event_errors_seen_) {
      return_code->insert(ListenReturnCode::ERROR_EVENT);
      return false;
    }
    return true;
  }
  if (stop_params.num_send_errors_seen_ > 0) {
    if (stats.num_send_errors_ >= stop_params.num_send_errors_seen_) {
      return_code->insert(ListenReturnCode::SEND_ERROR);
      return false;
    }
    return true;
  }
  if (stop_params.num_receive_errors_seen_ > 0) {
    if (stats.num_receive_errors_ >=
        stop_params.num_receive_errors_seen_) {
      return_code->insert(ListenReturnCode::RECEIVE_ERROR);
      return false;
    }
    return true;
  }
  if (stop_params.num_connection_errors_seen_ > 0) {
    if (stats.num_connection_errors_ >=
        stop_params.num_connection_errors_seen_) {
      return_code->insert(ListenReturnCode::CONNECT_ERROR);
      return false;
    }
    return true;
  }
  if (stop_params.num_disconnect_errors_seen_ > 0) {
    if (stats.num_disconnect_errors_ >=
        stop_params.num_disconnect_errors_seen_) {
      return_code->insert(ListenReturnCode::DISCONNECT_ERROR);
      return false;
    }
    return true;
  }
  if (stop_params.num_msec_since_last_event_ > 0) {
    if (stats.msec_since_last_event_ >=
        stop_params.num_msec_since_last_event_) {
      return_code->insert(ListenReturnCode::NO_ACTIVITY);
      return false;
    }
    return true;
  }
  if (stop_params.num_current_connections_ >= 0) {
    if (stats.num_current_connections_ >=
            stop_params.num_current_connections_ ||
        stats.num_peak_connections_ >=
            stop_params.num_current_connections_) {
      return_code->insert(ListenReturnCode::TOO_MANY_CONNECTIONS);
      return false;
    }
    return true;
  }
  if (stop_params.num_distinct_connections_seen_ >= 0) {
    if (stats.num_distinct_connections_seen_ >=
        stop_params.num_distinct_connections_seen_) {
      return_code->insert(
          ListenReturnCode::CUMULATIVE_CONNECTIONS_THRESHOLD);
      return false;
    }
    return true;
  }
  if (stop_params.num_sent_bytes_ >= 0) {
    if (stats.num_sent_bytes_ >= stop_params.num_sent_bytes_) {
      return_code->insert(ListenReturnCode::CUMULATIVE_BYTES_SENT);
      return false;
    }
    return true;
  }
  if (stop_params.num_sent_transactions_ >= 0) {
    if (stats.num_sent_transactions_ >=
        stop_params.num_sent_transactions_) {
      return_code->insert(
          ListenReturnCode::CUMULATIVE_SEND_TRANSACTIONS);
      return false;
    }
    return true;
  }
  if (stop_params.num_received_bytes_ >= 0) {
    if (stats.num_received_bytes_ >= stop_params.num_received_bytes_) {
      return_code->insert(ListenReturnCode::CUMULATIVE_BYTES_RECEIVED);
      return false;
    }
    return true;
  }
  if (stop_params.num_received_transactions_ >= 0) {
    if (stats.num_received_transactions_ >=
        stop_params.num_received_transactions_) {
      return_code->insert(
          ListenReturnCode::CUMULATIVE_RECEIVE_TRANSACTIONS);
      return false;
    }
    return true;
  }
  if (stop_params.num_received_null_characters_ >= 0) {
    if (stats.num_received_null_characters_ >=
        stop_params.num_received_null_characters_) {
      return_code->insert(
          ListenReturnCode::CUMULATIVE_RECEIVE_NULL_CHARS);
      return false;
    }
    return true;
  }

  // All fields have default-values; this is an error.
  log_fatal("Unable to ShouldKeepListening: All fields of leaf "
            "stopping condition have default values.");
  return true;
}

bool Socket::ShouldKeepListening(set<ListenReturnCode> * return_codes) {
  const bool check_stop_params =
      listen_params_.type_.find(ListenType::USE_STOP_CONDITION) !=
      listen_params_.type_.end();
  return ShouldKeepListening(
      stats_,
      (check_stop_params ? listen_params_.stop_params_ :
                           kDefaultStopListeningCondition),
      return_codes);
}

// ================================ SOCKET SETUP ===============================

bool Socket::GetLocalHost(string * ip) {
  if (!Initialize()) {
    SetErrorMessage(__LINE__, "Unable to Initialize()");
    return false;
  }

  if (!SocketGetLocalHost(ip)) {
    SetErrorMessage(
        __LINE__, "Unable to GetLocalHost(): Null HostInfo.");
    return false;
  }

  return true;
}

bool Socket::GetWithinNetworkIP(string * ip) {
  if (!Initialize()) {
    SetErrorMessage(__LINE__, "Unable to Initialize()");
    return false;
  }
  if (!SocketGetWithinNetworkIP(ip)) {
    SetErrorMessage(
        __LINE__, "Unable to GetWithinNetworkIP(): Null HostInfo.");
    return false;
  }

  return true;
}

bool Socket::SetupSocket() {
  ++stats_.num_setup_socket_calls_;
  if (stats_.activate_timers_) {
    StartTimer(&stats_.setup_socket_timer_);
  }

  // Initialize socket (will return right away if socket already initialized).
  if (!Initialize()) {
    SetErrorMessage(__LINE__, "Unable to Initialize()");
    if (stats_.activate_timers_) {
      StopTimer(&stats_.setup_socket_timer_);
    }
    return false;
  }

  if (!is_socket_initialized_) {
    if (role_ == SocketRole::SERVER) {
      if (!SocketSetupServerSocket()) {
        SetErrorMessage(
            __LINE__,
            "Failed to SetupSocket() Socket: "
            "Unable to create server socket.");
        if (stats_.activate_timers_) {
          StopTimer(&stats_.setup_socket_timer_);
        }
        return false;
      }

      // Allow Server to start and stop, making sure it has the ability to
      // re-start listening on this socket.
      if (!SocketSetServerSocketOption(
              SocketGetSocketLayerCode(),
              SocketGetSocketReusePortCode())) {
        SetErrorMessage(
            __LINE__,
            "Failed to SetupSocket() Socket: "
            "Unable to set server socket SO_REUSEADDR setting: " +
                Itoa(SocketGetLastError()));
        if (stats_.activate_timers_) {
          StopTimer(&stats_.setup_socket_timer_);
        }
        return false;
      }
    } else {
      // Create a Socket for connecting to server.
      if (!SocketSetupClientSocket()) {
        SetErrorMessage(
            __LINE__,
            "Failed to SetupSocket() Socket: "
            "Unable to create client socket.");
        if (stats_.activate_timers_) {
          StopTimer(&stats_.setup_socket_timer_);
        }
        return false;
      }
    }

    is_socket_initialized_ = true;
  }

  // Stop Timer.
  if (stats_.activate_timers_) {
    StopTimer(&stats_.setup_socket_timer_);
  }

  return true;
}
// ============================== END SOCKET SETUP =============================

// ================================ CONNECT ====================================
bool Socket::RemoveConnection(
    vector<SocketIdentifier>::iterator & itr) {
  ++stats_.num_remove_connection_calls_;
  if (stats_.activate_timers_) {
    StartTimer(&stats_.remove_connection_timer_);
  }

  // Shutdown the connection.
  const bool to_return = ShutdownConnection(*itr);

  // Remove the connection from connections_.
  itr = connections_.erase(itr);

  if (stats_.activate_timers_) {
    StopTimer(&stats_.remove_connection_timer_);
  }

  return to_return;
}

bool Socket::ShutdownConnection(const SocketIdentifier & s) {
  if (!SocketShutdownConnection(s)) {
    SetErrorMessage(
        __LINE__,
        "Failed to close socket: " + Itoa(SocketGetLastError()));
    stats_.num_errors_seen_++;
    stats_.num_disconnect_errors_++;
    return false;
  }

  stats_.num_current_connections_--;
  return true;
}

bool Socket::ShutdownAllConnections() {
  string shutdown_error = "";

  // Close all connections_.
  for (SocketIdentifier & s : connections_) {
    ++stats_.num_close_socket_calls_;
    if (stats_.activate_timers_) {
      StartTimer(&stats_.close_socket_timer_);
    }
    if (!SocketShutdownConnection(s)) {
      const int last_error_code = SocketGetLastError();
      // closesocket() will fail if Client has already terminated the connection.
      // In this case, there is no actual error, even though close socket returns
      // SocketSocketErrorCode(), with SocketGetLastError() returning WSAENOTSOCK.
      if (last_error_code != SocketSocketUnavailableCode()) {
        shutdown_error += "Failed to close socket " + PrintSocketId(s) +
            ": " + Itoa(last_error_code) + ".\n";
      }
    }
    if (stats_.activate_timers_) {
      StopTimer(&stats_.close_socket_timer_);
    }
  }

  connections_.clear();

  received_bytes_per_socket_.clear();
  bytes_to_send_per_socket_.clear();

  if (!shutdown_error.empty()) {
    SetErrorMessage(__LINE__, shutdown_error);
    return false;
  }

  return true;
}

bool Socket::CloseSocket() {
  if (!is_socket_initialized_)
    return true;
  return Disconnect();
}

bool Socket::CloseClientSocket() {
  if (stats_.activate_timers_) {
    StartTimer(&stats_.client_close_socket_timer_);
  }

  // Make sure socket is initialized.
  if (!is_socket_initialized_) {
    if (stats_.activate_timers_) {
      StopTimer(&stats_.client_close_socket_timer_);
    }
    SetErrorMessage(
        __LINE__,
        "Failed to CloseClientSocket(): is_socket_initialized_ = 0.");
    return false;
  }

  // Remove any pending items in the Send buffer.
  bytes_to_send_per_socket_.clear();

  // Clear stats_.
  ResetSocketStatsLight();

  // There should be a single connection for Socket in the Client role.
  // Sanity-check this is true, and then close it.
  bool connection_already_removed = false;
  if (connections_.empty()) {
    // Nothing to do: Sometimes (e.g. if Listen() was called, and
    // Server terminated connection before data was sent) the connection_
    // has already been removed by the time CloseClientSocket() is called.
    // This is not an error (when such a thing happens, if it was an error
    // when it happened, then the error should have already been reported).
    connection_already_removed = true;
  } else if (connections_.size() != 1) {
    if (stats_.activate_timers_) {
      StopTimer(&stats_.client_close_socket_timer_);
    }
    SetErrorMessage(
        __LINE__,
        "Failed to CloseClientSocket(): Expected Client to have a "
        "single "
        "connection, but found: " +
            Itoa(connections_.size()));
    for (int i = 0; i < connections_.size(); ++i) {
      const SocketIdentifier & s = connections_[i];
      SetErrorMessage(
          __LINE__,
          "Connection " + Itoa(i + 1) + ": " + PrintSocketId(s));
    }
    return false;
  }

  // Call OS-specific code to close the socket connection.
  if (!SocketCloseClientSocket(connection_already_removed)) {
    if (stats_.activate_timers_) {
      StopTimer(&stats_.client_close_socket_timer_);
    }
    SetErrorMessage(
        __LINE__,
        "Failed to shutdown(): " + Itoa(SocketGetLastError()));
    return false;
  }

  connections_.clear();
  is_socket_initialized_ = false;

  // Clear error message.
  error_msg_ = "";

  if (stats_.activate_timers_) {
    StopTimer(&stats_.client_close_socket_timer_);
  }

  return true;
}

bool Socket::CloseServerSocket() {
  if (stats_.activate_timers_) {
    StartTimer(&stats_.server_close_socket_timer_);
  }

  // Sanity-check Server Socket had been initialized.
  if (!is_socket_initialized_) {
    if (stats_.activate_timers_) {
      StopTimer(&stats_.server_close_socket_timer_);
    }
    SetErrorMessage(
        __LINE__,
        "Failed to CloseServerSocket(): server socket wasn't "
        "initialized. This should never happen.");
    return false;
  }

  // Make sure the role is not CLIENT.
  if (role_ == SocketRole::CLIENT) {
    if (stats_.activate_timers_) {
      StopTimer(&stats_.server_close_socket_timer_);
    }
    SetErrorMessage(
        __LINE__, "Failed CloseServerSocket() Socket for CLIENT.");
    return false;
  }

  // Cleanup all connections.
  if (!ShutdownAllConnections()) {
    if (stats_.activate_timers_) {
      StopTimer(&stats_.server_close_socket_timer_);
    }
    SetErrorMessage(
        __LINE__,
        "Failed CloseServerSocket(): Unable to clear connections_.");
    return false;
  }

  // Close the socket.
  server_socket_is_bound_and_accepting_connection_requests_ = false;
  if (!SocketCloseServerSocket()) {
    if (stats_.activate_timers_) {
      StopTimer(&stats_.server_close_socket_timer_);
    }
    SetErrorMessage(
        __LINE__,
        "Failed to shutdown(): " + Itoa(SocketGetLastError()));
    return false;
  }
  is_socket_initialized_ = false;

  // Clear stats_.
  ResetSocketStatsLight();

  // Clear error message.
  error_msg_ = "";

  if (stats_.activate_timers_) {
    StopTimer(&stats_.server_close_socket_timer_);
  }

  return true;
}

bool Socket::Disconnect() {
  if (stats_.activate_timers_) {
    StartTimer(&stats_.disconnect_timer_);
  }

  if (!is_socket_initialized_) {
    SetErrorMessage(
        __LINE__, "Failed to Disconnect(): No connections present.");
    if (stats_.activate_timers_) {
      StopTimer(&stats_.disconnect_timer_);
    }
    return false;
  }

  if ((role_ == SocketRole::CLIENT && !CloseClientSocket()) ||
      (role_ == SocketRole::SERVER && !CloseServerSocket())) {
    if (stats_.activate_timers_) {
      StopTimer(&stats_.disconnect_timer_);
    }
    return false;
  }

  // Stop Timer.
  if (stats_.activate_timers_) {
    StopTimer(&stats_.disconnect_timer_);
  }

  return true;
}
// ============================== END CONNECT ==================================

// ================================= LISTEN ====================================
set<ListenReturnCode> Socket::Listen(bool clear_receive_buffer) {
  /* DEPRECATED. There are now other kinds of Socket classes, that don't use
   * ip/port.
  // Make sure IP and Port have been set.
  if (role_ == SocketRole::CLIENT) {
    if (connect_params_.ip_.empty() || connect_params_.port_ == 0) {
      SetErrorMessage(__LINE__,
          "Failed to Listen() on malformed ip '" + connect_params_.ip_ +
          "' and port " + Itoa(connect_params_.port_) + ".");
      return CreateSet(ListenReturnCode::BAD_INITIALIZATION_ERROR);
    }
  } else {
    if (listen_params_.ip_.empty() || listen_params_.port_ == 0) {
      SetErrorMessage(__LINE__,
          "Failed to Listen() on malformed ip '" + listen_params_.ip_ +
          "' and port " + Itoa(listen_params_.port_) + ".");
      return CreateSet(ListenReturnCode::BAD_INITIALIZATION_ERROR);
    }
  }
  */

  // Setup socket, if not done already.
  if (!is_socket_initialized_ && !SetupSocket()) {
    SetErrorMessage(__LINE__, "Unable to SetupSocket()");
    return CreateSet(ListenReturnCode::BAD_INITIALIZATION_ERROR);
  }

  // If this is a client, connect if haven't already.
  if (role_ == SocketRole::CLIENT && !IsConnected() && !Connect()) {
    SetErrorMessage(__LINE__, "Unable to connect to Server.");
    return CreateSet(ListenReturnCode::BAD_INITIALIZATION_ERROR);
  }

  // First check if there is already something in the receive buffer
  // along this connection: it's possible that all bytes have already
  // been received, in which case the call to ThreadListen() below
  // will hang forever, since there will be no 'receive' event (i.e.
  // select() will never succeed), and thus ThreadListen() won't do anything.
  if ((role_ == SocketRole::SERVER && clear_receive_buffer) ||
      (role_ == SocketRole::CLIENT &&
       received_bytes_per_socket_.size() == 1)) {
    for (map<SocketIdentifier, ReceivedData>::iterator itr =
             received_bytes_per_socket_.begin();
         itr != received_bytes_per_socket_.end();
         ++itr) {
      const SocketIdentifier & s = itr->first;
      ReceivedData & received_data = itr->second;
      if (!received_data.buffer_.empty() &&
          !HandleReceivedBytes(listen_params_, s, &received_data)) {
        SetErrorMessage(
            __LINE__,
            "Failed to Listen(): Failed to parse existing bytes in "
            "Receive buffer.");
        return CreateSet(ListenReturnCode::BAD_INITIALIZATION_ERROR);
      }
    }
  }

  return ThreadListen();
}

set<ListenReturnCode> Socket::CallThreadListen(const bool as_server) {
  // Create a listener thread to listen for connection requests.
  //   - Solution 1: Have main thread infinitely loop through 'select()'
  return ThreadListen(/* as_server */);
  //   - Solution 3c: Use std::thread
  // thread listener(ThreadListen);
  // listener.join();
  // return true;
  //   - Solution 3b: Use PThread.
  /* Not Working: Compilier hiccups during linking.
  pthread_t listener;
  Socket* temp = this;
  const int thread_creation_code =
      pthread_create(&listener, NULL, &Socket::CallPThreadListen, (void*) temp);
  if (thread_creation_code) {
    SetErrorMessage(__LINE__,
        "Failed to create listener thread: " + Itoa(thread_creation_code));
    return false;
  }
  */
}

void * Socket::CallPThreadListen(void * windows_socket_class_instance) {
  // NOTE: Not sure which of the following two lines is correct; and couldn't
  // test them both since I never got pthread to work...
  //return (void*) ((Socket*) windows_socket_class_instance)->ThreadListen();
  //pthread_exit((void*) ((Socket*) windows_socket_class_instance)->ThreadListen();
  return nullptr;
}

set<ListenReturnCode> Socket::ThreadListen() {
  // Need to do extra setup if this instance is a Server: have Server Socket
  // bind the socket.
  const bool is_server = role_ == SocketRole::SERVER;
  const bool is_client = role_ == SocketRole::CLIENT;
  if (is_server &&
      !server_socket_is_bound_and_accepting_connection_requests_) {
    ++stats_.num_bind_calls_;
    if (stats_.activate_timers_) {
      StartTimer(&stats_.bind_timer_);
    }

    // Binds a TCP/IP port (with the address specified in listen_params_.[ip, port]_).
    if (!SocketBind()) {
      if (stats_.activate_timers_) {
        StopTimer(&stats_.bind_timer_);
      }
      return HandleBindError();
    }

    // Initiate listening for incoming connection requests.
    ++stats_.num_listen_calls_;
    if (stats_.activate_timers_) {
      StartTimer(&stats_.listen_timer_);
    }

    if (!SocketServerListen()) {
      CloseServerSocket();
      SetErrorMessage(
          __LINE__,
          "Failed to initiate listening on server socket: " +
              Itoa(SocketGetLastError()));
      return CreateSet(ListenReturnCode::LISTEN_ERROR);
    }

    if (stats_.activate_timers_) {
      StopTimer(&stats_.listen_timer_);
    }
    server_socket_is_bound_and_accepting_connection_requests_ = true;
  } else if (is_client) {
    // Client is listening for response from Server. Make sure this is consistent
    // with current state.
    if (connections_.size() != 1 ||
        !SocketIsClientSocket(connections_[0])) {
      SetErrorMessage(
          __LINE__,
          "Failed to Listen(): Connection to server is not "
          "established.");
      return CreateSet(ListenReturnCode::LISTEN_ERROR);
    }
  }

  // Keep listening until the stop condition is met, or an Event occurs that
  // needs to be handled by caller.
  ++stats_.num_times_listen_loop_called_;
  if (stats_.activate_timers_) {
    StartTimer(&stats_.listen_loop_timer_);
    if (connections_.empty())
      StartTimer(&stats_.server_awaiting_client_timer_);
  }

  set<ListenReturnCode> to_return;
  while (ShouldKeepListening(&to_return)) {
    ++stats_.num_times_in_listen_loop_;

    // Check for a "Connection Event", which is one of:
    //   a) A connection request from a new connection; or
    //   b) A send/receive request from an existing connection; or
    //   c) A disconnect request/alert
    //   d) An error
    // (a) is detected based on the presence of 'Server Socket' in 'read_sockets'
    // (b) is detected based on the presence of one of the connections_ in
    //     '[read | write]_sockets'
    // (c) is detected based on the presence of one of the connections_ in
    //     'read_sockets' (and then num_bytes_read will be -1, and error will
    //     indicate the socket was closed on the other end)
    // (d) is detected based on the presence of one of the connections_ in
    //     'error_sockets', and/or a negative return value from select().
    if (stats_.activate_timers_) {
      StartTimer(&stats_.listen_select_timer_);
    }

    set<SocketIdentifier> read_sockets, error_sockets;
    const int return_value = SocketSelect(
        (is_server ? SocketGetServerSocketId() :
                     SocketGetReceiveSocketId()),
        0,
        listen_params_.select_timeout_ms_ * 1000,
        &read_sockets,
        nullptr,
        &error_sockets);

    if (stats_.activate_timers_) {
      StopTimer(&stats_.listen_select_timer_);
    }

    if (return_value < 0) {
      // select() returned an error; return.
      const int last_error_code = SocketGetLastError();
      stats_.num_errors_seen_++;
      stats_.num_select_errors_++;
      // Check if (one of) the connection(s) was terminated by the other end.
      if (last_error_code == SocketSocketUnavailableCode()) {
        if (!is_server && connections_.size() == 1 &&
            SocketIsClientSocket(connections_[0])) {
          log_warn("Failed select() within Listen() for Client Socket. "
                   "This typically means the Server terminated the "
                   "connection. "
                   "Removing Client Socket from connections_...");
          connections_.clear();
          if (stats_.activate_timers_) {
            StopTimer(&stats_.listen_loop_timer_);
          }
          return CreateSet(ListenReturnCode::SELECT_ERROR);
        } else if (!is_server) {
          SetErrorMessage(
              __LINE__, "Unexpected select() error for Listen()");
          if (stats_.activate_timers_) {
            StopTimer(&stats_.listen_loop_timer_);
          }
          return CreateSet(ListenReturnCode::SELECT_ERROR);
        } else {
          // TODO(paul): One of the Client's terminated their end of the
          // connection. Figure out which one it was, and call Disconnect()
          // on it.
          log_warn("A Client terminated the connection. Remove it "
                   "gracefully.");
          continue;
        }
      } else {
        SetErrorMessage(
            __LINE__, "Failed select(): " + Itoa(last_error_code));
        continue;
      }
    } else if (return_value == 0) {
      stats_.msec_since_last_event_ +=
          listen_params_.select_timeout_ms_;
      // No Events; sleep, then call select again.
      if (listen_params_.no_event_sleep_ms_ > 0) {
        SocketSleep(listen_params_.no_event_sleep_ms_ * 1000);
      }
      continue;
    } else {
      // An Event occurred. Process it.
      stats_.msec_since_last_event_ = 0;

      // Check if the Event is an Error event on the main socket.
      if (SocketIsServerSocketInSet(error_sockets)) {
        stats_.num_errors_seen_++;
        stats_.num_server_socket_errors_++;
        SetErrorMessage(
            __LINE__,
            "select() detected an error on the main Server socket: " +
                Itoa(SocketGetServerSocketError()));
        // TODO(paul): Determine the best way to handle errors on the main Client/
        // Server socket: e.g., there may be events on the non-main socket that
        // should be processed here, so maybe don't return? Ditto below.
        continue;
      }
      if (SocketIsClientSocketInSet(error_sockets)) {
        stats_.num_errors_seen_++;
        stats_.num_client_socket_errors_++;
        SetErrorMessage(
            __LINE__,
            "select() detected an error on the main Client socket: " +
                Itoa(SocketGetClientSocketError()));
        continue;
      }

      // Check if it is a New Connection request (new connection requests will
      // cause Server Socket to appear in read_sockets).
      if (SocketIsServerSocketInSet(read_sockets)) {
        if (!is_server) {
          log_fatal(
              "Server Socket should only be in read_sockets when "
              "running "
              "Listen() as Server. This check should never fail...");
        }
        if (!HandleNewConnection())
          continue;
      }

      // Now, handle all other Event types: Go through all connections_,
      // and if a given connection has had an Event (based on its
      // presence in one of [read | error]_sockets), handle it.
      if (!HandleSelectEvents(&read_sockets, &error_sockets)) {
        continue;
      }
    }
  }

  if (stats_.activate_timers_) {
    StopTimer(&stats_.listen_loop_timer_);
  }

  return to_return;
}

const vector<char> *
Socket::GetReceivedBytesVector(const SocketIdentifier & s) {
  map<SocketIdentifier, ReceivedData>::iterator itr =
      received_bytes_per_socket_.find(s);
  if (itr == received_bytes_per_socket_.end())
    return nullptr;
  return &(itr->second.buffer_);
}

const vector<char> *
Socket::GetReceivedBytesVector(const uint64_t & socket_index) {
  if (received_bytes_per_socket_.size() <= socket_index)
    return nullptr;
  map<SocketIdentifier, ReceivedData>::iterator itr =
      received_bytes_per_socket_.begin();
  advance(itr, socket_index);
  return &(itr->second.buffer_);
}

bool Socket::SwapBuffer(
    const SocketIdentifier & s, vector<char> * output) {
  if (output == nullptr)
    return false;
  map<SocketIdentifier, ReceivedData>::iterator itr =
      received_bytes_per_socket_.find(s);
  if (itr == received_bytes_per_socket_.end())
    return false;
  output->swap(itr->second.buffer_);
  return true;
}

bool Socket::SwapBuffer(const uint64_t & index, vector<char> * output) {
  if (output == nullptr)
    return false;
  if (received_bytes_per_socket_.size() <= index)
    return false;
  map<SocketIdentifier, ReceivedData>::iterator itr =
      received_bytes_per_socket_.begin();
  advance(itr, index);
  output->swap(itr->second.buffer_);
  return true;
}

// DEPRECATED: The following functions are commented-out, because I don't think
// they do what they are designed to do: move a sub-vector of buffer_ to
// the provided container, with constant cost; i.e. just set the pointer
// of the input vector to point to the 'start_index' element of buffer_.
// Indeed, I think 'swap_ranges' below will just do a copy.
// I don't think C++ has a default function to use for this, since not
// swapping the whole vectors threatens the 'vector' guarantee that memory
// will lie in contiguous bytes. One way to manually implement the functions
// below would be to just do:
//   output->data() = (buffer_.data()) + start_index;
// But this is a little risky, since now output will be made invalid if
// buffer_ is ever cleared/modified.
// In the end, I decided not to implement these, and just rely on each use-case
// either doing the above trick (safe, if we know buffer_ will remain in scope
// and unchanged for as long as 'output' is needed), or just set output =
// buffer_, and then offset the index by 'start_index' whenever accessing
// an element in 'output'. Or, if you really need a vector with the
// appropriate elements and size, use CopyBuffer() below (incurs the linear
// cost of copying all vector elements).
/*
// Same as above, but only swaps the tail end of buffer_ (starting at
// start_index).
// TODO(PHB): Consider also having an API that takes in a range:
// [start_index, end_index). If you do this, also do it for the API below.
bool Socket::SwapBuffer(const SocketIdentifier& s, const uint64_t& start_index,
                        vector<char>* output) {
  if (output == nullptr) return false;
  map<SocketIdentifier, ReceivedData>::iterator itr =
      received_bytes_per_socket_.find(s);
  if (itr == received_bytes_per_socket_.end()) return false;
  output->resize(itr->second.buffer_.size());
  swap_ranges(itr->second.buffer_.begin(), itr->second.buffer_.end(),
              output->begin());
  return true;
}
// Same as above, but API gives a location within received_bytes_per_socket_
// instead of a SocketIdentifier.
bool Socket::SwapBuffer(const uint64_t& socket_index, const uint64_t& start_index,
                        vector<char>* output) {
  if (output == nullptr) return false;
  if (received_bytes_per_socket_.size() <= socket_index) return false;
  map<SocketIdentifier, ReceivedData>::iterator itr =
      received_bytes_per_socket_.begin();
  advance(itr, socket_index);
  output->resize(itr->second.buffer_.size());
  swap_ranges(itr->second.buffer_.begin(), itr->second.buffer_.end(),
              output->begin());
  return true;
}
*/

bool Socket::CopyBuffer(
    const SocketIdentifier & s,
    const uint64_t & start_index,
    vector<char> * output) {
  if (output == nullptr)
    return false;
  map<SocketIdentifier, ReceivedData>::iterator itr =
      received_bytes_per_socket_.find(s);
  if (itr == received_bytes_per_socket_.end())
    return false;
  output->resize(itr->second.buffer_.size() - start_index);
  copy(
      itr->second.buffer_.begin() + start_index,
      itr->second.buffer_.end(),
      output->begin());
  return true;
}

bool Socket::CopyBuffer(
    const uint64_t & socket_index,
    const uint64_t & start_index,
    vector<char> * output) {
  if (output == nullptr)
    return false;
  if (received_bytes_per_socket_.size() <= socket_index)
    return false;
  map<SocketIdentifier, ReceivedData>::iterator itr =
      received_bytes_per_socket_.begin();
  advance(itr, socket_index);
  output->resize(itr->second.buffer_.size() - start_index);
  copy(
      itr->second.buffer_.begin() + start_index,
      itr->second.buffer_.end(),
      output->begin());
  return true;
}

bool Socket::CopyBuffer(
    const SocketIdentifier & s,
    const uint64_t & start_index,
    uint64_t * num_copied_bytes,
    char * output) {
  if (output == nullptr)
    return false;
  map<SocketIdentifier, ReceivedData>::iterator itr =
      received_bytes_per_socket_.find(s);
  if (itr == received_bytes_per_socket_.end())
    return false;
  const uint64_t num_bytes_to_copy =
      (num_copied_bytes != nullptr && *num_copied_bytes > 0) ?
      *num_copied_bytes :
      itr->second.buffer_.size() - start_index;
  if (num_copied_bytes != nullptr) {
    if (*num_copied_bytes == 0) {
      *num_copied_bytes = num_bytes_to_copy;
    } else if (
        *num_copied_bytes > itr->second.buffer_.size() - start_index) {
      return false;
    }
  }
  memcpy(
      output,
      itr->second.buffer_.data() + start_index,
      num_bytes_to_copy);
  return true;
}

bool Socket::CopyBuffer(
    const uint64_t & socket_index,
    const uint64_t & start_index,
    uint64_t * num_copied_bytes,
    char * output) {
  if (output == nullptr)
    return false;
  if (received_bytes_per_socket_.size() <= socket_index)
    return false;
  map<SocketIdentifier, ReceivedData>::iterator itr =
      received_bytes_per_socket_.begin();
  advance(itr, socket_index);
  const uint64_t num_bytes_to_copy =
      (num_copied_bytes != nullptr && *num_copied_bytes > 0) ?
      *num_copied_bytes :
      itr->second.buffer_.size() - start_index;
  if (num_copied_bytes != nullptr) {
    if (*num_copied_bytes == 0) {
      *num_copied_bytes = num_bytes_to_copy;
    } else if (
        *num_copied_bytes > itr->second.buffer_.size() - start_index) {
      return false;
    }
  }
  memcpy(
      output,
      itr->second.buffer_.data() + start_index,
      num_bytes_to_copy);
  return true;
}

int64_t Socket::ComputeConnectSleepTime(const bool print_message) {
  // Make sure we haven't exceeded the number of allowed failed connect attempts.
  if (connect_params_.num_retries_ == 0 ||
      connect_params_.sleep_time_ < 0 ||
      (connect_params_.num_retries_ >= 0 &&
       connect_params_.num_retries_ < stats_.num_connection_errors_)) {
    return -1;
  }

  int64_t sleep_time = connect_params_.sleep_time_;
  // Compute Sleep Time, based on number of failed attampts so far and
  // connect_params_ settings.
  if (connect_params_.sleep_time_ > 0) {
    const int num_times_to_double_sleep_time =
        connect_params_.num_tries_at_current_sleep_time_ <= 0 ?
        0 :
        (stats_.num_connection_errors_ /
         connect_params_.num_tries_at_current_sleep_time_);
    // INT64_MAX is 2^64 - 1, so since sleep_time_ is uint64_t, once we
    // double it 63 times, we're sure to have wrapped around. In particular,
    // see a few lines below, where 'sleep_time' is set based on the
    // original ConnectParams.sleep_time_ field, multiplied by:
    //   (1 << num_times_to_double_sleep_time)
    // This is where the '63' below comes from.
    if (num_times_to_double_sleep_time > 63) {
      if (stats_.activate_timers_) {
        StopTimer(&stats_.client_awaiting_server_timer_);
      }
      // Doubled too many times (and in particular, the number of milliseconds
      // to sleep can no longer be expressed as uint64_t).
      SetErrorMessage(
          __LINE__,
          "Failed to connect client socket: Too many failures.");
      return false;
    }
    sleep_time = connect_params_.sleep_time_ *
        ((uint64_t)1 << num_times_to_double_sleep_time);
    // The test below will catch if sleep time has overflown.
    if (sleep_time < connect_params_.sleep_time_) {
      if (stats_.activate_timers_) {
        StopTimer(&stats_.client_awaiting_server_timer_);
      }
      // Doubled too many times (and in particular, the number of milliseconds
      // to sleep can no longer be expressed as uint64_t).
      SetErrorMessage(
          __LINE__,
          "Failed to connect client socket: Too many failures.");
      return false;
    }
  }

  // Inform user that connection failed.
  if (print_message &&
      (connect_params_.print_default_message_ ||
       !connect_params_.message_.empty())) {
    if (connect_params_.print_default_message_) {
      string time_str = "";
      if (sleep_time > 0) {
        time_str += " in ";
        if (sleep_time >= 1000) {
          time_str += Itoa(sleep_time / 1000) + " ";
        } else {
          time_str += Itoa(sleep_time) + " milli";
        }
        time_str += "seconds";
      }
      string msg = "Failed to Connect to " + connect_params_.ip_ +
          " on port " + Itoa(connect_params_.port_) + ". Retrying" +
          time_str + "...";
      log_warn("%s", msg.c_str());
    } else {
      log_warn("%s", connect_params_.message_.c_str());
    }
  }

  return sleep_time;
}

bool Socket::CheckConnectStatus(
    bool * is_connected, int * socket_error_code) {
  set<SocketIdentifier> write_sockets, error_sockets;
  const int return_value = SocketSelect(
      SocketGetClientSocketId(),
      0,
      connect_params_.timeout_ms_ * 1000,
      nullptr,
      &write_sockets,
      &error_sockets);

  if (return_value < 0) {
    // select() returned an error; return.
    const int last_error_code = SocketGetLastError();
    stats_.num_errors_seen_++;
    stats_.num_select_errors_++;
    SetErrorMessage(
        __LINE__,
        "Unexpected select() error for CheckConnectStatus(): " +
            Itoa(last_error_code));
    return false;
  } else if (return_value == 0) {
    *is_connected = false;
    return true;
  } else if (SocketIsClientSocketInSet(error_sockets)) {
    *is_connected = false;
    *socket_error_code = SocketGetClientSocketError();
    return true;
  } else if (SocketIsClientSocketInSet(write_sockets)) {
    *socket_error_code = SocketGetClientSocketError();
    *is_connected = *socket_error_code == 0;
    return true;
  }

  // Should never reach here: select() returned an event, but none of
  // the expected ones...
  SetErrorMessage(
      __LINE__, "Unexpected select() response in CheckConnectStatus()");
  return false;
}

// Non-blocking connect() will *never* complete successfully, and will
// always return error 'SocketSocketWouldBlockCode()'. Then, even though the thread
// has returned (since it is non-blocking), the connect() request is still
// pending/processing. There are two options:
//   1) Wait for the connect() request to complete (either successfully or not),
//      and then proceed.
//   2) Abort the connect() request, and start over.
// NOTE 1: In terms of (1), there are 4 ways to detect when the connect()
// request has completed (successfully or not):
//   a) Have the client socket call select(), and detect when the connection
//      is writable
//   b) Use WSAAsyncSelect, which will deliver 'FD_CONNECT'
//   c) Use WSAEventSelect, which will signal completion to the associated object
//   d) Keep calling connect(). It will return error 'WSAEALREADY' until
//      connect() completes, at which point it returns error 'WSAEISCONN'
// Note that (d) is NOT recommended by Windows connect() documentation.
// NOTE 2: In terms of (2), this requires closing the existing Client Socket,
// and starting over. Note that this doesn't kill the original connect()
// request, so the Server may still get that, and process it; so there may
// be some clean-up/maintenance required on Server's end to removed aborted
// connect() requests.
// TODO(paul): Implement Server clean-up for aborted connect()s, if necessary.
// NOTE 3: The whole reason I begain pursuing non-blocking connect() was
// because when Client attempts to connect() before Server is listening (via
// select()) for connection requests, there is a lag time of ~0.5s before
// the connection succeeds; i.e. this appears to be a timeout for retrying
// failed connect() attempts (when there is no listening Server); thus, even if
// the Server appears immediately after (or simultaneously with) the connect()
// request, we have to wait 0.5s before actually connecting. Contrast this to
// the case that the Server is listening (via select()) *before* the connect()
// call, in which case connect() successfully completes in microseconds.
// In particular, pursuing (1) above doesn't solve the 0.5s issue: if the
// connect() request is going to fail, then even in non-blocking mode, the
// system is going to wait the 0.5s before retrying.
// CONCLUSION:
// Pursue (1) (via (a)) for a very brief period, to make give the connect()
// request time to return successfully (assuming Server is listening).
// If connect() has not returned successfully by this time, we do (2)
// and abort and forcefully retry connect (on a new socket), so that
// we don't incur the 0.5s delay.
bool Socket::HandleNonBlockingConnect(const int last_error_code) {
  if (last_error_code != SocketSocketWouldBlockCode() &&
      last_error_code != SocketSocketOpAlreadyCode() &&
      last_error_code != SocketSocketInProgressCode()) {
    if (stats_.activate_timers_) {
      StopTimer(&stats_.client_awaiting_server_timer_);
    }
    SetErrorMessage(
        __LINE__,
        "Failed to (non-blocking) connect client socket: " +
            Itoa(last_error_code));
    return false;
  }

  int64_t sleep_time = ComputeConnectSleepTime(
      stats_.num_failed_connect_sleep_calls_ > 0);
  if (sleep_time > 0) {
    // Sleep.
    // WARNING: Sleep time depends on system clock; it is only guaranteed to be
    // at *least* the argument specified. For example, on Windows, this appears
    // to be around 15ms, so having 'sleep_time' anything less than this will
    // likely sleep up to 15ms anyway.
    long long int pre_sleep_time = GetMicroTime();
    SocketSleep(sleep_time * 1000);
    long long int actual_sleep_time = GetMicroTime() - pre_sleep_time;
    stats_.msec_waiting_to_connect_ += actual_sleep_time;
    stats_.failed_connect_sleep_timer_ += actual_sleep_time;
    stats_.msec_waiting_to_connect_last_ = actual_sleep_time;
  }

  // Check if connect() is complete by testing readiness to write via select().
  bool is_connected = false;
  // TODO(paul): Determine if there is a use-case where socket_error_code is useful.
  // For example, I often see error code 10061 (WSAECONNREFUSED) after 1s has
  // elapsed with no response from the server. I haven't seen any other errors;
  // so perhaps handle 10061 appropriately (e.g. do nothing), and abort for any
  // other error?
  int socket_error_code = 0;
  if (!CheckConnectStatus(&is_connected, &socket_error_code)) {
    if (stats_.activate_timers_) {
      StopTimer(&stats_.client_awaiting_server_timer_);
    }
    SetErrorMessage(
        __LINE__, "Failed to (non-blocking) connect client socket.");
    return false;
  }

  if (stats_.activate_timers_) {
    StopTimer(&stats_.client_awaiting_server_timer_);
    stats_.connect_last_timer_ = GetMicroTime() -
        (stats_.time_at_last_failed_connect_call_ > 0 ?
             stats_.time_at_last_failed_connect_call_ :
             stats_.last_connect_call_start_);
  }

  if (is_connected) {
    // Add the new connection to the set of connections_.
    connections_.push_back(SocketGetClientSocketId());

    // Refresh counter.
    stats_.msec_waiting_to_connect_ = 0;
    return true;
  }

  // Not connected. This likely means Server is not yet listening for new connections.
  // Retry connecting.
  ++stats_.num_failed_connect_sleep_calls_;
  if (connect_params_.sleep_time_ >= 0) {
    if (stats_.activate_timers_) {
      stats_.time_at_last_failed_connect_call_ = GetMicroTime();
    }
    // For Linux, we scrap the client_socket_ and start over with
    // a fresh one; whereas for Windows, we resuse the original
    // socket (and in particular, the same Client-side port).
    // We distinguish these cases based on the return value
    // of select(), which for Linus will be 111 = ECONNREFUSED.
    if (socket_error_code == SocketSocketConnectionRefusedCode()) {
      if (!CloseClientSocket()) {
        SetErrorMessage(__LINE__, "Failed to Connect()");
        return false;
      }
    }

    return Connect();
  }

  return false;
}

bool Socket::HandleConnectError(const int last_error_code) {
  stats_.num_connection_errors_++;

  // Check that we haven't exceeded allowable time for Connect().
  if (connect_params_.timeout_ms_ > 0 &&
      stats_.msec_waiting_to_connect_ >= connect_params_.timeout_ms_) {
    SetErrorMessage(
        __LINE__,
        "Connect() timeout (" + Itoa(connect_params_.timeout_ms_) +
            " ms) exceeded.");
    return false;
  }

  // If connect_params_ indicates to retry after failed connections, sleep
  // and then try again.
  const int64_t sleep_time =
      last_error_code == SocketSocketConnectionRefusedCode() ?
      ComputeConnectSleepTime(true) :
      -1;
  if (sleep_time >= 0) {
    ++stats_.num_failed_connect_sleep_calls_;
    if (sleep_time > 0) {
      // Sleep.
      // WARNING: Sleep time depends on system clock; it is only guaranteed to be
      // at *least* the argument specified. For example, on Windows, this appears
      // to be around 15ms, so having 'sleep_time' anything less than this will
      // likely sleep up to 15ms anyway.
      long long int pre_sleep_time = GetMicroTime();
      SocketSleep(sleep_time * 1000);
      long long int actual_sleep_time = GetMicroTime() - pre_sleep_time;
      stats_.msec_waiting_to_connect_ += actual_sleep_time;
      stats_.failed_connect_sleep_timer_ += actual_sleep_time;
      stats_.msec_waiting_to_connect_last_ = actual_sleep_time;
    }

    if (stats_.activate_timers_) {
      StopTimer(&stats_.client_awaiting_server_timer_);
    }

    return Connect();
  }

  // The fact that we reached here means failed connect attempts shouldn't
  // sleep and retry. Return false.
  if (stats_.activate_timers_) {
    StopTimer(&stats_.client_awaiting_server_timer_);
  }
  SetErrorMessage(
      __LINE__,
      "Failed to connect client socket after " +
          Itoa(stats_.num_connection_errors_) +
          " attempts: " + Itoa(last_error_code));
  return false;
}

// DISCUSSION:
// Connect (for Client) and Listen (for connection requests for Server) can be
// done in a Blocking or Non-Blocking manner:
//   - For non-blocking Connect/Listen, the code would then use a subsequent
//     call to select(), with a specified timeout time required for select(),
//     and select() will indicate:
//       - To Server: That a connection request has been made (by a Client)
//       - To Client: That a connection request has been accepted (by Server)
//   - For blocking calls (to Connect or the subsequent call to select()),
//     a timeout should be set to determine how long to block before
//     returning with failure.
// Probably, the cleanest implementation is just to use blocking calls everywhere.
// However, the current code doesn't do this because the blocking calls seemed
// to add up to a 0.5s delay to the Server's Listen/Select code [although it is
// possible that this delay was an artificial product of the fact of running
// both the Client and Server on the same machine, and thus of competing
// resources/threads, etc.]. Furthermore, this delay was present even if the
// Client's Connect() request occurred *before* the Server called Listen/Select.
// Note that this delay was only encountered on the Server's select() call
// (i.e. if going for non-blocking option for Connect, then the Client's
// select() call to detect the connection never seemed to cause a lag), and it
// was only encountered when the Client was started first (and so the original
// connect() call was made before Server was listening). Furthermore, the delay
// was consistently witnessed (when Client started first), with delay time
// always in [0, 0.5] seconds, and on average around 0.25s.
//
// I tried to get around the 0.5s delay by playing with different options
// for Connect(), Listen(), and Select() (for both Client and Server),
// in terms of blocking vs. non-blocking and also the timeouts.
// However, none of the settings got rid of the delay, and while some
// appeared to reduce the delay more than others, it is really hard to know if
// there is any statistically significant difference (since the delay time
// variance between consecutive runs is high).
//
// To control blocking vs. non-blocking for options:
//   - (Server) Listen: listen_params_.select_timeout_ms_ controls
//                      blocking vs. non-blocking, where a non-zero select
//                      timeout indicates that Listen() should return right
//                      away (non-blocking), and then the select timeout
//                      is as specified. Meanwhile, a zero value here
//                      indicates to block on Listen().
//   - Connect: connect_params_.non_blocking_ controls blocking vs. non-blocking,
//              and in the case of non-blocking, connect_params_.timeout_ms_
//              controls the subsequent select() timeout.
// Default settings are to do non-blocking for Server Listening for new
// connections (with 100ms for select() timeout) and a blocking call for Connect.
// But again, I'm not sure these defaults are any better than just blocking for
// both Server Listen and Client Connect, or non-blocking for both, or any
// other possible combination...
bool Socket::Connect() {
  if (stats_.activate_timers_) {
    const long long int current_time = GetMicroTime();
    stats_.time_from_last_connect_call_ =
        current_time - stats_.last_connect_call_start_;
    stats_.time_from_last_failed_connect_call_ =
        current_time - stats_.time_at_last_failed_connect_call_;
    stats_.last_connect_call_start_ = current_time;
    StartTimer(&stats_.client_awaiting_server_timer_);
  }

  // Make sure this instance's role is *not* a server (since servers should
  // never initiate a new connection request).
  if (role_ != SocketRole::CLIENT) {
    SetErrorMessage(
        __LINE__,
        "Failed to Connect(): Servers may not initiate new connection "
        "requests.");
    return false;
  }

  // Make sure Client Socket is initialized.
  if (!is_socket_initialized_ && !SetupSocket()) {
    SetErrorMessage(
        __LINE__, "Failed to Connect(): Unable to SetupSocket.");
    return false;
  }

  // Make sure IP and Port have been set.
  if (connect_params_.ip_.empty() || connect_params_.port_ == 0) {
    SetErrorMessage(
        __LINE__,
        "Failed to Connect() on malformed ip '" + connect_params_.ip_ +
            "' and port " + Itoa(connect_params_.port_));
    return false;
  }

  // Set Socket TCP_NODELAY option.
  if ((!connect_params_.non_blocking_ || !client_socket_is_bound_) &&
      !SocketSetSocketOption(
          SocketGetClientSocketId(),
          SocketGetSocketTcpCode(),
          SocketGetSocketTcpNoDelayCode())) {
    SetErrorMessage(
        __LINE__,
        "Unable to set client socket TCP_NODELAY setting: " +
            Itoa(SocketGetLastError()));
    if (stats_.activate_timers_) {
      StopTimer(&stats_.client_awaiting_server_timer_);
    }
    return false;
  }

  // Set whether the connect() call by Client Socket is blocking or not.
  if (connect_params_.non_blocking_) {
    // Mark the new connection as non-blocking.
    if (!SocketSetNonBlockingSocket(true, SocketGetClientSocketId())) {
      SetErrorMessage(
          __LINE__,
          "Failed to set Client Socket to non-blocking: " +
              Itoa(SocketGetLastError()));
      if (stats_.activate_timers_) {
        StopTimer(&stats_.client_awaiting_server_timer_);
      }
      return false;
    }
    // Blocking Connect() calls go through a series of (non-blocking) connect()
    // calls, followed by calls to select() to determine if the connection has
    // been established. Depending on the connect_params_.select_timeout_ms_,
    // that call to select() can itself be blocking or non-blocking. In the
    // non-blocking case (and sometimes even for the blocking case, if select()
    // returns an "error" event, which seems to happen on Windows when Server
    // still hasn't responded within 1s of the original connect() request), the
    // code will attempt to call connect() again. What happens next depends on the OS:
    //   - For Linux: select() will report error ECONNREFUSED. Then we should
    //     go ahead and close the socket and start over (the Server will
    //     never see the original/aborted connection attempt).
    //   - For Windows: In order to avoid getting a bunch of connections on different
    //     (Client-side) ports (in particular, this is necessary to avoid having
    //     the Server think he has lots of connections to the Client's IP, on
    //     many different ports) we need to reuse the same (client-side) port
    //     every time. Since shutdown/close socket may not have cleaned up the
    //     port by the time we try to reopen it, we need to set the following option.
    // Thus, in either case we set the following socket option; this is
    // necessary for Windows, and it is essentially a no-opt for Linux,
    // since Linux will destroy and recreate the socket on every failed
    // connect() call anyway.
    if (!SocketSetClientSocketOption(
            SocketGetSocketLayerCode(),
            SocketGetSocketReusePortCode())) {
      SetErrorMessage(
          __LINE__,
          "Failed to SetClientSocket() Socket: "
          "Unable to set server socket SO_REUSEADDR setting: " +
              Itoa(SocketGetLastError()));
      if (stats_.activate_timers_) {
        StopTimer(&stats_.client_awaiting_server_timer_);
      }
      return false;
    }
  }

  ++stats_.num_connect_calls_;
  if (stats_.activate_timers_) {
    StartTimer(&stats_.connect_timer_);
  }
  // Connects to connect_params_.[ip, port]_
  if (!SocketClientConnect()) {
    if (stats_.activate_timers_) {
      StopTimer(&stats_.connect_timer_);
    }
    const int last_error_code = SocketGetLastError();
    if (connect_params_.non_blocking_) {
      return HandleNonBlockingConnect(last_error_code);
    }
    return HandleConnectError(last_error_code);
  }
  if (stats_.activate_timers_) {
    StopTimer(&stats_.client_awaiting_server_timer_);
    StopTimer(&stats_.connect_timer_);
    stats_.connect_last_timer_ = GetMicroTime() -
        (stats_.time_at_last_failed_connect_call_ > 0 ?
             stats_.time_at_last_failed_connect_call_ :
             stats_.last_connect_call_start_);
  }

  // Add the new connection to the set of connections_.
  connections_.push_back(SocketGetClientSocketId());
  stats_.msec_waiting_to_connect_ = 0;

  return true;
}

bool Socket::ServerConnect() {
  // Listen() for connection request, then return immediately, so that
  // we can return to calling code (typically, ServerConnect() is called
  // when the Server is supposed to *send* data to a Client, and hence
  // we only want the Server to Listen() for the new connection request,
  // and then stop listen()ing).
  stats_.abort_listening_with_code_.clear();
  const ListenParams orig_params = listen_params_;
  ListenParams tmp = orig_params;
  tmp.type_.clear();
  tmp.stop_params_.Reset(1);
  listen_params_ = tmp;
  const set<ListenReturnCode> return_codes = Listen();
  if (return_codes.size() != 1 ||
      *(return_codes.begin()) != ListenReturnCode::NEW_CONNECTION) {
    string return_code_str =
        "Listen Return Codes (" + Itoa(return_codes.size()) + "):";
    for (const ListenReturnCode & code_i : return_codes) {
      return_code_str += " " + GetListenReturnCodeString(code_i);
    }

    SetErrorMessage(
        __LINE__,
        "Unable to connect to Client: " + return_code_str +
            "\n.Socket Error Message:\n" + GetErrorMessage());
    return false;
  }
  stats_.abort_listening_with_code_.clear();

  // Connect request received. Handle it.
  // First, reset original listen params, which specified how to handle connection requests.
  listen_params_ = orig_params;
  return HandleNewConnection();
}

bool Socket::IsConnected(const SocketIdentifier & socket) const {
  for (const SocketIdentifier & existing : connections_) {
    if (socket.Equals(existing))
      return true;
  }
  return false;
}

bool Socket::HandleNewConnection() {
  // Determine if (this) socket should handle the New Connection Event.
  if (listen_params_.type_.find(ListenType::HANDLE_NEW_CONNECTIONS) ==
          listen_params_.type_.end() &&
      listen_params_.type_.find(ListenType::HANDLE_ALL_EVENTS) ==
          listen_params_.type_.end()) {
    // New Connections should be handled by caller. Return control.
    stats_.abort_listening_with_code_.insert(
        ListenReturnCode::NEW_CONNECTION);
    return false;
    // Handle New Connection event via provided callback, if appropriate.
  } else if (listen_params_.handle_new_connection_ != nullptr) {
    // Caller provided a function to handle new connections. Call it.
    if (!(*listen_params_.handle_new_connection_)(
            SocketGetServerSocketId())) {
      SetErrorMessage(
          __LINE__,
          "Failed to create connection via user-provided function in "
          "listen_params_.handle_new_connection_.");
      stats_.num_errors_seen_++;
      stats_.num_connection_errors_++;
      return false;
    }
    // Use default mechanism to handle New Connection.
  } else if (!OpenNewConnection()) {
    SetErrorMessage(
        __LINE__,
        "Failed to create connection socket via accept(): " +
            Itoa(SocketGetLastError()));
    stats_.num_errors_seen_++;
    stats_.num_connection_errors_++;
    return false;
  }

  // Update stats.
  ++stats_.num_distinct_connections_seen_;
  ++stats_.num_current_connections_;
  stats_.num_peak_connections_ = max(
      stats_.num_peak_connections_, stats_.num_current_connections_);

  return true;
}

bool Socket::OpenNewConnection() {
  ++stats_.num_open_new_connection_calls_;
  if (stats_.activate_timers_) {
    StartTimer(&stats_.new_connection_timer_);
    if (connections_.empty())
      StopTimer(&stats_.server_awaiting_client_timer_);
  }

  SocketIdentifier new_connection;
  if (!SocketOpenNewConnection(&new_connection)) {
    SetErrorMessage(
        __LINE__,
        "Failed to OpenNewConnection(): accept() failed. "
        "Server is bound and accepting connection requests: " +
            Itoa((
                int)server_socket_is_bound_and_accepting_connection_requests_));
    return false;
  }
  if (stats_.activate_timers_) {
    StopTimer(&stats_.new_connection_timer_);
  }

  // Set the TCP_NODELAY option.
  if (!SocketSetSocketOption(
          new_connection,
          SocketGetSocketTcpCode(),
          SocketGetSocketTcpNoDelayCode())) {
    SetErrorMessage(
        __LINE__,
        "Failed to set socket options on new connection: "
        "Unable to set server socket TCP_NODELAY setting: " +
            Itoa(SocketGetLastError()));
    return false;
  }

  // Mark the new connection as non-blocking.
  if (!SocketSetNonBlockingSocket(true, new_connection)) {
    SetErrorMessage(
        __LINE__,
        "Failed to set Server's new connection (socket) to "
        "non-blocking: " +
            Itoa(SocketGetLastError()));
    return false;
  }

  // Add the new connection to the set of connections_.
  connections_.push_back(new_connection);

  if (connections_.size() > 63) {
    // TODO(paul): Determine what to do here...
    log_warn("More than 63 client connections accepted.  "
             "This will not work reliably on some Winsock stacks!");
  }

  return true;
}

bool Socket::HandleSelectEvents(
    set<SocketIdentifier> * read_sockets,
    set<SocketIdentifier> * error_sockets) {
  bool to_return = true;

  // Special Handling for CookieSockets, which always have empty connections_.
  if (socket_type_ == SocketType::COOKIE) {
    if (error_sockets->find(SocketGetReceiveSocketId()) !=
        error_sockets->end()) {
      error_sockets->erase(SocketGetReceiveSocketId());
      stats_.num_event_errors_++;
      to_return = false;
    }
    if (error_sockets->find(SocketGetSendSocketId()) !=
        error_sockets->end()) {
      error_sockets->erase(SocketGetSendSocketId());
      stats_.num_event_errors_++;
      to_return = false;
    }
    if (read_sockets->find(SocketGetReceiveSocketId()) !=
        read_sockets->end()) {
      read_sockets->erase(SocketGetReceiveSocketId());
      if (!ReceiveData(SocketGetReceiveSocketId())) {
        // An unrecoverable receive error was encountered. Close connection.
        stats_.num_errors_seen_++;
        stats_.num_receive_errors_++;
        to_return = false;
      }
    }
    if (read_sockets->find(SocketGetSendSocketId()) !=
        read_sockets->end()) {
      log_error("CookieSocket should not be receiving anything along "
                "its 'send' connection.");
      read_sockets->erase(SocketGetSendSocketId());
      stats_.num_event_errors_++;
      to_return = false;
    }
    return to_return;
  }

  vector<SocketIdentifier>::iterator itr = connections_.begin();
  while (itr != connections_.end()) {
    // See if this socket's flag is set in any of the socket lists.
    //   - This socket has an Error.
    if (error_sockets->find(*itr) != error_sockets->end()) {
      error_sockets->erase(*itr);
      if (!HandleGeneralSocketError(itr)) {
        stats_.num_event_errors_++;
        to_return = false;
        continue;
      }
      //   - This socket has no pending activity (send nor receive requests).
    } else if (read_sockets->find(*itr) == read_sockets->end()) {
      // Nothing to do.
      //   - This socket has a Send and/or Receive request.
    } else {
      // Handle 'Receive' Event.
      read_sockets->erase(*itr);
      if (!ReceiveData(*itr)) {
        // An unrecoverable receive error was encountered. Close connection.
        RemoveConnection(itr);
        stats_.num_errors_seen_++;
        stats_.num_receive_errors_++;
        to_return = false;
        continue;
      }
    }

    // Go on to next connection.
    ++itr;
  }

  return to_return;
}

bool Socket::ReceiveData(const SocketIdentifier & s) {
  ++stats_.num_server_receive_data_calls_;
  if (stats_.activate_timers_) {
    StartTimer(&stats_.server_receive_data_timer_);
  }
  // Create a buffer to read bytes into, if not already present.
  ReceivedData * received_data =
      FindOrInsert(s, received_bytes_per_socket_, ReceivedData());

  // Sanity-Check there are not multiple threads trying to receive on
  // this socket at the same time.
  if (received_data->is_receiving_data_) {
    SetErrorMessage(
        __LINE__, "ReceiveData called, even though already receiving.");
    if (stats_.activate_timers_) {
      StopTimer(&stats_.server_receive_data_timer_);
    }
    return false;
  }
  received_data->is_receiving_data_ = true;

  // Resize buffer to have room for the new data.
  const int max_bytes_to_receive =
      listen_params_.receive_buffer_max_size_;
  const int pre_receive_size = received_data->buffer_.size();
  if (pre_receive_size >= kMaxBytesTransferrable) {
    received_data->is_receiving_data_ = false;
    SetErrorMessage(__LINE__, "Receive Buffer is full.");
    if (stats_.activate_timers_) {
      StopTimer(&stats_.server_receive_data_timer_);
    }
    return false;
  }
  try {
    received_data->buffer_.resize(
        max_bytes_to_receive + received_data->buffer_.size());
  } catch (const length_error & e) {
    received_data->is_receiving_data_ = false;
    SetErrorMessage(
        __LINE__,
        "Failed to allocate " +
            Itoa(max_bytes_to_receive + received_data->buffer_.size()) +
            " bytes of memory to Read in data:\n" + e.what());
    if (stats_.activate_timers_) {
      StopTimer(&stats_.server_receive_data_timer_);
    }
    return false;
  }

  int num_bytes_received = SocketReceive(
      s,
      max_bytes_to_receive,
      received_data->buffer_.data() + pre_receive_size);

  // Check for Error receiving data.
  if (num_bytes_received == SocketSocketErrorCode()) {
    received_data->buffer_.resize(pre_receive_size);
    received_data->is_receiving_data_ = false;
    if (stats_.activate_timers_) {
      StopTimer(&stats_.server_receive_data_timer_);
    }
    return HandleReceiveError(s);
  }

  // Resize buffer based on number of bytes received.
  received_data->buffer_.resize(pre_receive_size + num_bytes_received);
  received_data->is_receiving_data_ = false;
  stats_.num_received_bytes_ += num_bytes_received;
  stats_.num_global_received_bytes_ += num_bytes_received;
  stats_.num_received_transactions_++;
  stats_.num_global_received_transactions_++;

  if (num_bytes_received == 0) {
    // TODO(paul): Determine if anything needs to be done here.
    SetErrorMessage(__LINE__, "Zero bytes received.");
    if (stats_.activate_timers_) {
      StopTimer(&stats_.server_receive_data_timer_);
    }
    return false;
  }

  if (received_data->buffer_.back() == '\0') {
    stats_.num_received_null_characters_++;
  }

  const bool to_return =
      HandleReceivedBytes(listen_params_, s, received_data);

  // Stop Timer.
  if (stats_.activate_timers_) {
    StopTimer(&stats_.server_receive_data_timer_);
  }

  return to_return;
}

bool Socket::HandleReceivedBytes(
    const ListenParams & listen_params,
    const SocketIdentifier & s,
    ReceivedData * received_data) {
  // Actually *do* something with the data received, as per user-specifications.
  if (listen_params.type_.find(ListenType::HANDLE_RECEIVE) ==
          listen_params.type_.end() &&
      listen_params.type_.find(ListenType::HANDLE_ALL_EVENTS) ==
          listen_params.type_.end()) {
    // New Connections should be handled by caller. Return control.
    stats_.abort_listening_with_code_.insert(ListenReturnCode::RECEIVE);
    // Handle New Connection event via provided callback, if appropriate.
  } else if (listen_params.receive_data_ != nullptr) {
    // Caller provided a function to handle new connections. Call it.
    if (!(*listen_params.receive_data_)(
            s,
            received_data,
            &stats_,
            listen_params.receive_data_args_)) {
      SetErrorMessage(
          __LINE__,
          "Failed to receive data via user-provided function in "
          "listen_params.receive_data_.");
      return false;
    }
    // Use default mechanism to handle New Connection.
  } else {
    // No default behavior set. Just store data in received_data's buffer_.
  }

  return true;
}

SendReturnCode
Socket::SendData(const char * data, const size_t & size) {
  if (role_ == SocketRole::CLIENT) {
    if (!IsConnected() && !Connect()) {
      SetErrorMessage(__LINE__, "Unable to connect to Server.");
      return SendReturnCode::CANNOT_CONNECT;
    }
    return SendData(SocketGetSendSocketId(), data, size);
  } else if (role_ == SocketRole::SERVER) {
    // This API doesn't specify a socket/connection to use, which means the
    // caller expects that the Server should only have one connection, and
    // to use that one. First check that a connection is available, and if
    // not (since we are in case role_ == SERVER), wait for a connection
    // request from the CLIENT.
    if (!IsConnected() && !ServerConnect()) {
      SetErrorMessage(__LINE__, "Unable to connect to Client.");
      return SendReturnCode::CANNOT_CONNECT;
    }
    return SendData(0, data, size);
  } else if (role_ == SocketRole::NEITHER) {
    return SendData(SocketGetSendSocketId(), data, size);
  }

  // Unsupported role.
  return SendReturnCode::BAD_ROLE;
}

SendReturnCode Socket::SendData(
    const SocketIdentifier & socket,
    const char * data,
    const size_t & size) {
  // First check if a connection across 'socket' already exists.
  if (role_ == SocketRole::CLIENT) {
    if (!IsConnected() && !Connect()) {
      SetErrorMessage(__LINE__, "Unable to connect to Server.");
      return SendReturnCode::CANNOT_CONNECT;
    }
  } else if (role_ == SocketRole::SERVER) {
    // If Server is not already connected to the Client via 'socket', then
    // wait for Client to establish a connection.
    bool is_connected = false;
    while (!is_connected) {
      for (const SocketIdentifier & s : connections_) {
        is_connected |= socket.Equals(s);
      }
      if (!is_connected && !ServerConnect()) {
        SetErrorMessage(__LINE__, "Unable to connect to Client.");
        return SendReturnCode::CANNOT_CONNECT;
      }
    }
  }

  // Queue data to send (we do this instead of just calling SendDataNow()
  // directly in case there is already some stuff queued to send).
  if (!SendDataNoFlush(socket, data, size)) {
    SetErrorMessage(__LINE__, "Unable to Send data.");
    return SendReturnCode::ENQUEUE_ERROR;
  }

  // Flush.
  return FlushSendBuffer(socket);
}

SendReturnCode Socket::SendDataNow(
    const SocketIdentifier & socket,
    const char * data,
    const size_t & size) {
  int num_chars_remaining = size;
  int next_char_to_send_index = 0;
  while (num_chars_remaining > 0) {
    ++stats_.num_send_data_calls_temp_;
    int num_chars_sent = 0;
    const SendReturnCode result = SendBlob(
        socket,
        data + next_char_to_send_index,
        num_chars_remaining,
        &num_chars_sent);
    if (result != SendReturnCode::SUCCESS)
      return result;
    num_chars_remaining -= num_chars_sent;
    next_char_to_send_index += num_chars_sent;
  }

  // Update stats_, incrementing number of send() transactions.
  stats_.num_sent_transactions_++;
  stats_.num_global_sent_transactions_++;

  return SendReturnCode::SUCCESS;
}

SendReturnCode Socket::SendBlob(
    const SocketIdentifier & s,
    const char * buffer,
    const size_t & num_chars_in_buffer,
    int * num_chars_sent) {
  ++stats_.num_inside_send_data_calls_;
  if (num_chars_in_buffer == 0)
    return SendReturnCode::SUCCESS;
  if (stats_.activate_timers_) {
    StartTimer(&stats_.send_data_timer_);
  }

  // Make sure socket is initialized.
  if (!is_socket_initialized_ && !SetupSocket()) {
    SetErrorMessage(
        __LINE__, "Failed to SendBlob(): Unable to SetupSocket.");
    return SendReturnCode::BAD_SOCKET;
  }

  // Test if Socket s is ready for writing.
  // Instead of calling 'send()' directly, we make sure the Socket is
  // ready for use (e.g. that the Receiving end isn't still processing data).
  if (stats_.activate_timers_) {
    StartTimer(&stats_.send_data_select_timer_);
  }
  set<SocketIdentifier> write_sockets, error_sockets;
  const int socket_ready = SocketSelect(
      s,
      0,
      listen_params_.select_timeout_ms_ * 1000,
      nullptr,
      &write_sockets,
      &error_sockets);
  if (stats_.activate_timers_) {
    StopTimer(&stats_.send_data_select_timer_);
  }
  if (socket_ready < 0) {
    SetErrorMessage(
        __LINE__,
        "Failed to SendBlob(): select() returned error: " +
            Itoa(SocketGetLastError()));
    if (stats_.activate_timers_) {
      StopTimer(&stats_.send_data_timer_);
    }
    return SendReturnCode::SELECT_ERROR;
  } else if (socket_ready == 0) {
    // Socket not ready for writing.
    // NOTE: I should only be here if listen_params_.select_timeout_ms_ is
    // positive, indicating that the select() call is non-blocking, and that
    // the socket is not ready yet.
    if (stats_.activate_timers_) {
      StopTimer(&stats_.send_data_timer_);
    }

    if (listen_params_.select_timeout_ms_ <= 0) {
      SetErrorMessage(
          __LINE__,
          "SendBlob() hit a code path that shouldn't ever happen: "
          "select() returned 'socket not ready', even though the "
          "'blocking' version of select() was used...");
      return SendReturnCode::UNKNOWN_ERROR;
    }

    // TODO(paul) Handle this case appropriately based on send_params_.
    SetErrorMessage(
        false,
        __LINE__,
        "Failed to SendBlob(): select() indicated socket not ready "
        "to send. Last Error: " +
            Itoa(SocketGetLastError()));
    return SendReturnCode::SOCKET_NOT_READY;
  } else if (!error_sockets.empty()) {
    if (stats_.activate_timers_) {
      StopTimer(&stats_.send_data_timer_);
    }
    if (error_sockets.size() != 1 ||
        error_sockets.find(s) == error_sockets.end()) {
      SetErrorMessage(
          __LINE__,
          "Failed to SendBlob(): Error (" +
              Itoa(SocketGetSocketError(*(error_sockets.begin()))) +
              ") on unexpected socket.");
    } else {
      SetErrorMessage(
          __LINE__,
          "Failed to SendBlob(): Socket Error: " +
              Itoa(SocketGetSocketError(s)));
    }
    return SendReturnCode::SOCKET_ERROR;
  } else if (write_sockets.empty()) {
    SetErrorMessage(
        __LINE__,
        "Failed to SendBlob(): select() indicated socket "
        "was ready for writing, but then didn't populate "
        "write_sockets. "
        "This should never happen.");
    if (stats_.activate_timers_) {
      StopTimer(&stats_.send_data_timer_);
    }
    return SendReturnCode::UNKNOWN_ERROR;
  } else if (write_sockets.find(s) == write_sockets.end()) {
    // This should never happen. Even if this Client/Server has multiple
    // connections, SocketSelect() above should have only looked for changes
    // to the present socket 's'.
    SetErrorMessage(
        __LINE__,
        "Failed to SendBlob(): select() found a change to "
        "a socket (" +
            write_sockets.begin()->ToString() +
            ") that wasn't supposed to be monitored");
    if (stats_.activate_timers_) {
      StopTimer(&stats_.send_data_timer_);
    }
    return SendReturnCode::UNKNOWN_ERROR;
  } else {
    // Socket is ready for writing.
    int num_chars_sent_on_this_transmission = 0;
    const SendReturnCode result = SendBlobNow(
        s,
        buffer,
        num_chars_in_buffer,
        &num_chars_sent_on_this_transmission);
    if (result != SendReturnCode::SUCCESS) {
      SetErrorMessage(__LINE__, "SendBlobNow() failed.");
      if (stats_.activate_timers_) {
        StopTimer(&stats_.send_data_timer_);
      }
      return result;
    }
    if (num_chars_sent_on_this_transmission <= 0) {
      SetErrorMessage(
          __LINE__,
          "SendBlobNow() failed: number of bytes sent: " +
              Itoa(num_chars_sent_on_this_transmission) +
              ", last error: " + Itoa(SocketGetLastError()));
      if (stats_.activate_timers_) {
        StopTimer(&stats_.send_data_timer_);
      }
      return SendReturnCode::SEND_ERROR;
    }
    *num_chars_sent += num_chars_sent_on_this_transmission;
    // Not all of the data was sent. Try to Send remaining data.
    // TODO(paul): Determine if this is the right way to handle the case that
    // not all data was sent.
    if (num_chars_sent_on_this_transmission < num_chars_in_buffer) {
      if (stats_.activate_timers_) {
        StopTimer(&stats_.send_data_timer_);
      }
      /*
      DLOG_INFO("Only some of the data was sent: " +
                Itoa(num_chars_sent_on_this_transmission) + " of " +
                Itoa(num_chars_in_buffer) + " (in total, " +
                Itoa(*num_chars_sent) + " bytes have been sent).");
      */
      --stats_.num_inside_send_data_calls_;
      return SendBlob(
          s,
          buffer + num_chars_sent_on_this_transmission,
          (num_chars_in_buffer - num_chars_sent_on_this_transmission),
          num_chars_sent);
    }
  }

  // Stop Timer.
  if (stats_.activate_timers_) {
    StopTimer(&stats_.send_data_timer_);
  }

  return SendReturnCode::SUCCESS;
}

SendReturnCode Socket::SendBlobNow(
    const SocketIdentifier & s,
    const char * buffer,
    const size_t & num_chars_in_buffer,
    int * num_chars_sent) {
  if (stats_.activate_timers_) {
    StartTimer(&stats_.send_timer_);
  }
  *num_chars_sent = SocketSend(s, buffer, num_chars_in_buffer);
  if (stats_.activate_timers_) {
    StopTimer(&stats_.send_timer_);
  }
  if (*num_chars_sent == SocketSocketErrorCode()) {
    const int last_error = SocketGetLastError();
    // Something bad happened on the socket.  Deal with it.
    const int error_code = SocketGetSocketError(s);
    SetErrorMessage(
        __LINE__,
        "Failed to SendBlobNow() for sending " +
            Itoa(num_chars_in_buffer) + " bytes. Error Code: " +
            Itoa(error_code) + ", Last Error: " + Itoa(last_error));
    // NOTE: Now that I've split out this function, which calls send(),
    // and only called it after select() has indicated the socket is ready
    // for writing, I shouldn't ever get the WSAEWOULDBLOCK error any more.
    //return (last_error == WSAEWOULDBLOCK);
    return SendReturnCode::SEND_ERROR;
  }

  // Update stats_ with number of bytes sent.
  stats_.num_sent_bytes_ += *num_chars_sent;
  stats_.num_global_sent_bytes_ += *num_chars_sent;

  return SendReturnCode::SUCCESS;
}

bool Socket::SendDataNoFlush(
    const SocketIdentifier & socket_info,
    const char * data,
    const size_t & size) {
  if (size == 0)
    return true;

  // First check if a connection across 'socket' already exists.
  if (role_ == SocketRole::CLIENT) {
    if (!IsConnected() && !Connect()) {
      SetErrorMessage(__LINE__, "Unable to connect to Server.");
      return false;
    }
  } else if (role_ == SocketRole::SERVER) {
    // If Server is not already connected to the Client via 'socket', then
    // wait for Client to establish a connection.
    bool is_connected = false;
    while (!is_connected) {
      for (const SocketIdentifier & s : connections_) {
        is_connected |= socket_info.Equals(s);
      }
      if (!is_connected && !ServerConnect()) {
        SetErrorMessage(__LINE__, "Unable to connect to Client.");
        return false;
      }
    }
  }

  // Now lookup Socket's send buffer.
  DataToSend * to_send = FindOrInsert(
      socket_info, bytes_to_send_per_socket_, DataToSend());
  const size_t orig_size = to_send->buffer_.size();

  // Append bytes to Socket's send buffer.
  to_send->buffer_.resize(orig_size + size);
  memcpy(to_send->buffer_.data() + orig_size, data, size);

  return true;
}
bool Socket::SendDataNoFlush(
    const SocketIdentifier & socket_info, const vector<char> & data) {
  return SendDataNoFlush(socket_info, data.data(), data.size());
}
bool Socket::SendDataNoFlush(
    const SocketIdentifier & socket_info,
    const vector<unsigned char> & data) {
  return SendDataNoFlush(socket_info, data.data(), data.size());
}
bool Socket::SendDataNoFlush(
    const SocketIdentifier & socket_info,
    const unsigned char * data,
    const size_t & size) {
  return SendDataNoFlush(socket_info, (const char *)data, size);
}
bool Socket::SendDataNoFlush(const char * data, const size_t & size) {
  if (role_ == SocketRole::CLIENT) {
    if (!IsConnected() && !Connect()) {
      SetErrorMessage(__LINE__, "Unable to connect to Server.");
      return false;
    }
    return SendDataNoFlush(SocketGetSendSocketId(), data, size);
  } else if (role_ == SocketRole::SERVER) {
    // This API doesn't specify a socket/connection to use, which means the
    // caller expects that the Server should only have one connection, and
    // to use that one. First check that a connection is available, and if
    // not (since we are in case role_ == SERVER), wait for a connection
    // request from the CLIENT.
    if (!IsConnected() && !ServerConnect()) {
      SetErrorMessage(__LINE__, "Unable to connect to Client.");
      return false;
    }
    return SendDataNoFlush(0, data, size);
  } else if (role_ == SocketRole::NEITHER) {
    return SendDataNoFlush(SocketGetSendSocketId(), data, size);
  }

  // Unsupported role.
  return false;
}
bool Socket::SendDataNoFlush(
    const unsigned char * data, const size_t & size) {
  return SendDataNoFlush((const char *)data, size);
}
bool Socket::SendDataNoFlush(const vector<char> & data) {
  return SendDataNoFlush(data.data(), data.size());
}
bool Socket::SendDataNoFlush(const vector<unsigned char> & data) {
  return SendDataNoFlush(data.data(), data.size());
}

SendReturnCode Socket::FlushSendBuffer() {
  if (role_ == SocketRole::CLIENT) {
    return FlushSendBuffer(SocketGetSendSocketId());
  } else if (role_ == SocketRole::SERVER) {
    if (connections_.empty())
      return SendReturnCode::SUCCESS;
    return FlushSendBuffer(0);
  } else if (role_ == SocketRole::NEITHER) {
    return FlushSendBuffer(SocketGetSendSocketId());
  }

  // Unsupported role.
  return SendReturnCode::BAD_ROLE;
}

SendReturnCode
Socket::FlushSendBuffer(const SocketIdentifier & socket_info) {
  DataToSend * to_flush =
      FindOrNull(socket_info, bytes_to_send_per_socket_);
  if (to_flush == nullptr)
    return SendReturnCode::UNKNOWN_SOCKET;
  return FlushSendBuffer(socket_info, to_flush);
}

SendReturnCode Socket::FlushSendBuffer(const int connection_index) {
  if (connection_index < 0 || connections_.size() <= connection_index) {
    return SendReturnCode::UNKNOWN_SOCKET;
  }
  return FlushSendBuffer(connections_[connection_index]);
}

SendReturnCode Socket::FlushSendBuffer(
    const SocketIdentifier & socket_info, DataToSend * to_flush) {
  to_flush->is_sending_data_ = true;
  const SendReturnCode result =
      FlushSendBuffer(socket_info, to_flush->buffer_);
  if (result != SendReturnCode::SUCCESS) {
    to_flush->is_sending_data_ = false;
    return result;
  }
  to_flush->buffer_.clear();
  to_flush->num_chars_sent_ = 0;
  to_flush->is_sending_data_ = false;
  return SendReturnCode::SUCCESS;
}

SendReturnCode Socket::FlushSendBuffers() {
  if (role_ == SocketRole::CLIENT) {
    DataToSend * to_flush =
        FindOrNull(SocketGetSendSocketId(), bytes_to_send_per_socket_);
    if (to_flush == nullptr)
      return SendReturnCode::UNKNOWN_SOCKET;
    return FlushSendBuffer(SocketGetSendSocketId(), to_flush);
  } else if (role_ == SocketRole::SERVER) {
    // If no current connections are available, create a new one.
    if (connections_.empty())
      return SendReturnCode::SUCCESS;
    for (map<SocketIdentifier, DataToSend>::iterator itr =
             bytes_to_send_per_socket_.begin();
         itr != bytes_to_send_per_socket_.end();
         ++itr) {
      if (itr->second.buffer_.empty())
        continue;
      const SendReturnCode result =
          FlushSendBuffer(itr->first, &(itr->second));
      if (result != SendReturnCode::SUCCESS) {
        return result;
      }
    }
    return SendReturnCode::SUCCESS;
  } else if (role_ == SocketRole::NEITHER) {
    DataToSend * to_flush =
        FindOrNull(SocketGetSendSocketId(), bytes_to_send_per_socket_);
    if (to_flush == nullptr)
      return SendReturnCode::UNKNOWN_SOCKET;
    return FlushSendBuffer(SocketGetSendSocketId(), to_flush);
  }

  // Unsupported role.
  return SendReturnCode::BAD_ROLE;
}

SendReturnCode Socket::FlushSendBuffer(
    const SocketIdentifier & socket_info,
    const vector<char> & to_flush) {
  if (to_flush.empty())
    return SendReturnCode::SUCCESS;
  return SendDataNow(socket_info, to_flush.data(), to_flush.size());
}

bool Socket::HandleGeneralSocketError(
    vector<SocketIdentifier>::iterator & itr) {
  stats_.num_errors_seen_++;

  // Remove the connection.
  if (!RemoveConnection(itr))
    return false;

  // TODO(paul): Determine if anything should be done here.
  return true;
}

set<ListenReturnCode> Socket::HandleBindError() {
  const int last_error_code = SocketGetLastError();
  stats_.num_bind_errors_++;

  // If listen_params_.bind_params_ indicates to retry after failed connections,
  // sleep and then try again.
  if (last_error_code == SocketSocketIntUseCode() &&
      listen_params_.bind_params_.num_retries_ != 0 &&
      listen_params_.bind_params_.sleep_time_ >= 0 &&
      (listen_params_.bind_params_.num_retries_ < 0 ||
       listen_params_.bind_params_.num_retries_ >=
           stats_.num_connection_errors_)) {
    // Try bind()ing again immediately, if sleep time is zero.
    uint64_t sleep_time = listen_params_.bind_params_.sleep_time_;
    if (listen_params_.bind_params_.sleep_time_ > 0) {
      // Figure out the sleep time, based on listen_params_.bind_params_.
      const int num_times_to_double_sleep_time =
          listen_params_.bind_params_
                  .num_tries_at_current_sleep_time_ <= 0 ?
          0 :
          (stats_.num_connection_errors_ /
           listen_params_.bind_params_
               .num_tries_at_current_sleep_time_);
      if (num_times_to_double_sleep_time > 63) {
        // Doubled too many times (and in particular, the number of milliseconds
        // to sleep can no longer be expressed as uint64_t).
        SetErrorMessage(
            __LINE__,
            "Failed to bind server socket: " + Itoa(last_error_code));
        CloseServerSocket();
        return CreateSet(ListenReturnCode::BIND_ERROR);
      }
      sleep_time = listen_params_.bind_params_.sleep_time_ *
          ((uint64_t)1 << num_times_to_double_sleep_time);
      if (sleep_time < listen_params_.bind_params_.sleep_time_) {
        // Doubled too many times (and in particular, the number of milliseconds
        // to sleep can no longer be expressed as uint64_t).
        SetErrorMessage(
            __LINE__,
            "Failed to bind serve socket: " + Itoa(last_error_code));
        CloseServerSocket();
        return CreateSet(ListenReturnCode::BIND_ERROR);
      }
    }

    // Inform user that connection failed.
    if (listen_params_.bind_params_.print_default_message_ ||
        !listen_params_.bind_params_.message_.empty()) {
      if (listen_params_.bind_params_.print_default_message_) {
        string time_str = "";
        if (sleep_time > 0) {
          time_str += " in ";
          if (sleep_time >= 1000) {
            time_str += Itoa(sleep_time / 1000) + " ";
          } else {
            time_str += Itoa(sleep_time) + " milli";
          }
          time_str += "seconds";
        }
        string msg(
            "Failed to Bind " + listen_params_.ip_ + " on port " +
            Itoa(listen_params_.port_) + ". Retrying" + time_str +
            "...");
        log_warn("%s", msg.c_str());
      } else {
        log_warn("%s", listen_params_.bind_params_.message_.c_str());
      }
    }

    // Sleep.
    // WARNING: Sleep time depends on system clock; it is only guaranteed to be
    // at *least* the argument specified. For example, on Windows, this appears
    // to be around 15ms, so having 'sleep_time' anything less than this will
    // likely sleep up to 15ms anyway.
    ++stats_.num_bind_sleep_calls_;
    long long int pre_sleep_time = GetMicroTime();
    SocketSleep(sleep_time * 1000);
    stats_.bind_sleep_timer_ += GetMicroTime() - pre_sleep_time;

    return ThreadListen();
  }

  SetErrorMessage(
      __LINE__,
      "Failed to bind server socket: " + Itoa(last_error_code));
  CloseServerSocket();
  return CreateSet(ListenReturnCode::BIND_ERROR);
}

bool Socket::HandleSendError(const SocketIdentifier & s) {
  return HandleReceiveError(s);
}

bool Socket::HandleReceiveError(const SocketIdentifier & s) {
  stats_.num_errors_seen_++;

  // Get Error Code.
  const int last_error_code = SocketGetLastError();

  // Check socket status; in particular check if this was a WSAEWOULDBLOCK error.
  const int error_code = SocketGetSocketError(s);

  // Return true on WSAEWOULDBLOCK errors (not sure why; this is how the
  // example I copied did things...); original comment regarding these:
  //   "The WSAEWOULDBLOCK can happen after select() says a socket is readable
  //    under Win9x; it doesn't happen on WinNT/2000 or on Unix."
  if (error_code == SocketSocketWouldBlockCode() ||
      error_code == SocketSocketInProgressCode()) {
    SetErrorMessage(
        __LINE__,
        "Failed to Receive on socket " + PrintSocketId(s) +
            ": WSAEWOULDBLOCK.");
    return true;
  }

  // Handle other errors; some will cause termination of socket, others won't.
  //   - Bad socket termination by Client (e.g. client's program abruptly
  //     ended (without first closing the socket connection).
  if (last_error_code == SocketSocketConnectResetCode()) {
    SetErrorMessage(
        __LINE__,
        "Failed to Receive on socket " + PrintSocketId(s) + " (" +
            Itoa(SocketSocketConnectResetCode()) +
            "): Connection reset by peer. This can happen if the other "
            "end aborts abruptly, without first closing the socket.");
    return false;
    //   - Software caused socket/connection abort; this can happen if Server
    //     software is bad, e.g. no space allocated for receive buffer.
  } else if (last_error_code == SocketSocketConnectionAbortedCode()) {
    SetErrorMessage(
        __LINE__,
        "Failed to Receive on socket " + PrintSocketId(s) + " (" +
            Itoa(SocketSocketConnectionAbortedCode()) +
            "): Software caused connection abort. This can happen if "
            "receive buffer has not allocated any space, or other "
            "software errors.");
    return false;
  } else {
    SetErrorMessage(
        __LINE__,
        "Failed to Receive on socket " + PrintSocketId(s) + " (" +
            Itoa(last_error_code) + ").");
    return true;
  }

  return true;
}

void Socket::ResetForReceive() {
  stats_.ResetSendReceiveCounts();
  stats_.ResetErrors();
  ClearReceivedBuffer();
}

bool Socket::ResetForReceive(vector<char> & extra_bytes) {
  if (received_bytes_per_socket_.size() != 1)
    return false;
  const SocketIdentifier socket_id =
      received_bytes_per_socket_.begin()->first;
  ResetForReceive();
  ReceivedData & buffer =
      received_bytes_per_socket_
          .insert(make_pair(socket_id, ReceivedData()))
          .first->second;
  buffer.buffer_.swap(extra_bytes);
  return true;
}

// =============================== END LISTEN ==================================

} // namespace networking
