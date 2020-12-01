/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 *
 * Description: For F/t-table parsing
 */
#include <fstream>

#include <Util/read_file_utils.h>
#include <Util/string_utils.h>
#include <dealer/RandomTableLookup.h>

/* Logging Configuration */
#include <ff/logging.h>

namespace safrn {
namespace dealer {

bool read_table_csv_file(
    std::string const file_name,
    std::vector<size_t> & row_ids,
    std::vector<std::vector<Boolean_t>> & table_data,
    dataowner::LargeNum & num_cols,
    size_t & bits_of_precision,
    dataowner::LargeNum & step_size,
    size_t max_num_rows,
    size_t num_bytes) {
  std::ifstream input_stream(file_name.c_str());
  if (!input_stream.is_open()) {
    log_error("Error opening file %s", file_name.c_str());
    return false;
  }

  bits_of_precision = 5;
  step_size = 1;

  std::string line;

  bool first_time_through = true;

  row_ids.reserve(max_num_rows);
  log_debug("about to read");
  size_t cell_counter = 0;
  while (getline(input_stream, line)) {
    //log_debug("Hi, line is %s", line.c_str());
    file_reader_utils::RemoveWindowsTrailingCharacters(&line);
    line = string_utils::RemoveAllWhitespace(line);
    if (string_utils::HasPrefixString(line, "#")) {
      if (string_utils::HasPrefixString(line, "#bits_of_precision=")) {
        std::string suffix;
        string_utils::StripPrefixString(
            line, "#bits_of_precision=", &suffix);
        if (!string_utils::Stoi(suffix, &bits_of_precision)) {
          log_error(
              "Failed parsing bits_of_precision: '%s'", suffix.c_str());
          return false;
        }
      } else if (string_utils::HasPrefixString(line, "#step_size=")) {
        std::string suffix;
        string_utils::StripPrefixString(line, "#step_size=", &suffix);
        uint64_t step_size64 = 0;
        if (!string_utils::Stoi(suffix, &step_size64)) {
          log_error("Failed parsing step_size: '%s'", suffix.c_str());
          return false;
        } else {
          step_size = step_size64;
        }
      }
      continue;
    }

    std::vector<std::string> split_line;
    string_utils::Split(line, ",", &split_line);
    std::vector<std::vector<Boolean_t>> parsed_line;
    parsed_line.reserve(split_line.size() - 1);
    dataowner::SmallNum row_id;
    size_t i = 0;
    for (std::string & s : split_line) {
      if (i != 0) {
        dataowner::SmallNum shifted_cell_value =
            static_cast<dataowner::SmallNum>(
                floor(pow(2, 8 * num_bytes) * atof(s.c_str())));
        std::vector<Boolean_t> cell_value_bytes;
        cell_value_bytes.reserve(num_bytes);
        for (size_t j = 0; j < num_bytes; j++) {
          // re-cast an integer as a byte string
          cell_value_bytes.push_back(
              static_cast<Boolean_t>(shifted_cell_value % 256));

          shifted_cell_value /= 256;
        }
        parsed_line.push_back(cell_value_bytes);
      } else {
        row_id = static_cast<dataowner::SmallNum>(atoi(s.c_str()));
      }
      cell_counter++;
      i++;
    }
    if (first_time_through) {
      num_cols = dataowner::LargeNum(parsed_line.size());
      first_time_through = false;
    }
    row_ids.push_back(row_id);
    auto it = std::next(
        parsed_line.begin(), parsed_line.end() - parsed_line.begin());
    std::move(parsed_line.begin(), it, std::back_inserter(table_data));
  }
  log_debug("table_data.size() %zu", table_data.size());
  return true;
}

RandomTableLookup::RandomTableLookup(
    RandomTableLookupInfo const & info) :
    RandomTableLookup() {
}

void RandomTableLookupInfo::generate(
    ::std::size_t n_parties,
    size_t /*unused*/,
    ::std::vector<RandomTableLookup> & vals) const {
  vals.clear();
  vals.reserve(n_parties);

  log_debug("Calling generate");
  for (::std::size_t i = 0; i < n_parties; ++i) {
    vals.emplace_back(RandomTableLookup(
        dataowner::LargeNum(1),
        ::std::vector<Boolean_t>(this->table_size_)));
  }

  log_debug("HI");

  log_debug("Table size: %zu", this->table_size_);

  /* Step 1: Randomly create the original TableLookup instance */
  auto & orig = vals.front();

  dataowner::SmallNum original_r_small =
      ::ff::mpc::randomModP<dataowner::SmallNum>(this->table_size_);

  log_debug("r %u", original_r_small);
  orig.u_[original_r_small] = 0x01;
  orig.r_ = original_r_small;

  log_debug("original_r_small %u", original_r_small);

  /* Step 2: Randomly secret share the original */
  for (::std::size_t i = 1; i < n_parties; ++i) {
    auto & share = vals[i];
    share.r_ =
        ::ff::mpc::randomModP<dataowner::LargeNum>(this->r_modulus_);
    orig.r_ = ::ff::mpc::modSub(orig.r_, share.r_, this->r_modulus_);

    if (!ff::mpc::randomBytes(share.u_.data(), share.u_.size())) {
      throw std::runtime_error("Bad rands");
    }

    for (::std::size_t j = 0; j < share.u_.size(); ++j) {
      /** n.b. these both need to be random bits, not random bytes, for efficient share reconstruction */
      orig.u_[j] = orig.u_[j] ^ (share.u_[j] & 0x01);
      share.u_[j] = share.u_[j] & 0x01;
    }
  }
  log_debug("leaving generate");
}

} // namespace dealer
} // namespace safrn
