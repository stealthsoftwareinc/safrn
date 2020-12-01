/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 *
 * DESCRIPTION: Class with captures top-level query
 * filter parameters and expressions which are
 * OR'd together.
 */

#ifndef INC_7A51964F16364B80B0CC66E3AFBCB377
#define INC_7A51964F16364B80B0CC66E3AFBCB377

/* platform-specific includes */

/* c/c++ standard includes */
#include <cstdint>
#include <exception>
#include <tuple>
#include <vector>

/* third-party library includes */
#include <nlohmann/json.hpp>

/* project-specific includes */
#include <JSON/Config/StudyConfig.h>
#include <JSON/Query/ColumnSpec.h>
#include <JSON/Query/Comparison.h>
#include <JSON/Query/FilterExpression.h>
#include <JSON/Query/QueryTypes.h>
#include <JSON/Query/RationalFraction.h>

namespace safrn {

class QueryFilter {
public:
  explicit QueryFilter(const nlohmann::json & json);
  QueryFilter(
      const safrn::StudyConfig & study, const nlohmann::json & json);

  std::vector<FilterExpression> expressions;

  class NotEnoughExpressions : std::exception {
    const char * what() const noexcept override {
      return "Not enough expressions defined.";
    }
  };

private:
  static std::vector<FilterExpression>
  createExpressions(const nlohmann::json & json);

  static std::vector<FilterExpression> createExpressions(
      const safrn::StudyConfig & study, const nlohmann::json & json);
};

} // namespace safrn

#endif
