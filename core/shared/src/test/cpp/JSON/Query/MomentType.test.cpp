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
#include <JSON/Query/MomentType.h>

/* same module include */

TEST(MomentType, CaseInsensitivity) {
  const std::map<std::string, safrn::MomentType::Enum_t>
      SubsetOfValues = {
          {R"("COUNT")", safrn::MomentType::Enum_t::COUNT},
          {R"("count")", safrn::MomentType::Enum_t::COUNT}};

  for (const auto & value : SubsetOfValues) {
    const nlohmann::json jsonInit = nlohmann::json::parse(value.first);
    safrn::MomentType target(jsonInit);

    EXPECT_EQ(target.value, value.second);
  }
}

TEST(MomentType, ExhaustiveValues) {
  const std::map<std::string, safrn::MomentType::Enum_t>
      SubsetOfValues = {
          {R"("count")", safrn::MomentType::Enum_t::COUNT},
          {R"("mean")", safrn::MomentType::Enum_t::MEAN},
          {R"("variance")", safrn::MomentType::Enum_t::VARIANCE},
          {R"("skew")", safrn::MomentType::Enum_t::SKEW},
          {R"("kurtosis")", safrn::MomentType::Enum_t::KURTOSIS}};

  for (const auto & value : SubsetOfValues) {
    const nlohmann::json jsonInit = nlohmann::json::parse(value.first);
    safrn::MomentType target(jsonInit);

    EXPECT_EQ(target.value, value.second);
  }
}
