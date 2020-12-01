/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* platform-specific includes */

/* c/c++ standard includes */
#include <exception>

/* third-party library includes */

/* project-specific includes */
#include <JSON/Query/FunctionFactory.h>
#include <Util/Utils.h>

/* same module include */
#include <JSON/Query/Query.h>

std::vector<safrn::QueryFilter>
safrn::Query::createPrefilters(const nlohmann::json & json) {
  std::vector<QueryFilter> result;

  if (json.size() > 2) {
    throw TooManyPrefilters();
  }

  for (const auto & jsonFilter : json) {
    result.emplace_back(jsonFilter);
  }

  return result;
}

std::vector<safrn::QueryFilter> safrn::Query::createPrefilters(
    const safrn::StudyConfig & study, const nlohmann::json & json) {
  std::vector<QueryFilter> result;

  if (json.size() > 2) {
    throw TooManyPrefilters();
  }

  for (const auto & jsonFilter : json) {
    result.emplace_back(study, jsonFilter);
  }

  return result;
}

safrn::Query::Query(const nlohmann::json & json) :
    prefilters(createPrefilters(json["prefilters"])),
    joinStatement(createJoinStatementIfExists(json)),
    function(FunctionFactory(json["function"])) {
}

safrn::Query::Query(
    const safrn::StudyConfig & study, const nlohmann::json & json) :
    prefilters(createPrefilters(study, json["prefilters"])),
    joinStatement(createJoinStatementIfExists(study, json)),
    function(FunctionFactory(study, json["function"])) {
}

std::unique_ptr<safrn::JoinStatement>
safrn::Query::createJoinStatementIfExists(
    const nlohmann::json & topJson) {
  constexpr auto JOIN_STATMENT_KEY = "joinStatement";

  if (json_contains(topJson, JOIN_STATMENT_KEY)) {
    return std::unique_ptr<JoinStatement>(
        new JoinStatement(topJson[JOIN_STATMENT_KEY]));
  } else {
    /* Return empty JoinStatement if
     * it didn't exist in the JSON. */
    return std::unique_ptr<JoinStatement>();
  }
}

std::unique_ptr<safrn::JoinStatement>
safrn::Query::createJoinStatementIfExists(
    const safrn::StudyConfig & study, const nlohmann::json & topJson) {
  constexpr auto JOIN_STATMENT_KEY = "joinStatement";

  if (json_contains(topJson, JOIN_STATMENT_KEY)) {
    return std::unique_ptr<JoinStatement>(
        new JoinStatement(study, topJson[JOIN_STATMENT_KEY]));
  } else {
    /* Return empty JoinStatement if
     * it didn't exist in the JSON. */
    return std::unique_ptr<JoinStatement>();
  }
}
