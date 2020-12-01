/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 *
 * DESCRIPTION: Base class for functions used
 * by the SAFRN query language and objects.
 */

#ifndef INC_65BC00794A0A4C3BA4C627C3887A6BB1
#define INC_65BC00794A0A4C3BA4C627C3887A6BB1

/* platform-specific includes */

/* c/c++ standard includes */

/* third-party library includes */

/* project-specific includes */

namespace safrn {

// Supported FUNCTIONS.
enum class FunctionType {
  MOMENT,
  ORDER,
  LIN_REGRESSION,
};

class SafrnFunction {
public:
  explicit SafrnFunction(FunctionType type);

  const FunctionType type;
};

} // namespace safrn

#endif
