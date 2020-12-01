/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

#ifndef SAFRN_DEALER_RANDOM_TABLE_LOOKUP_H_
#define SAFRN_DEALER_RANDOM_TABLE_LOOKUP_H_

/* C and POSIX Headers */

/* C++ Headers */
#include <cstdint>
#include <utility>
#include <vector>

/* 3rd Party Headers */
#include <ff/Message.h>
#include <mpc/Randomness.h>
#include <mpc/templates.h>

#include <dataowner/fortissimo.h>

/* Logging config */
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
    size_t num_bytes);

class RandomTableLookupInfo;

class RandomTableLookup {
public:
  RandomTableLookup(
      const dataowner::LargeNum r, ::std::vector<Boolean_t> u) :
      r_(r), u_(std::move(u)){};

  RandomTableLookup() : RandomTableLookup(0U, {}){};

  RandomTableLookup(RandomTableLookupInfo const & info);

  dataowner::LargeNum r_ = dataowner::LargeNum(0);
  ::std::vector<Boolean_t> u_;

  static std::string name() {
    return std::string("Random Table Lookup");
  }
};

class RandomTableLookupInfo {
public:
  RandomTableLookupInfo(
      const dataowner::LargeNum r_modulus,
      const dataowner::SmallNum table_size) :
      r_modulus_(r_modulus), table_size_(table_size){};

  RandomTableLookupInfo() : RandomTableLookupInfo(1U, 0U){};

  ::std::size_t instanceSize() const {
    return (static_cast<size_t>(this->table_size_) *
            sizeof(Boolean_t)) +
        (ff::mpc::numberLen(this->r_modulus_));
  }

  void generate(
      ::std::size_t n_parties,
      ::std::size_t /*unused*/,
      ::std::vector<RandomTableLookup> & vals) const;

  bool operator==(RandomTableLookupInfo const & other) const {
    return this->r_modulus_ == other.r_modulus_ &&
        this->table_size_ == other.table_size_;
  }

  bool operator!=(RandomTableLookupInfo const & other) const {
    return !(*this == other);
  }

  dataowner::LargeNum r_modulus_ = dataowner::LargeNum(0);
  dataowner::SmallNum table_size_;

private:
};

} // namespace dealer
} // namespace safrn

#include <dealer/RandomTableLookup.t.h>

#define LOG_UNCLUDE
#include <ff/logging.h>

#endif /* SAFRN_DEALER_RANDOM_TABLE_LOOKUP_H_ */
