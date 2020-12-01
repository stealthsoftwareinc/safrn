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
#include <JSON/Columns/CategoricalColumn.h>

TEST(CategoricalColumn, Initialization) {
  const std::string initString = R"({"name": "columnname",
                                     "columnIndex": 1,
                                     "type": "categorical",
                                     "categorical": "categoricalname"})";
  const nlohmann::json initJson = nlohmann::json::parse(initString);
  safrn::CategoricalColumn target(initJson);

  EXPECT_EQ(target.name, "columnname");
  EXPECT_EQ(target.index, 1);
  EXPECT_EQ(target.categoricalName, "categoricalname");
}
