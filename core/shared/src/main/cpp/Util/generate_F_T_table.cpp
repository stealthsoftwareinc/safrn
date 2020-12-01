/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 *
 * Description: Generates an F table
 */

/* c/c++ standard includes */
#include <fstream>
#include <string>
#include <tuple>
#include <vector>

/* third-party library includes */
#include <gsl/gsl_cdf.h>
#include <nlohmann/json.hpp>

/* project-specific includes */
#include "data_structures.h" // For GenericValue, Distribution, SamplingParameters.
#include "init_utils.h"
#include "map_utils.h"
#include "random_utils.h" // For Random32BitInt().
#include "read_file_utils.h" // For RemoveWindowsTrailingCharacters().
#include <JSON/Config/StudyConfig.h>
#include <JSON/Query/LinearRegressionFunction.h>
#include <JSON/Query/SafrnFunction.h>

/* Logging Configuration */
#include <ff/logging.h>

using namespace file_reader_utils;
using namespace map_utils;
using namespace math_utils;
using namespace string_utils;
using namespace std;

namespace {

//static const int kNumCurrencyDecimalDigits = 100;
//static const int kNumCurrencyDecimalDigits = 100000;
const string kFileCommentChar = "#";
//string kConfigFile = "generate_sample_data.config";
string kComma = ",";
string kSingleQuote = "";
//string kSingleQuote = "'";
bool kPrintOrgId = false;
bool kPrintUserId = false;

// Converts an int in [0-99] into a string of length exactly two.
string AsString(const int input) {
  string to_return = (input < 10) ? "0" : "";
  return to_return + Itoa(input);
}

string TwoDigitNum(const int input) {
  if (input < 10)
    return "0" + Itoa(input);
  return Itoa(input);
}

// ============================== Timestamps ================================
struct BlockTimestamp {
  int year_; // 2000 - 2020
  int month_; // 1 - 12, with 1 = Jan and 12 = Dec
  int day_; // 1 - 31
  int hour_; // 0 - 23
  int min_; // 0 - 59

  BlockTimestamp() {
    year_ = 0;
    month_ = 0;
    day_ = 0;
    hour_ = 0;
    min_ = 0;
  }

  bool IsValidTimestamp() const {
    return (
        year_ < 2021 && year_ > 2000 && month_ > 0 && month_ < 13 &&
        day_ > 0 && day_ < 32 && hour_ >= 0 && hour_ < 24 &&
        min_ >= 0 && min_ < 60);
  }

  string ToString() const {
    if (!IsValidTimestamp())
      return "";
    return "'" + Itoa(year_) + "-" + AsString(month_) + "-" +
        AsString(day_) + " " + AsString(hour_) + ":" + AsString(min_) +
        ":00'";
  }
};

void AdvanceMonth(BlockTimestamp * orig) {
  if (orig->month_ < 12) {
    orig->month_++;
    return;
  }
  orig->month_ = 1;
  orig->year_++;
}
void AdvanceDay(BlockTimestamp * orig) {
  // Check if month is Feb.
  if (orig->month_ == 2) {
    if (orig->day_ < 28 || (orig->year_ % 4 == 0 && orig->day_ < 29)) {
      orig->day_++;
      return;
    }
    // 30 days in: {April = 4, June = 6, September = 9, November = 11)
  } else if (
      orig->month_ == 4 || orig->month_ == 6 || orig->month_ == 9 ||
      orig->month_ == 11) {
    if (orig->day_ < 30) {
      orig->day_++;
      return;
    }
    // 31 days in remaining months.
  } else {
    if (orig->day_ < 31) {
      orig->day_++;
      return;
    }
  }

  // None of the above cases matched, which means I'm in the last day
  // of the month. Reset day to '1', and increase month.
  orig->day_ = 1;
  AdvanceMonth(orig);
}

void AdvanceHour(BlockTimestamp * orig) {
  if (orig->hour_ < 23) {
    orig->hour_++;
    return;
  }
  orig->hour_ = 0;
  AdvanceDay(orig);
}

void AdvanceBlock(const int block_freq, BlockTimestamp * orig) {
  int advance_min_by = block_freq % 60;
  int advance_hour_by = (block_freq / 60) % 24;
  int advance_day_by = (block_freq / (60 * 24));
  if (orig->min_ + advance_min_by < 60) {
    orig->min_ += advance_min_by;
  } else {
    orig->min_ += (advance_min_by - 60);
    AdvanceHour(orig);
  }
  if (orig->hour_ + advance_hour_by < 24) {
    orig->hour_ += advance_hour_by;
  } else {
    orig->hour_ += (advance_hour_by - 24);
    AdvanceDay(orig);
  }
  while (advance_day_by > 0) {
    AdvanceDay(orig);
    advance_day_by--;
  }
}

// Returns true iff (as dates/timestamps):
//   - x < target  OR
//   - x == target and inclusive=TRUE
bool IsBefore(
    const bool inclusive,
    const BlockTimestamp & x,
    const BlockTimestamp & target) {
  if (target.year_ < x.year_)
    return false;
  if (target.year_ > x.year_)
    return true;
  if (target.month_ < x.month_)
    return false;
  if (target.month_ > x.month_)
    return true;
  if (target.day_ < x.day_)
    return false;
  if (target.day_ > x.day_)
    return true;
  if (target.hour_ < x.hour_)
    return false;
  if (target.hour_ > x.hour_)
    return true;
  if (target.min_ < x.min_)
    return false;
  if (target.min_ > x.min_)
    return true;

  // Only reach here if x == target.
  return inclusive;
}

int NumBlocks(
    const int block_freq,
    const BlockTimestamp & first,
    const BlockTimestamp & last) {
  int to_return = 0;
  BlockTimestamp current = first;
  while (IsBefore(true, current, last) &&
         to_return < (60 * 24 * 365 * 21)) {
    ++to_return;
    AdvanceBlock(block_freq, &current);
  }

  return to_return;
}

} // namespace

std::string
printRow(const std::vector<double> & row, const size_t num_digits) {
  std::string ret = Itoa(row.front());

  // Generate data for the rest of the columns.
  for (size_t column_i = 1; column_i < row.size(); ++column_i) {
    ret += ",";
    ret += Itoa(row[column_i], num_digits);
  }
  return ret;
}

bool compareAdjacentRows(
    const std::vector<double> & row1,
    const std::vector<double> & row2,
    const double pct_error_tol,
    const size_t num_digits) {
  // i starts at 1 to avoid column for num_observations
  double ten_to_pow = 1.0;
  for (size_t i = 0; i < num_digits; i++) {
    ten_to_pow *= 0.1;
  }
  for (size_t i = 1; i < row1.size(); i++) {
    if (row1[i] > ten_to_pow || row2[i] > ten_to_pow) {
      if (row1[i] / row2[i] - 1 > pct_error_tol) {
        return true;
      }
      if (row2[i] / row1[i] - 1 > pct_error_tol) {
        return true;
      }
    }
  }
  return false;
}

void GenerateFTable(
    const string & output_dir,
    const size_t nu_1, // num_IVs
    const size_t max_nu_2, // max_num_obs
    const size_t F_bits,
    const size_t F_step_size,
    const double f_max,
    const double pct_error_tol,
    const size_t num_digits) {
  std::vector<string> csv_lines;
  std::vector<double> prev;
  std::vector<double> current;

  // Add header lines.
  csv_lines.push_back("#bits_of_precision=" + Itoa(F_bits));
  csv_lines.push_back("#step_size=" + Itoa(F_step_size));

  double f_diff = pow(0.5, F_bits) * F_step_size;
  size_t num_cols = static_cast<size_t>(ceil(f_max / f_diff));
  current.resize(num_cols + 1);
  for (size_t row_i = 1; row_i < max_nu_2; row_i++) {
    current.front() = row_i;
    for (size_t col_j = 0; col_j < num_cols; col_j++) {
      auto result = gsl_cdf_fdist_Q((1 + col_j) * f_diff, nu_1, row_i);
      current[col_j + 1] = result;
    }
    if (row_i == 1) {
      csv_lines.push_back(printRow(current, num_digits));
      prev = current;
    } else {
      if (compareAdjacentRows(
              current, prev, pct_error_tol, num_digits)) {
        csv_lines.push_back(printRow(current, num_digits));
        prev = current;
      }
    }
  }

  printf("Num lines: csv_lines.size() = %zu", csv_lines.size());
  // Print data for this party.
  const string filename =
      output_dir + "/f_table_num_ivs_" + Itoa(nu_1) + ".csv";
  if (!WriteLines(filename, csv_lines)) {
    log_fatal("Unable to write '%s'", filename.c_str());
  }
}

void GenerateTTable(
    const string & output_dir,
    const size_t max_nu_2, // max_num_obs
    const size_t t_bits,
    const size_t t_step_size,
    const double t_max,
    const double pct_error_tol,
    const size_t num_digits) {

  double t_diff = pow(0.5, t_bits);
  // Generate data for this party.
  std::vector<string> csv_lines;
  std::vector<double> prev;
  std::vector<double> current;
  size_t num_cols =
      static_cast<size_t>(ceil(t_max * t_max / (t_diff * t_diff)));
  current.resize((num_cols / t_step_size) + 1);

  // Add header lines.
  csv_lines.push_back("#bits_of_precision=" + Itoa(t_bits));
  csv_lines.push_back("#step_size=" + Itoa(t_step_size));
  for (size_t row_i = 1; row_i < max_nu_2; ++row_i) {
    current.front() = row_i;
    for (size_t col_j = 0; col_j < num_cols; col_j += t_step_size) {
      auto result = gsl_cdf_tdist_Q(sqrt((1 + col_j) * t_diff), row_i);
      current[(col_j) / t_step_size + 1] = result;
    }

    if (row_i == 1) {
      csv_lines.push_back(printRow(current, num_digits));
      prev = current;
    } else {
      if (compareAdjacentRows(
              current, prev, pct_error_tol, num_digits)) {
        csv_lines.push_back(printRow(current, num_digits));
        prev = current;
      }
    }
  }
  // Print data for this party.
  const string filename = output_dir + "/t_table.csv";
  if (!WriteLines(filename, csv_lines))
    log_fatal("Unable to write '%s'", filename.c_str());
}

int main(int argc, char * argv[]) {
  InitMain();

  // Parse name of config file.
  size_t max_num_obs = 1000;
  if (argc > 1) {
    // Parse Study config, if provided.
    const std::string study_filename = string(argv[1]);

    std::ifstream studyFile(study_filename.c_str());
    if (!studyFile.is_open()) {
      struct CannotOpenFileException : std::exception {
        const char * what() const noexcept override {
          return "Study file cannot be opened";
        }
      };
      throw CannotOpenFileException();
    }
    const nlohmann::json studyJson = nlohmann::json::parse(studyFile);
    safrn::StudyConfig study = safrn::readStudyFromJson(studyJson);

    for (const auto & f : study.allowedQueries) {
      if (f->type == safrn::FunctionType::LIN_REGRESSION) {
        const safrn::LinearRegressionFunction & func =
            *((safrn::LinearRegressionFunction *)f.get());
        max_num_obs = func.max_f_t_table_rows;
        break;
      }
    }
  }

  string output_dir = ".";

  /** TO-DO: 
    * generateGlobalInfo object from study and query config
    * Read in max_num_obs from global info
    * set F_max = 40.0 and t_max = 10.0 by fiat (or from config)
    * Compute diff as 2^(-bits_of_precision)*step_size
  */

  size_t F_bits = 5;
  size_t F_step_size = 1;

  for (size_t i = 1; i < 17; i++) {
    GenerateFTable(
        output_dir,
        i, // num_IVs
        max_num_obs,
        F_bits,
        F_step_size,
        40.0, // F_max
        0.01, // drop if % difference less than 1%
        8); // OR if absolute number less than 10^(-8)
  }

  size_t t_bits = 5;
  size_t t_step_size = 4;

  GenerateTTable(
      output_dir,
      max_num_obs,
      t_bits,
      t_step_size,
      10.0, // t_max
      0.01,
      8);

  log_info("Success! Outputs in: %s/F_t_table.csv", output_dir.c_str());
  return 0;
}
