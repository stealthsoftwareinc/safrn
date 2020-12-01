/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* platform-specific includes */

/* c/c++ standard includes */

/* third-party library includes */

/* project-specific includes */

/* same module include */
#include <JSON/Query/GCDReducedFraction.h>

intmax_t
safrn::GCDReducedFraction::calculateGCD(intmax_t a, intmax_t b) {

  while (a != 0) {
    auto c = a;
    a = b % a;
    b = c;
  }

  return b;
}

safrn::GCDReducedFraction::GCDReducedFraction(
    intmax_t numerator, intmax_t denominator) :
    gcd(calculateGCD(numerator, denominator)),
    numerator(numerator / gcd),
    denominator(denominator / gcd) {
}
