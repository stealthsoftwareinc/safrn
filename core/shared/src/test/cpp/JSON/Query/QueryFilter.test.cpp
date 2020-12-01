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
#include <JSON/Query/QueryFilter.h>

TEST(QueryFilter, EmptyInitialization) {
  const std::string initString = R"([])";
  const nlohmann::json initJson = nlohmann::json::parse(initString);

  EXPECT_THROW(
      safrn::QueryFilter target(initJson),
      safrn::QueryFilter::NotEnoughExpressions);
}

TEST(QueryFilter, PopulatedInitialization) {
  const std::string initString = R"([[{"left": [{"coefficient": 1,
                                      "values": []}],
                                     "right": 1,
                                     "comp": "<"}]])";
  const nlohmann::json initJson = nlohmann::json::parse(initString);

  safrn::QueryFilter target(initJson);

  EXPECT_EQ(target.expressions.size(), 1);
}
