/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 *
 * DESCRIPTION: Class which parses and stores
 * fields associated with real (fixed-point) columns
 * in the database.
 */

#ifndef FEF78D5B3E804EE8A0635B2D0B4BDE84
#define FEF78D5B3E804EE8A0635B2D0B4BDE84

/* c/c++ standard includes */
#include <memory>
#include <string>

/* third-party library includes */
#include <nlohmann/json.hpp>

/* project-specific includes */
#include <JSON/Columns/ColumnBase.h>

namespace safrn {

class RealColumn : public ColumnBase {
public:
  explicit RealColumn(const nlohmann::json & json);

  static std::unique_ptr<ColumnBase>
  CreateFromJSON(const nlohmann::json & JSON);

  const uint8_t precision;
  const uint8_t scale;
};

} // namespace safrn

#endif
