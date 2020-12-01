/**
 * Copyright (C) 2020 Stealth Software Technologies, Inc.
 *
 * Developed under IR&D
 *
*/

// Description: Timer.

#include <chrono> // For std::chrono:: microseconds, timepoint, system_clock, etc.
#include <set>
#include <string>

#ifndef TIMER_UTILS_H
#define TIMER_UTILS_H

namespace test_utils {

enum TimerState {
  TIMER_UNKNOWN, // Unknown timer state.
  TIMER_UNSTARTED, // Timer has not been started yet.
  TIMER_PAUSED, // Timer is currently paused (may be restarted later).
  TIMER_RUNNING, // Timer is currently running.
  TIMER_STOPPED, // Timer is done tracking time (won't ever be re-started).
};

struct Timer {
  TimerState state_;
  // Only valid if state_ is TIMER_RUNNING; in which case it
  // gives the time that the timer was (most recently) started.
  std::chrono::time_point<std::chrono::steady_clock> start_time_;
  // Only valid if state_ is TIMER_PAUSED or TIMER_STOPPED; in which case it
  // gives the time that the timer was (most recently) stopped.
  std::chrono::time_point<std::chrono::steady_clock> stop_time_;
  // Cummulative amount of time timer has been running, i.e.:
  //   \sum (stop_time - start_time)
  std::chrono::microseconds elapsed_time_;

  Timer() {
    state_ = TimerState::TIMER_UNSTARTED;
    elapsed_time_ = std::chrono::microseconds::zero();
  }
};

inline TimerState GetTimerState(const Timer & timer) {
  return timer.state_;
}

// Returns the elapsed time (in microseconds).
inline int64_t GetElapsedTime(const Timer & timer) {
  return timer.elapsed_time_.count();
}

// Returns timer.elapsed_time_ in format:
//   DD HH:MM:SS.MILISECONDS
// e.g.:
//   5 Days, 17:25:43.12345
// NOTE: Will only print out non-0 prefixes, e.g. if DD is zero, it won't
// print out "0 Days".
extern std::string GetElapsedTimeString(const Timer & timer);

// Given an elapsed time in milliseconds, returns the time in (string) format:
//   DD HH:MM:SS.MILISECONDS
extern std::string FormatTime(const int64_t & milliseconds);

// Checks that timer is in a valid state to be started: TIMER_UNSTARTED or
// TIMER_PAUSED. If so, sets the state_ of the input timer to TIMER_RUNNING,
// and sets start_time_ accordingly.
extern bool StartTimer(Timer * timer);

// Checks that timer is in a valid state to be stopped: TIMER_RUNNING. If so,
// updates elapsed_time_ accordingly, and sets stop_time_ to the current time.
// Sets state_ to TIMER_PAUSED or TIMER_STOPPED based on input 'stop_permanently'.
extern bool StopTimer(const bool stop_permanently, Timer * timer);
// Same as above, passing in default value of stop_permanently = false.
inline bool StopTimer(Timer * timer) {
  return StopTimer(false, timer);
}

// Clears elapsed_time_, and puts timer into state TIMER_UNSTARTED.
extern bool ResetTimer(Timer * timer);

// ========================== Sleep Timers with Backoff ========================
// The following (abstract) class describes the functionality any back-off timer
// must support.
struct SleepTimer {
public:
  virtual void Reset() = 0;
  // This timer doesn't know/care the units of the time returned; the caller
  // will ultimately determine how the returned value is interpretted.
  virtual bool GetSleepTime(uint64_t * time) = 0;
};

static const int kDefaultNumFailures = 10;
static const float kDefaultBackoff = 2.0;

// Exponential back-off: After each N failed attempts, update sleep time to be
// backoff_factor_ * current_time_.
struct ExponentialEachNFailuresSleepTimer : public SleepTimer {
public:
  ExponentialEachNFailuresSleepTimer() :
      ExponentialEachNFailuresSleepTimer(
          kDefaultNumFailures, kDefaultBackoff) {
  }
  ExponentialEachNFailuresSleepTimer(
      const int n, const float & backoff) {
    n_ = n;
    num_failures_ = 0;
    num_failures_tolerated_ = -1;
    backoff_factor_ = backoff;
    default_time_ = 1;
    current_time_ = 1;
    total_slept_time_ = 0;
  }
  ExponentialEachNFailuresSleepTimer(
      const int n,
      const float & backoff,
      const int num_failures_tolerated,
      const uint64_t default_time) :
      ExponentialEachNFailuresSleepTimer(n, backoff) {
    num_failures_tolerated_ = num_failures_tolerated;
    default_time_ = default_time;
    current_time_ = default_time_;
  }

  // Override parent class' virtual functions.
  void Reset();
  bool GetSleepTime(uint64_t * time);
  uint64_t GetTotalSleptTime() const {
    return total_slept_time_;
  }
  int GetNumFailures() const {
    return num_failures_;
  }

private:
  // Number of failures to tolerate before hitting the current sleep time by
  // the backoff factor.
  int n_;
  int num_failures_;
  int num_failures_tolerated_;
  float backoff_factor_;
  uint64_t default_time_;
  uint64_t current_time_;
  uint64_t total_slept_time_;
};

} // namespace test_utils

#endif
