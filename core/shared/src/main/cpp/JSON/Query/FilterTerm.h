/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 *
 * DESCRIPTION: Class for capturing parameters needed to describe
 * the comparison of the integral result of a multinomial to
 * a literal.
 */

#ifndef A786515AC9BE417B90030B210CA6B37D
#define A786515AC9BE417B90030B210CA6B37D

/* platform-specific includes */

/* c/c++ standard includes */

/* third-party library includes */
#include <nlohmann/json.hpp>

/* project-specific includes */
#include <JSON/Config/StudyConfig.h>
#include <JSON/Query/Comparison.h>
#include <JSON/Query/FilterMultinomial.h>
#include <JSON/Query/QueryTypes.h>

namespace safrn {

class FilterTerm {
public:
  explicit FilterTerm(const nlohmann::json & json);
  FilterTerm(
      const safrn::StudyConfig & study, const nlohmann::json & json);

  // TODO: Incorporate comparison into FilterMultinomial
  const FilterMultinomial left;
  const ComparisonValue_t right;
  const Comparison comp;
};

} // namespace safrn

#endif
