/**
 * Copyright (C) 2016-2020 Stealth Software Technologies, Inc.
 *
 * Developed under IR&D
 */

#include <chrono> // For time
#include <cstring> // For memcpy
#include <iostream> // For cout
#include <string>

#if defined(WINDOWS) || defined(__WIN32__) || defined(__WIN64__) || \
    defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#else
#include <thread>
#endif

#include "init_utils.h"

using namespace std;

namespace {

// I want to avoid using string_utils here, so as not to have circular dependency.
// And other Int to String methods don't always work. So just do a hack here,
// which we put in anonymous namespace, so no place else uses this.
// In particular, there is no checking here that the 'digit' is actually between
// [0..9]
string DigitToString(const int digit) {
  // Do a binary search, for speed (though it probably doesn't matter...).
  if (digit > 4) {
    if (digit > 7) {
      if (digit == 9)
        return "9";
      return "8";
    } else if (digit == 7) {
      return "7";
    } else if (digit == 6) {
      return "6";
    }
    return "5";
  } else if (digit > 2) {
    if (digit == 4)
      return "4";
    return "3";
  } else if (digit == 2) {
    return "2";
  } else if (digit == 1) {
    return "1";
  } else {
    return "0";
  }
}

// This duplicates logic from timer_utils::FormatTime, but we copy-paste that
// code here to avoid circular dependency.
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
  const string hours_digit_one =
      hours >= 10 ? DigitToString(hours / 10) : "";
  const string hours_digit_two =
      hours > 0 ? DigitToString(hours % 10) + ":" : "";
  to_return += hours_digit_one + hours_digit_two;
  const string minutes_digit_one =
      minutes >= 10 ? DigitToString(minutes / 10) : "0";
  const string minutes_digit_two =
      minutes > 0 ? DigitToString(minutes % 10) : "0";
  to_return += (minutes == 0 && hours == 0) ?
      "" :
      (minutes_digit_one + minutes_digit_two + ":");
  const string seconds_digit_one =
      seconds >= 10 ? DigitToString(seconds / 10) : "0";
  const string seconds_digit_two =
      seconds > 0 ? DigitToString(seconds % 10) : "0";
  to_return += seconds_digit_one + seconds_digit_two + ".";
  const string milliseconds_digit_one =
      milliseconds >= 100 ? DigitToString(milliseconds / 100) : "0";
  const string milliseconds_digit_two =
      milliseconds >= 10 ? DigitToString(milliseconds / 10) : "0";
  const string milliseconds_digit_three =
      milliseconds > 0 ? DigitToString(milliseconds % 10) : "0";
  to_return += milliseconds_digit_one + milliseconds_digit_two +
      milliseconds_digit_three;
  return to_return;
}

} // namespace

SYSTEM_ENDIAN endianness = SYSTEM_ENDIAN::UNKNOWN;
int kVerbosityLevel = 1;
int kNumCores = 1;
bool kUseLogColors = true;
long long int kProgramStartTimeMs = 0;
long long int kProgramStartTimeUs = 0;

void SetNumCores() {
#if defined(WINDOWS) || defined(__WIN32__) || defined(__WIN64__) || \
    defined(_WIN32) || defined(_WIN64)
  SYSTEM_INFO sys_info;
  GetSystemInfo(&sys_info);
  kNumCores = sys_info.dwNumberOfProcessors;
#else
  kNumCores = thread::hardware_concurrency();
#endif
}

SYSTEM_ENDIAN GetEndianness() {
  static bool system_evaluated_yet = false;
  if (!system_evaluated_yet) {
    system_evaluated_yet = true;

    // Test whether system is big Endian or Little Endian based on how
    // it expresses a uint32_t value.
    const uint32_t one = 1;
    unsigned char one_as_char_vector[sizeof(uint32_t)];
    memcpy(one_as_char_vector, (unsigned char *)&one, sizeof(uint32_t));
    if ((unsigned short)one_as_char_vector[0] == 1) {
      endianness = SYSTEM_ENDIAN::LITTLE;
    } else if (
        (unsigned short)one_as_char_vector[sizeof(uint32_t) - 1] == 1) {
      endianness = SYSTEM_ENDIAN::BIG;
    } else {
      cout << "ERROR Determining System Endianness." << endl;
    }
  }

  return endianness;
}

int GetVerbosity() {
  return kVerbosityLevel;
}

void SetVerbosity(const int level) {
  kVerbosityLevel = level;
}

int GetNumCores() {
  return kNumCores;
}

void SetUseLogColors(const bool use_colors) {
  kUseLogColors = use_colors;
}

bool GetUseLogColors() {
  return kUseLogColors;
}

string GetDateAndTime() {
  time_t current_time = time(nullptr);
  return asctime(localtime(&current_time));
}

string GetTime() {
  time_t current_time = time(nullptr);
  const tm * local_time = localtime(&current_time);
  const string hour = local_time->tm_hour < 10 ?
      "0" + DigitToString(local_time->tm_hour) :
      DigitToString(local_time->tm_hour / 10) +
          DigitToString(local_time->tm_hour % 10);
  const string minute = local_time->tm_min < 10 ?
      "0" + DigitToString(local_time->tm_min) :
      DigitToString(local_time->tm_min / 10) +
          DigitToString(local_time->tm_min % 10);
  const string second = local_time->tm_sec < 10 ?
      "0" + DigitToString(local_time->tm_sec) :
      DigitToString(local_time->tm_sec / 10) +
          DigitToString(local_time->tm_sec % 10);
  return hour + ":" + minute + ":" + second;
}

string GetMsTimeStr() {
  chrono::system_clock::time_point now = chrono::system_clock::now();
  chrono::milliseconds time_since_epoch =
      chrono::duration_cast<chrono::milliseconds>(
          now.time_since_epoch());
  const long long int time_as_int = time_since_epoch.count();
  // Use program start time as reference point if available, otherwise epoch start.
  if (kProgramStartTimeMs > 0) {
    return FormatTime(time_as_int - kProgramStartTimeMs);
  } else {
    return to_string(time_as_int);
  }
}

long long int GetMsTime() {
  chrono::system_clock::time_point now = chrono::system_clock::now();
  chrono::milliseconds time_since_epoch =
      chrono::duration_cast<chrono::milliseconds>(
          now.time_since_epoch());
  return time_since_epoch.count();
}

string GetMicroTimeStr() {
  chrono::system_clock::time_point now = chrono::system_clock::now();
  chrono::microseconds time_since_epoch =
      chrono::duration_cast<chrono::microseconds>(
          now.time_since_epoch());
  const long long int time_as_int = time_since_epoch.count();

  // Use program start time as reference point if available, otherwise epoch start.
  if (kProgramStartTimeUs > 0) {
    return FormatTime((time_as_int - kProgramStartTimeUs) / 1000);
  } else {
    return to_string(time_as_int);
  }
}

long long int GetMicroTime() {
  chrono::system_clock::time_point now = chrono::system_clock::now();
  chrono::microseconds time_since_epoch =
      chrono::duration_cast<chrono::microseconds>(
          now.time_since_epoch());
  return time_since_epoch.count();
}

// This function is called in place of std::terminate (since it is the callback
// referenced in the 'set_terminate()' call below).
void global_terminate_fn() {
  abort();
}

bool InitMain(int & argc, char * argv[]) {
  // Set global 'now' time.
  kProgramStartTimeUs = GetMicroTime();
  kProgramStartTimeMs = GetMsTime();

  // Parse command-line args.
  // Start loop at i = 1, as first argument is the program (name) itself.
  for (int i = 1; i < argc; ++i) {
    string arg = string(argv[i]);
    // Parse verbosity.
    if (arg == "--v") {
      if (i == argc - 1) {
        cout << "ERROR Reading Command: Expected argument after "
                "'--out'.\n";
        return false;
      }
      ++i;
      string arg_str = string(argv[i]);
      // HACK. Mingw doesn't support stoi, and I'd rather not
      // #include string_utils.h here, to keep this file as a "leaf" file
      // (no dependencies). Thus, I just test for a few valid values for --v.
      if (arg_str == "0") {
        kVerbosityLevel = 0;
      } else if (arg_str == "1") {
        kVerbosityLevel = 1;
      } else if (arg_str == "2") {
        kVerbosityLevel = 2;
      } else if (arg_str == "3") {
        kVerbosityLevel = 3;
      } else if (arg_str == "4") {
        kVerbosityLevel = 4;
      } else if (arg_str == "5") {
        kVerbosityLevel = 5;
      } else {
        cout << "ERROR: Unsupported verbosity (--v) level: '" << arg_str
             << "'" << endl;
      }
      // Now remove the --v LEVEL from the command-line args.
      for (int j = i + 1; j < argc; ++j) {
        argv[j - 2] = argv[j];
      }
      argc -= 2;
      i -= 2;
    } else if (arg == "--nlog_color") {
      kUseLogColors = false;
      // Now remove the --nlog_color from the command-line args.
      for (int j = i + 1; j < argc; ++j) {
        argv[j - 1] = argv[j];
      }
      argc -= 1;
      i -= 1;
    }
  }

  // Set kNumCores, based on detected number of cores for this OS.
  SetNumCores();

  // Set what happens on calls to std::terminate() (this gets called
  // automatically if a thrown exception is not caught; in particular,
  // since Check and LOG_FATAL are designed to throw an exception,
  // if these are not explicitly caught by the calling code, then
  // the default behavior is set here).
  set_terminate(global_terminate_fn);

  return true;
}

bool InitTestMain(int & argc, char * argv[]) {
  // Set default verbosity for unit tests to be '2' (this can be overidden via
  // command-line flag --v LEVEL).
  SetVerbosity(2);
  return InitMain(argc, argv);
}
