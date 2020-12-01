/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 *
 * DESCRIPTION: Initialize and store coefficient
 * needed for rational multiplication factors without
 * using floating point.
 */

#ifndef INC_956F878EFAB04829AD8E5A7ACB3F0CC7
#define INC_956F878EFAB04829AD8E5A7ACB3F0CC7

/* platform-specific includes */

/* c/c++ standard includes */
#include <cstdint>
#include <ratio>

/* third-party library includes */
#include <nlohmann/json.hpp>

/* project-specific includes */
#include <JSON/Query/GCDReducedFraction.h>

namespace safrn {

class RationalFraction : public safrn::GCDReducedFraction {
public:
  explicit RationalFraction(const nlohmann::json & json);

  /* operators */
  bool operator==(const intmax_t & literal) const;
  bool operator==(const RationalFraction & literal) const;

private:
  static constexpr auto FRACTION_DELIMITER = "/";
  static constexpr auto DEFAULT_FRACTION_TOKEN = "1";
};

} // namespace safrn

#endif
