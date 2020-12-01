/**
 * Copyright (C) 2020 Stealth Software Technologies, Inc.
 *
 * Developed under IR&D
 *
*/

#include "timer_utils.h"

#include "Util/string_utils.h"

#include <ff/logging.h>

#include <chrono>
#include <climits> // For ULLONG_MAX, etc.
#include <iostream>
#include <set>
#include <string>

using namespace string_utils;
using namespace std;
using namespace std::chrono;

static uint32_t kNumSuccessfulTests = 0;
static uint32_t kNumFailedTests = 0;

namespace test_utils {

string GetElapsedTimeString(const Timer & timer) {
  return FormatTime(timer.elapsed_time_.count() / 1000);
}

string FormatTime(const int64_t & total_milliseconds) {
  if (total_milliseconds < 0) {
    return "";
  }
  if (total_milliseconds == 0)
    return "0.0 seconds";

  const int64_t total_seconds = total_milliseconds / 1000;
  const int days = total_seconds / 86400;
  const int hours = (total_seconds - (days * 86400)) / 3600;
  const int minutes =
      (total_seconds - (days * 86400) - (hours * 3600)) / 60;
  const int seconds =
      total_seconds - (days * 86400) - (hours * 3600) - (minutes * 60);
  const int milliseconds = total_milliseconds % 1000;

  if (days < 0 || hours < 0 || hours >= 24 || minutes < 0 ||
      minutes >= 60 || seconds < 0 || seconds >= 60) {
    return "";
  }

  string to_return = "";
  const string hours_digit_one = hours >= 10 ? Itoa(hours / 10) : "";
  const string hours_digit_two =
      hours > 0 ? Itoa(hours % 10) + ":" : "";
  to_return += hours_digit_one + hours_digit_two;
  const string minutes_digit_one =
      minutes >= 10 ? Itoa(minutes / 10) : "0";
  const string minutes_digit_two =
      minutes > 0 ? Itoa(minutes % 10) : "0";
  to_return += (minutes == 0 && hours == 0) ?
      "" :
      (minutes_digit_one + minutes_digit_two + ":");
  const string seconds_digit_one =
      seconds >= 10 ? Itoa(seconds / 10) : "0";
  const string seconds_digit_two =
      seconds > 0 ? Itoa(seconds % 10) : "0";
  to_return += seconds_digit_one + seconds_digit_two + ".";
  const string milliseconds_digit_one =
      milliseconds >= 100 ? Itoa(milliseconds / 100) : "0";
  const string milliseconds_digit_two =
      milliseconds >= 10 ? Itoa(milliseconds / 10) : "0";
  const string milliseconds_digit_three =
      milliseconds > 0 ? Itoa(milliseconds % 10) : "0";
  to_return += milliseconds_digit_one + milliseconds_digit_two +
      milliseconds_digit_three;
  return to_return;
}

bool StartTimer(Timer * timer) {
  if (timer == nullptr ||
      (timer->state_ != TimerState::TIMER_UNSTARTED &&
       timer->state_ != TimerState::TIMER_PAUSED)) {
    return false;
  }

  timer->state_ = TimerState::TIMER_RUNNING;
  timer->start_time_ = steady_clock::now();
  return true;
}

bool StopTimer(const bool stop_permanently, Timer * timer) {
  if (timer == nullptr || timer->state_ != TimerState::TIMER_RUNNING) {
    return false;
  }

  timer->stop_time_ = steady_clock::now();
  if (timer->stop_time_ < timer->start_time_) {
    return false;
  }

  timer->state_ = stop_permanently ? TimerState::TIMER_STOPPED :
                                     TimerState::TIMER_PAUSED;
  timer->elapsed_time_ += duration_cast<microseconds>(
      timer->stop_time_ - timer->start_time_);
  return true;
}

bool ResetTimer(Timer * timer) {
  if (timer == nullptr)
    return false;
  timer->state_ = TimerState::TIMER_UNSTARTED;
  timer->elapsed_time_ = microseconds::zero();
  return true;
}

void ExponentialEachNFailuresSleepTimer::Reset() {
  num_failures_ = 0;
  current_time_ = default_time_;
  total_slept_time_ = 0;
}

bool ExponentialEachNFailuresSleepTimer::GetSleepTime(uint64_t * time) {
  if (n_ <= 0 || backoff_factor_ <= 0.0) {
    log_error("Unitialized sleep timer.");
    return false;
  }
  if (num_failures_tolerated_ >= 0 &&
      num_failures_ > num_failures_tolerated_) {
    string msg(
        "Number failures (" + Itoa(num_failures_) +
        ") exceeds allowable threshold (" +
        Itoa(num_failures_tolerated_) + ")");
    log_error("%s", msg.c_str());
    return false;
  }
  *time = current_time_;
  total_slept_time_ += current_time_;
  // Update current_time_ with the value to use next.
  ++num_failures_;
  if (num_failures_ % n_ == 0) {
    if (current_time_ >= ULLONG_MAX / backoff_factor_) {
      string msg(
          "Unable to sleep more time, as current sleep time is "
          "already " +
          Itoa(current_time_) + ", and applying backoff_factor_ of " +
          Itoa(backoff_factor_) + " would exceed ULLONG_MAX (" +
          Itoa(ULLONG_MAX) + ")");
      log_error("%s", msg.c_str());
      return false;
    }
    current_time_ *= backoff_factor_;
  }
  return true;
}

} // namespace test_utils
