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
#include <JSON/Query/MomentType.h>

safrn::MomentType::MomentType(const nlohmann::json & json) :
    value(valueFromString(json)) {
}

safrn::MomentType::Enum_t
safrn::MomentType::valueFromString(const std::string & string) {
  static const std::map<std::string, Enum_t> typeMap = {
      {"count", Enum_t::COUNT},
      {"mean", Enum_t::MEAN},
      {"variance", Enum_t::VARIANCE},
      {"skew", Enum_t::SKEW},
      {"kurtosis", Enum_t::KURTOSIS}};

  std::string lowercaseString = string;
  std::transform(
      lowercaseString.begin(),
      lowercaseString.end(),
      lowercaseString.begin(),
      tolower);
  return typeMap.at(lowercaseString);
}
