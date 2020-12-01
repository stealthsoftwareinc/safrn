/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* platform-specific includes */

/* c/c++ standard includes */

/* third-party library includes */

/* project-specific includes */

/* same module include */
#include <JSON/Query/FilterMultinomial.h>

safrn::FilterMultinomial::FilterMultinomial(
    const nlohmann::json & json) :
    terms(createTerms(json)) {
}

safrn::FilterMultinomial::FilterMultinomial(
    const safrn::StudyConfig & study, const nlohmann::json & json) :
    terms(createTerms(study, json)) {
}

std::vector<safrn::FilterMultinomialTerm>
safrn::FilterMultinomial::createTerms(const nlohmann::json & json) {

  if (json.empty()) {
    throw NotEnoughTerms();
  }

  std::vector<FilterMultinomialTerm> result;

  for (const auto & jsonItem : json) {
    result.emplace_back(jsonItem);
  }

  return result;
}

std::vector<safrn::FilterMultinomialTerm>
safrn::FilterMultinomial::createTerms(
    const safrn::StudyConfig & study, const nlohmann::json & json) {

  if (json.empty()) {
    throw NotEnoughTerms();
  }

  std::vector<FilterMultinomialTerm> result;

  for (const auto & jsonItem : json) {
    result.emplace_back(study, jsonItem);
  }

  return result;
}
