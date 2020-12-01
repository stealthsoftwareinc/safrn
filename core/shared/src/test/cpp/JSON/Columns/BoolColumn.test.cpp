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
#include <JSON/Columns/BoolColumn.h>

TEST(BoolColumn, Initialization) {
  const std::string initString = R"({"name": "test",
                                     "type": "bool",
                                     "columnIndex": 1})";
  const nlohmann::json initJson = nlohmann::json::parse(initString);
  safrn::BoolColumn target(initJson);

  EXPECT_EQ(target.isSigned, false);
  EXPECT_EQ(target.bits, 1);
  EXPECT_EQ(target.name, "test");
  EXPECT_EQ(target.index, 1);
}
