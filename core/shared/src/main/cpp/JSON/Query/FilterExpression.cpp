/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* platform-specific includes */

/* c/c++ standard includes */

/* third-party library includes */

/* project-specific includes */

/* same module include */
#include <JSON/Query/FilterExpression.h>

safrn::FilterExpression::FilterExpression(const nlohmann::json & json) :
    terms(createTerms(json)) {
}

safrn::FilterExpression::FilterExpression(
    const safrn::StudyConfig & study, const nlohmann::json & json) :
    terms(createTerms(study, json)) {
}

std::vector<safrn::FilterTerm>
safrn::FilterExpression::createTerms(const nlohmann::json & json) {

  if (json.empty()) {
    throw NotEnoughTerms();
  }

  std::vector<FilterTerm> result;

  for (const auto & jsonItem : json) {
    result.emplace_back(jsonItem);
  }

  return result;
}

std::vector<safrn::FilterTerm> safrn::FilterExpression::createTerms(
    const safrn::StudyConfig & study, const nlohmann::json & json) {

  if (json.empty()) {
    throw NotEnoughTerms();
  }

  std::vector<FilterTerm> result;

  for (const auto & jsonItem : json) {
    result.emplace_back(study, jsonItem);
  }

  return result;
}
