/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* platform-specific includes */

/* c/c++ standard includes */

/* third-party library includes */
#include <gtest/gtest.h>

/* project-specific includes */

/* same module include */
#include <JSON/Query/GCDReducedFraction.h>

TEST(GCDReducedFraction, Initialization) {
  safrn::GCDReducedFraction target(1, 2);

  EXPECT_EQ(target.numerator, 1);
  EXPECT_EQ(target.denominator, 2);
}

TEST(GCDReducedFraction, Reduction) {
  constexpr intmax_t PRIME_NUMERATOR = 5;
  constexpr intmax_t PRIME_DENOMINATOR = 7;
  constexpr intmax_t GCD = 10;
  safrn::GCDReducedFraction target(
      PRIME_NUMERATOR * GCD, PRIME_DENOMINATOR * GCD);

  EXPECT_EQ(target.numerator, PRIME_NUMERATOR);
  EXPECT_EQ(target.denominator, PRIME_DENOMINATOR);
}

TEST(GCDReducedFraction, NegativeReduction) {
  constexpr intmax_t PRIME_NUMERATOR = 5;
  constexpr intmax_t PRIME_DENOMINATOR = 7;
  constexpr intmax_t GCD = -10;

  safrn::GCDReducedFraction target(
      PRIME_NUMERATOR * GCD, PRIME_DENOMINATOR * GCD);

  EXPECT_EQ(target.numerator, PRIME_NUMERATOR);
  EXPECT_EQ(target.denominator, PRIME_DENOMINATOR);
}
