/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* platform-specific includes */

/* c/c++ standard includes */
#include <algorithm>
#include <map>
#include <string>

/* third-party library includes */

/* project-specific includes */

/* same module include */
#include <JSON/Query/JoinType.h>

safrn::JoinType::JoinType(const nlohmann::json & json) :
    value(joinTypeFromString(json)) {
}

safrn::JoinType::Enum_t
safrn::JoinType::joinTypeFromString(const std::string & string) {
  static const std::map<std::string, Enum_t> joinTypeMap = {
      {"inner", Enum_t::INNER},
      {"full_outer", Enum_t::FULL_OUTER},
      {"left", Enum_t::LEFT},
      {"right", Enum_t::RIGHT}};

  std::string lowercaseString = string;
  std::transform(
      lowercaseString.begin(),
      lowercaseString.end(),
      lowercaseString.begin(),
      tolower);
  return joinTypeMap.at(lowercaseString);
}
