/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 *
 * DESCRIPTION: Class which provides base utilities
 * and common storage for all columns in the database.
 */

#ifndef INC_4F376E2B865440B283619B693DFECB1D
#define INC_4F376E2B865440B283619B693DFECB1D

/* c/c++ standard includes */
#include <string>

/* third-party library includes */
#include <nlohmann/json.hpp>

/* project-specific includes */
#include <JSON/Query/QueryTypes.h>

namespace safrn {

// Enumerates the kinds of column datatypes supported by SAFRN
enum class ColumnDatatype {
  BOOL,
  INTEGER,
  REAL,
  CATEGORICAL // Different from BOOL: assume an enumeration on the column.
};

class ColumnBase {
public:
  explicit ColumnBase(const nlohmann::json & json);

  static constexpr size_t MAXIMUM_NAME_LENGTH = 64;

  const ColumnDatatype type;
  const std::string name;
  const ColumnIndex_t index;
};

} // namespace safrn

#endif
