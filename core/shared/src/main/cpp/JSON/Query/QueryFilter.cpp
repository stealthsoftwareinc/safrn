/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* platform-specific includes */

/* c/c++ standard includes */

/* third-party library includes */

/* project-specific includes */

/* same module include */
#include <JSON/Query/QueryFilter.h>

safrn::QueryFilter::QueryFilter(const nlohmann::json & json) :
    expressions(createExpressions(json)) {
}

safrn::QueryFilter::QueryFilter(
    const safrn::StudyConfig & study, const nlohmann::json & json) :
    expressions(createExpressions(study, json)) {
}

std::vector<safrn::FilterExpression>
safrn::QueryFilter::createExpressions(const nlohmann::json & json) {

  if (json.empty()) {
    throw NotEnoughExpressions();
  }

  std::vector<FilterExpression> result;

  for (const auto & jsonItem : json) {
    result.emplace_back(jsonItem);
  }

  return result;
}

std::vector<safrn::FilterExpression>
safrn::QueryFilter::createExpressions(
    const safrn::StudyConfig & study, const nlohmann::json & json) {

  if (json.empty()) {
    throw NotEnoughExpressions();
  }

  std::vector<FilterExpression> result;

  for (const auto & jsonItem : json) {
    result.emplace_back(study, jsonItem);
  }

  return result;
}
