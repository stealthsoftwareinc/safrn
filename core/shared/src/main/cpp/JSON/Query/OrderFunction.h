/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 *
 * DESCRIPTION: Class that stores the necessary
 * parameters required to specify the nature of
 * an "order" function.
 */

#ifndef INC_8ED52A81F6464BEFB21180635469A029
#define INC_8ED52A81F6464BEFB21180635469A029

/* platform-specific includes */

/* c/c++ standard includes */
#include <memory>

/* third-party library includes */
#include <nlohmann/json.hpp>

/* project-specific includes */
#include <JSON/Config/StudyConfig.h>
#include <JSON/Query/ColumnSpec.h>
#include <JSON/Query/QueryTypes.h>
#include <JSON/Query/RationalFraction.h>
#include <JSON/Query/SafrnFunction.h>

namespace safrn {

// NOTES:
//   - For Min: Set is_percentile_ = true and value_ = 0. Or equivalently,
//              Set is_percentile_ = false, lowest_first_ = true, value_ = 1.
//   - For Max: Set is_percentile_ = true and value_ = 100. Or equivalently,
//              Set is_percentile_ = false, lowest_first_ = false, value_ = 1.
//   - For Median: Set is_percentile_ to 'true' and value_ = 50.
class OrderFunction : public SafrnFunction {
public:
  explicit OrderFunction(const nlohmann::json & json);
  OrderFunction(
      const safrn::StudyConfig & study, const nlohmann::json & json);

  const ColumnSpec col;
  const bool is_percentile;
  const bool lowest_first;
  const size_t value;
};

} // namespace safrn

#endif
