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

/* same module include */
#include <JSON/Query/FilterMultinomial.h>

TEST(FilterMultinomial, NotEnoughTerms) {
  const std::string initString = R"([])";
  const nlohmann::json initJson = nlohmann::json::parse(initString);

  EXPECT_THROW(
      safrn::FilterMultinomial target(initJson),
      safrn::FilterMultinomial::NotEnoughTerms);
}

TEST(FilterMultinomial, PopulatedInitialization) {
  const std::string initString = R"([{"coefficient": 1,
                                      "values": []}])";
  const nlohmann::json initJson = nlohmann::json::parse(initString);

  safrn::FilterMultinomial target(initJson);

  EXPECT_EQ(target.terms.size(), 1);
  EXPECT_EQ(target.terms.at(0).coefficient, 1);
  EXPECT_EQ(target.terms.at(0).values.size(), 0);
}
