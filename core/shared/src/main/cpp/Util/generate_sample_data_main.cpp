/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 *
 * Description: Generates mock Linear Regression data, as per
 * generate_sample_data.config.  This works as follows: 1)
 * generate_sample_data.config specifies the parameters for generating
 * the joint/aggregate table. Namely, it includes info: a) How many
 * total columns (number of indep. variables, plus dep variable) b) How
 * many total observations c) How to sample (distribution) data for
 * each column d) How to partition the database (number verticals,
 * number parties per vertical, number observations for party).  2)
 * This script first generates all data (viewed as a single-table,
 * virtual database), and then partitions to the parties as specified.
 * As output, we generate .csv files for every party, that represent
 * that party's db: vertical_i_party_j.csv Each .csv file represents
 * that party's data; so the number of .csv columns equals the number
 * of columns in that Vertical, and the number of lines in the .csv
 * file represents the number of row's in that party's DB.
 *
 */

/* C and POSIX Headers */

/* C++ Headers */
#include <fstream>
#include <string>
#include <tuple>
#include <vector>

/* 3rd Party Headers */

/* SAFRN Headers */
#include "Util/data_structures.h" // For Distribution, SamplingParameters.
#include "Util/generate_sample_data_main.h"
#include "Util/init_utils.h"
#include "Util/random_utils.h" // For Random32BitInt().
#include "Util/read_file_utils.h" // For RemoveWindowsTrailingCharacters(), WriteLines.
#include "Util/simulate_data_utils.h" // For SampleValue().
#include "Util/string_utils.h" // For HasPrefixString, Stoi, StripPrefixString.

/* Logging Configuration */
#include <ff/logging.h>

using namespace crypto::random_number;
using namespace file_reader_utils;
using namespace math_utils;
using namespace string_utils;
using namespace std;

namespace {

const size_t maxIdSize = ((size_t)2) << 20;
const int kNumCurrencyDecimalDigits = 100;
const string kFileCommentChar = "#";
string kConfigFile = "generate_sample_data.config";
string kComma = ",";
string kSingleQuote;
bool kPrintOrgId = false;
bool kPrintUserId = false;

string AsString(const int input) {
  string to_return = (input < 10) ? "0" : "";
  return to_return + Itoa(input);
}

// ============================== Timestamps ================================

// ============================== Distributions ================================

SamplingParameters GetNormalDistSamplingParameters(
    const double & mean, const double & std_dev) {
  SamplingParameters sampling_params;
  sampling_params.type_ = Distribution::NORMAL;
  sampling_params.mean_ = mean;
  sampling_params.std_dev_ = std_dev;
  return sampling_params;
}

SamplingParameters GetStandardNormalDistSamplingParameters() {
  return GetNormalDistSamplingParameters(0.0, 1.0);
}

SamplingParameters GetUniformDistSamplingParameters(
    const double & left, const double & right) {
  SamplingParameters sampling_params;
  sampling_params.type_ = Distribution::UNIFORM;
  sampling_params.range_start_ = left;
  sampling_params.range_end_ = right;
  return sampling_params;
}

SamplingParameters GetDiscreteIntegerUniformDistSamplingParameters(
    const int & left, const int & right) {
  SamplingParameters sampling_params;
  sampling_params.type_ = Distribution::DISCRETE_UNIFORM;
  sampling_params.range_start_ = left;
  sampling_params.range_end_ = right;
  return sampling_params;
}

SamplingParameters GetDiscreteDecimalUniformDistSamplingParameters(
    const double & left,
    const double & right,
    const double & step_size) {
  SamplingParameters sampling_params;
  sampling_params.type_ = Distribution::DISCRETE_UNIFORM;
  sampling_params.range_start_ = left;
  sampling_params.range_end_ = right;
  sampling_params.constant_ = step_size;
  return sampling_params;
}

SamplingParameters GetPoissonSamplingParameters(const double & lambda) {
  SamplingParameters sampling_params;
  sampling_params.type_ = Distribution::POISSON;
  sampling_params.mean_ = lambda;
  return sampling_params;
}

SamplingParameters
GetDistSamplingParameters(const SafrnDistribution & dist) {
  if (dist.type_ == DistributionType::NORMAL) {
    return GetNormalDistSamplingParameters(dist.mean_, sqrt(dist.var_));
  } else if (dist.type_ == DistributionType::UNIFORM) {
    return GetUniformDistSamplingParameters(
        dist.mean_ - (dist.max_ - dist.mean_), dist.max_);
  } else if (dist.type_ == DistributionType::DISCRETE_UNIFORM) {
    return GetDiscreteDecimalUniformDistSamplingParameters(
        dist.mean_ - (dist.max_ - dist.mean_), dist.max_, dist.var_);
  } else if (dist.type_ == DistributionType::POISSON) {
    return GetPoissonSamplingParameters(dist.mean_);
  } else {
    log_fatal("Distribution type not set.");
  }
}
// ============================ END Distributions ==============================

} // namespace

bool ParseConfigFile(
    string * output_dir,
    size_t * num_total_cols,
    size_t * num_total_common_rows,
    size_t * num_verticals,
    vector<VerticalInfo> * info_per_vertical) {
  ifstream input_file(kConfigFile);
  if (!input_file.is_open()) {
    log_fatal("Unable to open config file '%s'", kConfigFile.c_str());
    return false;
  }
  int line_num = 0;
  string orig_line;
  string line;
  bool parsed_num_verticals = false;
  while (getline(input_file, orig_line)) {
    ++line_num;
    RemoveWindowsTrailingCharacters(&orig_line);
    line = RemoveAllWhitespace(orig_line);
    if (line.empty() ||
        HasPrefixString(line, string(kFileCommentChar))) {
      continue;
    }
    auto comment_pos = line.find(kFileCommentChar);
    if (comment_pos != std::string::npos) {
      line = line.substr(0, comment_pos);
    }

    if (!parsed_num_verticals) {
      if (HasPrefixString(line, "OUTPUT_DIR=")) {
        line = StripPrefixString(line, "OUTPUT_DIR=");
        if (!line.empty())
          *output_dir = line;
      } else if (HasPrefixString(line, "NUM_TOTAL_COLS=")) {
        line = StripPrefixString(line, "NUM_TOTAL_COLS=");
        if (!Stoi(line, num_total_cols))
          return false;
      } else if (HasPrefixString(line, "NUM_TOTAL_COMMON_ROWS=")) {
        line = StripPrefixString(line, "NUM_TOTAL_COMMON_ROWS=");
        if (!Stoi(line, num_total_common_rows))
          return false;
      } else if (HasPrefixString(line, "NUM_VERTICALS=")) {
        if (parsed_num_verticals)
          return false;
        line = StripPrefixString(line, "NUM_VERTICALS=");
        if (!Stoi(line, num_verticals))
          return false;
        parsed_num_verticals = true;
        info_per_vertical->resize(*num_verticals);
      } else {
        return false;
      }
      continue;
    }

    // That we reached here means this line is info for some
    // vertical. Parse it.
    if (!HasPrefixString(line, "V")) {
      log_error("Unexpected line: '%s'", line.c_str());
      return false;
    }
    string line_suffix = StripPrefixString(line, "V");
    size_t first_break_pos = line_suffix.find("_");
    if (first_break_pos == string::npos || first_break_pos == 0) {
      log_fatal("Unexpected line: '%s'", line.c_str());
      return false;
    }
    const string vertical_index =
        line_suffix.substr(0, first_break_pos);
    int v_index;
    if (!Stoi(vertical_index, &v_index) || v_index < 0 ||
        v_index >= *num_verticals) {
      log_error("Bad vertical index: '%s'", line.c_str());
      return false;
    }
    VerticalInfo & v_info = (*info_per_vertical)[v_index];
    line_suffix = line_suffix.substr(first_break_pos + 1);

    // Check if it's vertical header info.
    if (HasPrefixString(line_suffix, "NUM_PARTIES=")) {
      line_suffix = StripPrefixString(line_suffix, "NUM_PARTIES=");
      if (!Stoi(line_suffix, &(v_info.num_parties_)))
        return false;
      v_info.num_rows_per_party_.resize(v_info.num_parties_);
      continue;
    } else if (HasPrefixString(line_suffix, "KEY_NAME=")) {
      line_suffix = StripPrefixString(line_suffix, "KEY_NAME=");
      v_info.key_name_ = line_suffix;
      continue;
    } else if (HasPrefixString(line_suffix, "NUM_COLS=")) {
      line_suffix = StripPrefixString(line_suffix, "NUM_COLS=");
      if (!Stoi(line_suffix, &(v_info.num_cols_)))
        return false;
      v_info.column_info_.resize(v_info.num_cols_);
      continue;
    } else if (HasPrefixString(line_suffix, "NUM_ROWS=")) {
      line_suffix = StripPrefixString(line_suffix, "NUM_ROWS=");
      if (!Stoi(line_suffix, &(v_info.num_rows_)))
        return false;
      continue;
    }

    // That you reached here means we're parsing either party or
    // column info.  Check if this is party info (which is just num
    // rows for this party).
    if (HasPrefixString(line_suffix, "P")) {
      line_suffix = StripPrefixString(line_suffix, "P");
      size_t break_pos = line_suffix.find("_");
      if (break_pos == string::npos || break_pos == 0) {
        log_error("Unexpected line: '%s'", line.c_str());
        return false;
      }
      const string party_index = line_suffix.substr(0, break_pos);
      int p_index;
      if (!Stoi(party_index, &p_index) || p_index < 0 ||
          p_index >= v_info.num_parties_) {
        log_error("Bad party index: '%s'", line.c_str());
        return false;
      }
      line_suffix = line_suffix.substr(break_pos + 1);
      if (!HasPrefixString(line_suffix, "NUM_ROWS="))
        return false;
      line_suffix = StripPrefixString(line_suffix, "NUM_ROWS=");
      if (!Stoi(line_suffix, &(v_info.num_rows_per_party_[p_index])))
        return false;
      continue;
    }

    // That you reached here means we're parsing column info.
    if (!HasPrefixString(line_suffix, "C")) {
      log_error("Unexpected line: '%s'", line.c_str());
      return false;
    }
    line_suffix = StripPrefixString(line_suffix, "C");
    size_t break_pos = line_suffix.find("_");
    if (break_pos == string::npos || break_pos == 0) {
      log_error("Unexpected line: '%s'", line.c_str());
      return false;
    }
    const string column_index = line_suffix.substr(0, break_pos);
    int c_index;
    if (!Stoi(column_index, &c_index) || c_index < 0 ||
        c_index >= v_info.num_cols_) {
      log_error("Bad column index: '%s'", line.c_str());
      return false;
    }
    line_suffix = line_suffix.substr(break_pos + 1);
    auto & col_info = v_info.column_info_[c_index];
    if (HasPrefixString(line_suffix, "DATATYPE=")) {
      line_suffix = StripPrefixString(line_suffix, "DATATYPE=");
      if (line_suffix == "DOUBLE") {
        col_info.datatype_ = SafrnDatatype::DOUBLE;
      } else {
        log_error("Unsupported datatype: '%s'", line_suffix.c_str());
        return false;
      }
    } else if (HasPrefixString(line_suffix, "DIST=")) {
      line_suffix = StripPrefixString(line_suffix, "DIST=");
      if (line_suffix == "NORMAL") {
        col_info.distribution_.type_ = DistributionType::NORMAL;
      } else if (line_suffix == "UNIFORM") {
        col_info.distribution_.type_ = DistributionType::UNIFORM;
      } else if (line_suffix == "DISCRETE_UNIFORM") {
        col_info.distribution_.type_ =
            DistributionType::DISCRETE_UNIFORM;
      } else if (line_suffix == "POISSON") {
        col_info.distribution_.type_ = DistributionType::POISSON;
      } else {
        log_error(
            "Unrecognized distribution type: '%s'",
            line_suffix.c_str());
        return false;
      }
    } else if (HasPrefixString(line_suffix, "NAME=")) {
      line_suffix = StripPrefixString(line_suffix, "NAME=");
      col_info.column_name_ = line_suffix;
    } else if (HasPrefixString(line_suffix, "MEAN=")) {
      line_suffix = StripPrefixString(line_suffix, "MEAN=");
      if (!Stod(line_suffix, &(col_info.distribution_.mean_)))
        return false;
    } else if (HasPrefixString(line_suffix, "VAR=")) {
      line_suffix = StripPrefixString(line_suffix, "VAR=");
      if (line_suffix == "NA")
        continue;
      if (!Stod(line_suffix, &(col_info.distribution_.var_)))
        return false;
    } else if (HasPrefixString(line_suffix, "MAX=")) {
      line_suffix = StripPrefixString(line_suffix, "MAX=");
      if (line_suffix == "NA")
        continue;
      if (!Stod(line_suffix, &(col_info.distribution_.max_)))
        return false;
    } else {
      log_error("Unexpected line: '%s'", line.c_str());
      return false;
    }
  }

  return true;
}

void GenerateDataForParty(
    const string & output_dir,
    const string & header_line,
    const size_t current_start_col_index,
    const bool is_first_vertical,
    const bool is_last_vertical,
    const size_t v_index,
    const size_t p_index,
    const size_t num_rows,
    const vector<ColumnInfo> & col_info,
    const vector<double> & coeffs,
    size_t * num_remaining_rows_total,
    size_t * num_remaining_rows_in_vert,
    set<size_t> * used_ids,
    vector<size_t> * intersection_ids,
    vector<vector<double>> * joined_table) {
  auto unif_r = GetUniformDistSamplingParameters(0.0, 1.0);
  // Generate data for this party.
  vector<string> csv_lines(num_rows + 1);
  csv_lines[0] = header_line;

  for (size_t row_i = 1; row_i < num_rows + 1; ++row_i) {
    // Determine if this row is among the intersecting rows.
    double prob_in_intersection = ((double)*num_remaining_rows_total) /
        ((double)*num_remaining_rows_in_vert);
    double unif_random;
    if (!SimulationUtils::SampleValue(unif_r, &unif_random)) {
      log_fatal("Failed to Sample num_invoices Value.");
    }
    auto const is_row_in_intersection =
        unif_random < prob_in_intersection;

    // Add row id to output.
    if (!is_first_vertical && is_row_in_intersection) {
      csv_lines[row_i] =
          Itoa((*intersection_ids)[*num_remaining_rows_total - 1]);
    } else {
      size_t new_id = RandomInModulus((uint64_t)maxIdSize);
      size_t num_failures = 0;
      while (used_ids->find(new_id) != used_ids->end() &&
             num_failures < maxIdSize) {
        ++num_failures;
        if (new_id < maxIdSize)
          ++new_id;
        else
          new_id = 0;
      }
      if (num_failures == maxIdSize)
        log_fatal("Too many rows");
      used_ids->insert(new_id);
      csv_lines[row_i] = Itoa(new_id);
      if (is_row_in_intersection) {
        intersection_ids->push_back(new_id);
      }
    }

    // Generate data for the rest of the columns.
    for (size_t column_i = 0; column_i < col_info.size(); ++column_i) {
      auto const & col_i = col_info[column_i];
      csv_lines[row_i] += ",";
      double col_value;
      if (!SimulationUtils::SampleValue(
              GetDistSamplingParameters(col_i.distribution_),
              &col_value)) {
        log_fatal("Failed to Sample num_invoices Value.");
      }

      if (column_i < col_info.size() - 1 || !is_last_vertical ||
          !is_row_in_intersection) {
        csv_lines[row_i] += Itoa(col_value, 4);

        // Add row info to intersection set.
        if (is_row_in_intersection) {
          (*joined_table)
              [joined_table->size() - *num_remaining_rows_total]
              [current_start_col_index + column_i] = col_value;
        }
      } else {
        // If this is the last vertical, also generate/compute the
        // dependent value.
        auto y = col_value;
        for (size_t i = 0; i < coeffs.size(); ++i) {
          y += coeffs[i] *
              (*joined_table)
                  [joined_table->size() - *num_remaining_rows_total][i];
        }
        csv_lines[row_i] += Itoa(y, 4);
      }
    }

    (*num_remaining_rows_in_vert)--;
    if (is_row_in_intersection)
      (*num_remaining_rows_total)--;
  }

  // Print data for this party.
  auto const filename = output_dir + "/vertical_" + Itoa(v_index) +
      "_party_" + Itoa(p_index) + ".csv";

  if (!WriteLines(filename, csv_lines))
    log_fatal("Unable to write '%s'", filename.c_str());
}

void GenerateDataForVertical(
    const bool is_last_vertical,
    const string & output_dir,
    const string & header_line,
    const size_t v_index,
    const VerticalInfo & vert_info,
    const size_t current_start_col_index,
    const vector<double> & coeffs,
    set<size_t> * used_ids,
    vector<size_t> * intersection_ids,
    vector<vector<double>> * joined_table) {
  auto const num_intersection_rows = joined_table->size();
  auto const num_vertical_rows = vert_info.num_rows_;
  if (num_vertical_rows < num_intersection_rows) {
    log_fatal("bad");
  }
  auto const is_first_vertical = intersection_ids->empty();
  auto num_remaining_rows_total = num_intersection_rows;
  auto num_remaining_rows_in_vert = num_vertical_rows;

  // Loop through each party.
  for (size_t p_index = 0; p_index < vert_info.num_parties_;
       ++p_index) {
    GenerateDataForParty(
        output_dir,
        header_line,
        current_start_col_index,
        is_first_vertical,
        is_last_vertical,
        v_index,
        p_index,
        vert_info.num_rows_per_party_[p_index],
        vert_info.column_info_,
        coeffs,
        &num_remaining_rows_total,
        &num_remaining_rows_in_vert,
        used_ids,
        intersection_ids,
        joined_table);
  }
}

/**
 *  The main function to generate sample data.  To use, update
 *  generate_sample_data.config as desired, and then run:
 *
 *  ./generate_sample_data_main  FILEPATH_TO/generate_sample_data.config
 *
 *  where: If no filepath is given on command-line as 2nd arg, then
 *  default is to use kConfigFile (="./generate_sample_data.config").
 *
 */
int main(int argc, char * argv[]) {
  InitMain();

  // Parse name of config file.
  if (argc > 1)
    kConfigFile = string(argv[1]);

  // Parse the configuration file.
  string output_dir;
  size_t num_total_cols = 0;
  size_t num_total_common_rows = 0;
  size_t num_verticals = 0;
  vector<VerticalInfo> info_per_vertical;
  if (!ParseConfigFile(
          &output_dir,
          &num_total_cols,
          &num_total_common_rows,
          &num_verticals,
          &info_per_vertical)) {
    return 1;
  }

  if (num_verticals == 0) {
    log_error("Too few verticals.");
    return 1;
  }

  // Seed Pseduo-random generator.
  // NOTE: To have deterministic behavior, seed with a constant instead of
  // via Random32BitInt(); e.g. allow user to provide seed on the command-line.
  auto const orig_master_seed = Random32BitInt();
  SimulationUtils::ResetPnrgSeed(orig_master_seed);

  // Keep track of row_ids in the intersection.
  vector<size_t> intersection_ids;
  intersection_ids.reserve(num_total_common_rows);

  // Keep track of used row ids.
  set<size_t> used_row_ids;

  vector<vector<double>> joined_table(
      num_total_common_rows, vector<double>(num_total_cols));

  vector<double> coeffs{0.1, 0.5, 0.9};

  // Generate data for all verticals, one vertical at a time.
  size_t current_start_col_index = 0;
  for (size_t vert_i = 0; vert_i < num_verticals - 1; ++vert_i) {
    string header_line = info_per_vertical[vert_i].key_name_;
    for (auto obj : info_per_vertical[vert_i].column_info_) {
      header_line += ",";
      header_line += obj.column_name_;
    }
    GenerateDataForVertical(
        false,
        output_dir,
        header_line,
        vert_i,
        info_per_vertical[vert_i],
        current_start_col_index,
        coeffs,
        &used_row_ids,
        &intersection_ids,
        &joined_table);
    current_start_col_index += info_per_vertical[vert_i].num_cols_;
  }

  // Now do last vertical.
  auto header_line_last =
      info_per_vertical[num_verticals - 1].key_name_;
  for (auto obj : info_per_vertical[num_verticals - 1].column_info_) {
    header_line_last += ",";
    header_line_last += obj.column_name_;
  }
  GenerateDataForVertical(
      true,
      output_dir,
      header_line_last,
      num_verticals - 1,
      info_per_vertical[num_verticals - 1],
      current_start_col_index,
      coeffs,
      &used_row_ids,
      &intersection_ids,
      &joined_table);

  log_info(
      "Success! Outputs in: %s/vertical_i_party_j.csv",
      output_dir.c_str());
  return 0;
}
