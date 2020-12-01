/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

#ifndef INC_9C96C3E31B2844F59EDC78AF0689BBF2
#define INC_9C96C3E31B2844F59EDC78AF0689BBF2

/* platform-specific includes */

/* c/c++ standard includes */
#include <string>

/* third-party library includes */
#include <nlohmann/json.hpp>

/* project-specific includes */

namespace safrn {

class MomentType {
public:
  explicit MomentType(const nlohmann::json & json);

  enum class Enum_t { COUNT, MEAN, VARIANCE, SKEW, KURTOSIS };

  const Enum_t value;

private:
  static Enum_t valueFromString(const std::string & string);
};

} // namespace safrn

#endif
