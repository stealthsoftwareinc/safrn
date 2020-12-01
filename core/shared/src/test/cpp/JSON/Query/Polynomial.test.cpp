/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* platform-specific includes */

/* c/c++ standard includes */
#include <string>

/* third-party library includes */
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

/* project-specific includes */
#include <JSON/Query/Polynomial.h>

/* same module include */

TEST(Polynomial, IdentityInitialization) {
  const std::string initString = R"([0,1])";
  const nlohmann::json initJson = nlohmann::json::parse(initString);

  safrn::Polynomial<int> target(initJson);

  EXPECT_EQ(target.IsIdentity(), true);
}

TEST(Polynomial, NonIdentityInitialization) {
  const std::string initString = R"([5,10,20])";
  const nlohmann::json initJson = nlohmann::json::parse(initString);

  safrn::Polynomial<int> target(initJson);
  EXPECT_EQ(target.coefficients[0], 5);
  EXPECT_EQ(target.coefficients[1], 10);
  EXPECT_EQ(target.coefficients[2], 20);
}

TEST(Polynomial, NotEnoughCoefficients) {
  const std::string initString = R"([])";
  const nlohmann::json initJson = nlohmann::json::parse(initString);

  EXPECT_THROW(
      safrn::Polynomial<int> target(initJson),
      safrn::Polynomial<int>::NotEnoughCoefficients);
}

TEST(Polynomial, LastElementZero) {
  const std::string initString = R"([0])";
  const nlohmann::json initJson = nlohmann::json::parse(initString);

  EXPECT_THROW(
      safrn::Polynomial<int> target(initJson),
      safrn::Polynomial<int>::LastElementZero);
}
