/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 *
 * DESCRIPTION: Class which describes a set of terms
 * which will be logically AND'ed together.
 */

#ifndef C3CC0980421445DCB9843BF709B2D091
#define C3CC0980421445DCB9843BF709B2D091

/* platform-specific includes */

/* c/c++ standard includes */
#include <exception>
#include <vector>

/* third-party library includes */
#include <nlohmann/json.hpp>

/* project-specific includes */
#include <JSON/Config/StudyConfig.h>
#include <JSON/Query/FilterTerm.h>

namespace safrn {

class FilterExpression {
public:
  explicit FilterExpression(const nlohmann::json & json);
  FilterExpression(
      const safrn::StudyConfig & study, const nlohmann::json & json);

  const std::vector<FilterTerm> terms;

  class NotEnoughTerms : std::exception {
    const char * what() const noexcept override {
      return "Not enough terms.";
    }
  };

private:
  static std::vector<FilterTerm>
  createTerms(const nlohmann::json & json);

  static std::vector<FilterTerm> createTerms(
      const safrn::StudyConfig & study, const nlohmann::json & json);
};

} // namespace safrn

#endif
