/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 *
 * DESCRIPTION: Class which captures parameters for an arbitrary
 * multinomial for the LHS of the Clause value.  The class holds
 * the ADDITIVE terms of the multinomial
 * (and so each slot will be added together).
 * Example: 3x^2 + 2xy is represented as the following:
 * the `terms` vector would have size two, with each term
 * (3x^2 and 2xy) being stored in the corresponding entry of `terms`.
 */

#ifndef AF1199A41F054E73AFA5C7F1AFD542BC
#define AF1199A41F054E73AFA5C7F1AFD542BC

/* platform-specific includes */

/* c/c++ standard includes */
#include <exception>
#include <vector>

/* third-party library includes */
#include <nlohmann/json.hpp>

/* project-specific includes */
#include <JSON/Config/StudyConfig.h>
#include <JSON/Query/FilterMultinomialTerm.h>

namespace safrn {

class FilterMultinomial {
public:
  explicit FilterMultinomial(const nlohmann::json & json);
  FilterMultinomial(
      const safrn::StudyConfig & study, const nlohmann::json & json);

  const std::vector<FilterMultinomialTerm> terms;

  class NotEnoughTerms : std::exception {
    const char * what() const noexcept override {
      return "Not enough terms.";
    }
  };

private:
  static std::vector<FilterMultinomialTerm>
  createTerms(const nlohmann::json & json);

  static std::vector<FilterMultinomialTerm> createTerms(
      const safrn::StudyConfig & study, const nlohmann::json & json);
};

} // namespace safrn

#endif
