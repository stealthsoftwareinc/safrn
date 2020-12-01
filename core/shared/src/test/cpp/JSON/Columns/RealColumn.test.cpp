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
#include <JSON/Columns/RealColumn.h>

TEST(RealColumn, Initialization) {
  const std::string initString = R"({"name": "real column",
                                     "columnIndex": 1,
                                     "type": "real",
                                     "precision": 5,
                                     "scale": 10})";
  const nlohmann::json initJson = nlohmann::json::parse(initString);
  safrn::RealColumn target(initJson);

  EXPECT_EQ(target.name, "real column");
  EXPECT_EQ(target.index, 1);
  EXPECT_EQ(target.precision, 5);
  EXPECT_EQ(target.scale, 10);
}
