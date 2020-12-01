/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

#ifndef INC_7B2C8C7E1DE04A5886BD1072C3D72F49
#define INC_7B2C8C7E1DE04A5886BD1072C3D72F49

/* platform-specific includes */

/* c/c++ standard includes */

/* third-party library includes */
#include <nlohmann/json.hpp>

/* project-specific includes */

namespace safrn {

class JoinType {
public:
  explicit JoinType(const nlohmann::json & json);

  enum class Enum_t { INNER, FULL_OUTER, LEFT, RIGHT };

  const Enum_t value;

private:
  static Enum_t joinTypeFromString(const std::string & string);
};

} // namespace safrn

#endif
