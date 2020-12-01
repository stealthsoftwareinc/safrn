/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* platform-specific includes */

/* c/c++ standard includes */

/* third-party library includes */

/* project-specific includes */

/* same module include */
#include <JSON/Query/JoinStatement.h>

safrn::JoinStatement::JoinStatement(const nlohmann::json & json) :
    type(json["type"]),
    joinOns(JoinStatement::jsonToJoinOns(json["joinOns"])) {
}

safrn::JoinStatement::JoinStatement(
    const safrn::StudyConfig & study, const nlohmann::json & json) :
    type(json["type"]),
    joinOns(JoinStatement::jsonToJoinOns(study, json["joinOns"])) {
}

std::vector<std::pair<safrn::JoinOn, safrn::JoinOn>>
safrn::JoinStatement::jsonToJoinOns(const nlohmann::json & arrayJson) {

  if (arrayJson.empty()) {
    throw NotEnoughJoinOns();
  }

  std::vector<std::pair<JoinOn, JoinOn>> result;
  for (const auto & jsonItem : arrayJson) {
    result.emplace_back(std::pair<JoinOn, JoinOn>(
        jsonItem["first"], jsonItem["second"]));
  }

  return result;
}

std::vector<std::pair<safrn::JoinOn, safrn::JoinOn>>
safrn::JoinStatement::jsonToJoinOns(
    const safrn::StudyConfig & study,
    const nlohmann::json & arrayJson) {

  if (arrayJson.empty()) {
    throw NotEnoughJoinOns();
  }

  std::vector<std::pair<JoinOn, JoinOn>> result;
  for (const auto & jsonItem : arrayJson) {
    JoinOn first(study, jsonItem["first"]);
    JoinOn second(study, jsonItem["second"]);
    result.emplace_back(std::pair<JoinOn, JoinOn>(first, second));
  }

  return result;
}
