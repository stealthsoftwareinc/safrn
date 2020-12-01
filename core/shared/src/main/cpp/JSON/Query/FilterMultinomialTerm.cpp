/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* platform-specific includes */

/* c/c++ standard includes */

/* third-party library includes */

/* project-specific includes */

/* same module include */
#include <JSON/Query/FilterMultinomialTerm.h>

safrn::FilterMultinomialTerm::FilterMultinomialTerm(
    const nlohmann::json & json) :
    coefficient(json["coefficient"]),
    values(createValues(json["values"])) {
}

safrn::FilterMultinomialTerm::FilterMultinomialTerm(
    const safrn::StudyConfig & study, const nlohmann::json & json) :
    coefficient(json["coefficient"]),
    values(createValues(study, json["values"])) {
}

std::vector<safrn::FilterMultinomialTerm::ColumnExponent>
safrn::FilterMultinomialTerm::createValues(
    const nlohmann::json & json) {
  std::vector<ColumnExponent> result;

  for (const auto & jsonItem : json) {
    result.emplace_back(jsonItem);
  }

  return result;
}

std::vector<safrn::FilterMultinomialTerm::ColumnExponent>
safrn::FilterMultinomialTerm::createValues(
    const safrn::StudyConfig & study, const nlohmann::json & json) {
  std::vector<ColumnExponent> result;

  for (const auto & jsonItem : json) {
    result.emplace_back(study, jsonItem);
  }

  return result;
}

safrn::FilterMultinomialTerm::ColumnExponent::ColumnExponent(
    const nlohmann::json & json) :
    col(json["col"]), exp(createExponent(json["exp"])) {
}

safrn::FilterMultinomialTerm::ColumnExponent::ColumnExponent(
    const safrn::StudyConfig & study, const nlohmann::json & json) :
    col(study, json["col"]), exp(createExponent(json["exp"])) {
}

safrn::Exponent_t
safrn::FilterMultinomialTerm::ColumnExponent::createExponent(
    const nlohmann::json & json) {
  const auto result = (safrn::Exponent_t)json;

  if (result < 0) {
    throw std::runtime_error("Value out of bounds.");
  }

  return result;
}
