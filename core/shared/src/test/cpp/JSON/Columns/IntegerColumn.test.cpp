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
#include <JSON/Columns/IntegerColumn.h>

TEST(IntegerColumn, Initialization) {
  const std::string initString = R"({"name": "integer column",
                                     "columnIndex": 1,
                                     "type": "integer",
                                     "signed": true,
                                     "bits": 10})";
  const nlohmann::json initJson = nlohmann::json::parse(initString);
  safrn::IntegerColumn target(initJson);

  EXPECT_EQ(target.name, "integer column");
  EXPECT_EQ(target.index, 1);
  EXPECT_EQ(target.isSigned, true);
  EXPECT_EQ(target.bits, 10);
}
