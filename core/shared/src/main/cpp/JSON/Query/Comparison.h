/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 *
 * DESCRIPTION: Class used to describe various
 * inequality operations that produce a boolean
 * result from a database value and a literal.
 */

#ifndef ED428CED68DE4D2FA0B6DD70C3466113
#define ED428CED68DE4D2FA0B6DD70C3466113

/* platform-specific includes */

/* c/c++ standard includes */
#include <string>

/* third-party library includes */

/* project-specific includes */

namespace safrn {

class Comparison {
public:
  explicit Comparison(const std::string & string);

  enum class Enum_t { LT, LTE, GT, GTE, EQ, NEQ };
  const Enum_t value;

private:
  static Enum_t ValueFromString(const std::string & string);
};

} // namespace safrn

#endif
