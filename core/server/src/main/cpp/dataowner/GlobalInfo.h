/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/*
 * SAFRN GlobalInfo object to hold global constants
 */

#ifndef SAFRN_DATAOWNER_GLOBAL_INFO_H
#define SAFRN_DATAOWNER_GLOBAL_INFO_H

#include <JSON/Config/StudyConfig.h>
#include <JSON/Query/Query.h>
#include <framework/Framework.h>

namespace safrn {
namespace dataowner {

struct GlobalInfo {

  const size_t numDataowners;

  const size_t
      maxListSize; // 100 for now. Much larger, like 10**5 or 10**6 so in practice
  const size_t maxIntersectionSize;

  static const size_t BITS_OF_PRECISION_DEFAULT_VALUE =
      5; // alpha in wiki; 64 usually
  static const size_t BYTES_IN_LOOKUP_TABLE_CELLS_DEFAULT_VALUE = 4;
  static const size_t MAX_F_T_TABLE_NUM_ROWS_DEFAULT_VALUE = 1000;
  static const size_t KEY_MAX_DEFAULT_VALUE = 1048576;
  const size_t bitsOfPrecision;
  const size_t bytesInLookupTableCells;
  const size_t max_F_t_table_num_rows;
  const size_t key_max;

  /** function to use for general testing purposes */
  GlobalInfo(
      const size_t maxSize,
      const size_t nDataowners,
      const size_t maxIntersectionSize,
      const size_t bits_ = BITS_OF_PRECISION_DEFAULT_VALUE,
      const size_t bytes_ = BYTES_IN_LOOKUP_TABLE_CELLS_DEFAULT_VALUE,
      const size_t max_ = MAX_F_T_TABLE_NUM_ROWS_DEFAULT_VALUE,
      const size_t key_max_ = KEY_MAX_DEFAULT_VALUE) :
      numDataowners(nDataowners),
      maxListSize(maxSize),
      maxIntersectionSize(maxIntersectionSize),
      bitsOfPrecision(bits_),
      bytesInLookupTableCells(bytes_),
      max_F_t_table_num_rows(max_),
      key_max(key_max_) {
  }

  /** function to use when parsing actual queries */
  GlobalInfo(const size_t maxSize, const size_t nDataowners) :
      numDataowners(nDataowners),
      maxListSize(maxSize),
      maxIntersectionSize(maxSize * numDataowners),
      bitsOfPrecision(BITS_OF_PRECISION_DEFAULT_VALUE),
      bytesInLookupTableCells(
          BYTES_IN_LOOKUP_TABLE_CELLS_DEFAULT_VALUE),
      max_F_t_table_num_rows(MAX_F_T_TABLE_NUM_ROWS_DEFAULT_VALUE),
      key_max(KEY_MAX_DEFAULT_VALUE) {
  }

  /** Default barebones for 2 party tests */
  GlobalInfo() :
      numDataowners(2),
      maxListSize(100),
      maxIntersectionSize(26),
      bitsOfPrecision(BITS_OF_PRECISION_DEFAULT_VALUE),
      bytesInLookupTableCells(
          BYTES_IN_LOOKUP_TABLE_CELLS_DEFAULT_VALUE),
      max_F_t_table_num_rows(MAX_F_T_TABLE_NUM_ROWS_DEFAULT_VALUE),
      key_max(KEY_MAX_DEFAULT_VALUE) {
  }
};

GlobalInfo generateGlobals(Query const & q, StudyConfig const & cfg);

} // namespace dataowner
} // namespace safrn

#endif // SAFRN_DATAOWNER_GLOBAL_INFO_H
