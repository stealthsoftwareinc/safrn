/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* platform-specific includes */

/* c/c++ standard includes */
#include <string>
#include <vector>

/* third-party library includes */
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

/* project-specific includes */
#include <JSON/Query/Comparison.h>
#include <Util/Utils.h>

/* same module include */

TEST(Comparison, ExhaustiveValues) {

  /* test a subset of values to ensure
   * a change in the provided string will yield
   * a change in the result. */
  const std::vector<std::pair<std::string, safrn::Comparison::Enum_t>>
      AllPossibleValues = {{R"("<")", safrn::Comparison::Enum_t::LT},
                           {R"("<=")", safrn::Comparison::Enum_t::LTE},
                           {R"(">")", safrn::Comparison::Enum_t::GT},
                           {R"(">=")", safrn::Comparison::Enum_t::GTE},
                           {R"("=")", safrn::Comparison::Enum_t::EQ},
                           {R"("!=")", safrn::Comparison::Enum_t::NEQ}};

  for (const auto & value : AllPossibleValues) {
    const nlohmann::json jsonInit = nlohmann::json::parse(value.first);
    safrn::Comparison target(safrn::json_string(jsonInit));

    EXPECT_EQ(target.value, value.second);
  }
}
