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
#include <JSON/Query/FilterTerm.h>

TEST(FilterTerm, Initialization) {
  const std::string initString = R"({"left": [{"coefficient": 1,
                                      "values": []}],
                                     "right": 1,
                                     "comp": "<"})";
  const nlohmann::json initJson = nlohmann::json::parse(initString);

  safrn::FilterTerm target(initJson);

  EXPECT_EQ(target.right, 1);
}
