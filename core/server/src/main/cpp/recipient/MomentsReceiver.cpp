/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

#if defined(WINDOWS) || defined(__WIN32__) || defined(__WIN64__) || \
    defined(_WIN32) || defined(_WIN64)
#include <io.h>
#else
#include <unistd.h>
#endif

#include <recipient/MomentsReceiver.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <cstdio>
#include <string>
#include <vector>

#include <dataowner/Moments.h>
#include <dataowner/MomentsInfo.h>
#include <dataowner/fortissimo.h>
#include <framework/Framework.h>

/* Logging Configuration */
#include <ff/logging.h>

namespace safrn {
namespace recipient {

void MomentsPrettyPrint(
    std::vector<dataowner::SmallNum> const & results,
    const bool includeCount,
    size_t const bits_of_precision) {
  bool yep = false;
  size_t result_num = 0;
  do {
#if defined(WINDOWS) || defined(__WIN32__) || defined(__WIN64__) || \
    defined(_WIN32) || defined(_WIN64)
    int fd = _open(
        (std::string("Moments-result-") + std::to_string(result_num) +
         ".html")
            .c_str(),
        _O_CREAT | _O_WRONLY | _O_EXCL,
        _S_IREAD | _S_IWRITE);
#else
    int fd = open(
        (std::string("Moments-result-") + std::to_string(result_num) +
         ".html")
            .c_str(),
        O_CREAT | O_WRONLY | O_EXCL,
        S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
#endif

    if (fd < 0) {
      log_debug("tried %zu", result_num);
      result_num++;
      yep = false;
    } else {
      yep = true;
      log_info(
          "Writing result to \"Moments-result-%zu.html\"", result_num);
      FILE * file = fdopen(fd, "w");
      fprintf(
          file,
          "<!DOCTYPE html>\n<html>\n<body>\n<table>\n  <tr>\n    "
          "<th>Statistic</th>\n  <th>Value</th>\n  </tr>\n");

      log_debug("Include count? %d", includeCount);
      size_t include_count_offset = 0;
      if (includeCount) {
        double res_f = (double)results[include_count_offset];
        fprintf(
            file,
            "  <tr>\n    <td>Count</td>\n    <td>%lf</td>\n  </tr>\n",
            res_f);
        log_info("Result[%zu] := %lf", include_count_offset, res_f);
        include_count_offset++;
      }

      size_t highest_moment = results.size() - include_count_offset;

      std::vector<std::string> moment_names = {
          "Mean     ", "Variance ", "Skew     "};

      std::vector<double> moments(results.size());
      for (size_t i = include_count_offset; i < results.size(); i++) {
        double res_f = (double)results[i];
        moments[i - include_count_offset] = res_f /
            (1 << ((i + 2 - include_count_offset) * bits_of_precision));
      }

      if (highest_moment > 1) {
        moments[1] -= (moments[0] * moments[0]);
      }
      if (highest_moment > 2) {
        log_debug("E[X^3] %lf", moments[2]);
        moments[2] =
            (moments[2] - 3 * moments[0] * moments[1] -
             moments[0] * moments[0] * moments[0]);
        moments[2] /= sqrt(moments[1] * moments[1] * moments[1]);
      }

      for (size_t i = include_count_offset; i < results.size(); i++) {
        double res_f = (double)results[i];
        fprintf(
            file,
            "  <tr>\n    <td>%s</td>\n    <td>%lf</td>\n  </tr>\n",
            moment_names[i - include_count_offset].c_str(),
            moments[i - include_count_offset]);
        log_info(
            "Result[%zu] := %lf", i, moments[i - include_count_offset]);
      }

      fprintf(file, "</table>\n</body>\n</html>\n");
      fclose(file);
    }
  } while (!yep);
}

void MomentsReceiver::init() {
  this->getPeers().forEachDataowner(
      [this](Identity const & other) { this->numDataowners++; });
}

void MomentsReceiver::handleReceive(IncomingMessage & imsg) {
  // sum/mod results until no more dataowners
  for (size_t i = 0; i < this->results.size(); i++) {
    dataowner::LargeNum res = 0;
    imsg.read<dataowner::LargeNum>(res);

    this->results[i] = ff::mpc::modAdd(
        this->results[i], res, this->m_info->endModulus);
  }

  this->numDataowners--;
  if (this->numDataowners == 0) {
    std::vector<dataowner::SmallNum> resultsDowncast;
    for (dataowner::LargeNum num : this->results) {
      resultsDowncast.push_back(static_cast<dataowner::SmallNum>(num));
    }

    log_debug("include count? %d", this->m_info->includeZerothMoment);
    MomentsPrettyPrint(
        resultsDowncast,
        this->m_info->includeZerothMoment,
        this->bitsOfPrecision);
    this->complete();
  }
}

void MomentsReceiver::handleComplete(Fronctocol &) {
  log_info("Moments Receiver unexpected handle complete");
  this->abort();
}

void MomentsReceiver::handlePromise(Fronctocol &) {
  log_info("Moments Receiver unexpected handle promise");
  this->abort();
}

std::string MomentsReceiver::name() {
  return std::string("Moments Receiver");
}

} // namespace recipient
} // namespace safrn
