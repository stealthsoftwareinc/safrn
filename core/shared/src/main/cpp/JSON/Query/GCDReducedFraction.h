/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 *
 * DESCRIPTION: Class which stores and automatically
 * reduces the numerator and denominator of a fraction
 * into their smallest possible representations.
 */

#ifndef INC_730ED905CC4340B68427782526F6C56F
#define INC_730ED905CC4340B68427782526F6C56F

/* platform-specific includes */

/* c/c++ standard includes */
#include <cstdint>

/* third-party library includes */

/* project-specific includes */

namespace safrn {

class GCDReducedFraction {
public:
  GCDReducedFraction(
      const intmax_t numerator, const intmax_t denominator);

  const intmax_t gcd;
  const intmax_t numerator;
  const intmax_t denominator;

private:
  static intmax_t calculateGCD(const intmax_t A, const intmax_t B);
};

} // namespace safrn

#endif
