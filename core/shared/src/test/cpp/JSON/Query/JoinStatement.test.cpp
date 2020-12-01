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
#include <JSON/Query/JoinStatement.h>

TEST(JoinStatement, Initialization) {
  const std::string initString = R"(
    {"type": "INNER",
     "joinOns": [ {
     "first": {
        "col": {
          "vertical": 1,
          "columnIndex": 2
        },
        "formula": [
          "3"
        ]
      },
     "second": {
        "col": {
          "vertical": 4,
          "columnIndex": 5
        },
        "formula": [
          "6"
        ]
      }
    }]})";
  const nlohmann::json initJson = nlohmann::json::parse(initString);

  safrn::JoinStatement target(initJson);

  EXPECT_EQ(target.type.value, safrn::JoinType::Enum_t::INNER);
  EXPECT_EQ(target.joinOns.size(), 1);
  EXPECT_EQ(target.joinOns.at(0).first.col.vertical, 1);
  EXPECT_EQ(target.joinOns.at(0).first.col.column, 2);
  EXPECT_EQ(target.joinOns.at(0).first.formula.coefficients.at(0), 3);
  EXPECT_EQ(target.joinOns.at(0).second.col.vertical, 4);
  EXPECT_EQ(target.joinOns.at(0).second.col.column, 5);
  EXPECT_EQ(target.joinOns.at(0).second.formula.coefficients.at(0), 6);
}

TEST(JoinStatement, NotEnoughJoinOns) {
  const std::string initString = R"(
    {"type": "INNER",
     "joinOns": []})";
  const nlohmann::json initJson = nlohmann::json::parse(initString);

  EXPECT_THROW(
      safrn::JoinStatement target(initJson),
      safrn::JoinStatement::NotEnoughJoinOns);
}
