/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* platform-specific includes */

/* c/c++ standard includes */
#include <algorithm>
#include <map>

/* third-party library includes */

/* project-specific includes */

/* same module include */
#include <JSON/Query/Comparison.h>

safrn::Comparison::Comparison(const std::string & string) :
    value(ValueFromString(string)) {
}

safrn::Comparison::Enum_t
safrn::Comparison::ValueFromString(const std::string & string) {
  const std::map<std::string, Enum_t> NameTranslation = {
      {"<", Enum_t::LT},
      {"<=", Enum_t::LTE},
      {">", Enum_t::GT},
      {">=", Enum_t::GTE},
      {"=", Enum_t::EQ},
      {"!=", Enum_t::NEQ}};

  return NameTranslation.at(string);
}
