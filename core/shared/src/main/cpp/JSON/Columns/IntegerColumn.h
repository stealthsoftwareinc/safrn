/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 *
 * DESCRIPTION: Class which parses and stores
 * fields associated with integer columns
 * in the database.
 */

#ifndef ED77A4EF884A4648B9EA5A93FCF9D9B0
#define ED77A4EF884A4648B9EA5A93FCF9D9B0

/* c/c++ standard includes */
#include <cstdint>
#include <memory>

/* third-party library includes */
#include <nlohmann/json.hpp>

/* project-specific includes */
#include <JSON/Columns/ColumnBase.h>

namespace safrn {

class IntegerColumn : public ColumnBase {
public:
  explicit IntegerColumn(const nlohmann::json & json);

  static std::unique_ptr<ColumnBase>
  CreateFromJSON(const nlohmann::json & JSON);

  const bool isSigned;
  const uint64_t bits;
};

} // namespace safrn

#endif
