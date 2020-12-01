/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* platform-specific includes */

/* c/c++ standard includes */
#include <map>
#include <string>

/* third-party library includes */
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

/* project-specific includes */
#include <JSON/Query/JoinType.h>

/* same module include */

TEST(JoinType, CaseInsensitivity) {
  const std::map<std::string, safrn::JoinType::Enum_t> SubsetOfValues =
      {{R"("FULL_OUTER")", safrn::JoinType::Enum_t::FULL_OUTER},
       {R"("full_outer")", safrn::JoinType::Enum_t::FULL_OUTER},
       {R"("INNER")", safrn::JoinType::Enum_t::INNER},
       {R"("inner")", safrn::JoinType::Enum_t::INNER}};

  for (const auto & value : SubsetOfValues) {
    const nlohmann::json initJson = nlohmann::json::parse(value.first);
    safrn::JoinType target(initJson);

    EXPECT_EQ(target.value, value.second);
  }
}

TEST(JoinType, ExhaustiveValues) {
  const std::map<std::string, safrn::JoinType::Enum_t> SubsetOfValues =
      {{R"("inner")", safrn::JoinType::Enum_t::INNER},
       {R"("full_outer")", safrn::JoinType::Enum_t::FULL_OUTER},
       {R"("left")", safrn::JoinType::Enum_t::LEFT},
       {R"("right")", safrn::JoinType::Enum_t::RIGHT}};

  for (const auto & value : SubsetOfValues) {
    const nlohmann::json initJson = nlohmann::json::parse(value.first);
    safrn::JoinType target(initJson);

    EXPECT_EQ(target.value, value.second);
  }
}
