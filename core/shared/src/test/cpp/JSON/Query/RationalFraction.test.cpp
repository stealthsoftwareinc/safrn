/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* platform-specific includes */

/* c/c++ standard includes */

/* third-party library includes */
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

/* project-specific includes */

/* same module include */
#include <JSON/Query/RationalFraction.h>

TEST(RationalFraction, FractionInitialization) {
  const std::string initString = R"("1/2")";
  const nlohmann::json initJson = nlohmann::json::parse(initString);

  safrn::RationalFraction target(initJson);

  EXPECT_EQ(target.numerator, 1);
  EXPECT_EQ(target.denominator, 2);
}

TEST(RationalFraction, IntegerInitialization) {
  const std::string initString = R"("5")";
  const nlohmann::json initJson = nlohmann::json::parse(initString);

  safrn::RationalFraction target(initJson);

  EXPECT_EQ(target.numerator, 5);
  EXPECT_EQ(target.denominator, 1);
}
