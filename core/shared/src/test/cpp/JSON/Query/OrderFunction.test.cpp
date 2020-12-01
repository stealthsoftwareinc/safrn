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
#include <JSON/Query/OrderFunction.h>

/* same module include */

TEST(OrderFunction, Initialization) {
  const std::string initString = R"({
    "type": "OrderFunction",
    "col": {
      "vertical": 1,
      "columnIndex": 2
    },
    "is_percentile": true,
    "lowest_first": false,
    "value": 100
  })";
  const nlohmann::json initJson = nlohmann::json::parse(initString);

  safrn::OrderFunction target(initJson);

  EXPECT_EQ(target.col.vertical, 1);
  EXPECT_EQ(target.col.column, 2);
  EXPECT_EQ(target.is_percentile, true);
  EXPECT_EQ(target.lowest_first, false);
  EXPECT_EQ(target.value, 100);
}
