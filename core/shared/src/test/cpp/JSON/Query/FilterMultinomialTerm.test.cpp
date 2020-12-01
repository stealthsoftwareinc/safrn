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
#include <JSON/Query/FilterMultinomialTerm.h>

TEST(ColumnExponent, Initialization) {
  const std::string initString =
      R"({"col": {"vertical": 1, "columnIndex": 2},
                                     "exp": 3})";
  const nlohmann::json initJson = nlohmann::json::parse(initString);

  safrn::FilterMultinomialTerm::ColumnExponent target(initJson);

  EXPECT_EQ(target.col.vertical, 1);
  EXPECT_EQ(target.col.column, 2);
  EXPECT_EQ(target.exp, 3);
}

TEST(FilterMultinomialTerm, InitializationEmptyValues) {
  const std::string initString = R"({"coefficient": 1,
                                     "values": []})";
  const nlohmann::json initJson = nlohmann::json::parse(initString);

  safrn::FilterMultinomialTerm target(initJson);

  EXPECT_EQ(target.coefficient, 1);
  EXPECT_EQ(target.values.size(), 0);
}

TEST(FilterMultinomialTerm, InitializationPopulatedValues) {
  const std::string initString = R"({"coefficient": 1,
                                     "values": [{"col": {"vertical": 1, "columnIndex": 2},
                                                 "exp": 3}]})";
  const nlohmann::json initJson = nlohmann::json::parse(initString);

  safrn::FilterMultinomialTerm target(initJson);

  EXPECT_EQ(target.coefficient, 1);
  EXPECT_EQ(target.values.size(), 1);
  EXPECT_EQ(target.values.at(0).col.vertical, 1);
  EXPECT_EQ(target.values.at(0).col.column, 2);
  EXPECT_EQ(target.values.at(0).exp, 3);
}
