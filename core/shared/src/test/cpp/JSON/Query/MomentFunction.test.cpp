/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* platform-specific includes */

/* c/c++ standard includes */
#include <fstream>
#include <string>

/* third-party library includes */
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

/* project-specific includes */

/* same module include */
#include <JSON/Query/MomentFunction.h>

TEST(MomentFunction, InitializationWithStudy) {
  const std::string initString = R"({
    "type": "MomentFunction",
    "bits_of_precision": 5,
    "col": {
      "vertical": 0,
      "columnName": "math"
    },
    "momentType": "COUNT",
    "revealCount": true
  })";
  const nlohmann::json initJson = nlohmann::json::parse(initString);

  std::ifstream studyFile(
      "../../../../../shared/src/test/cfgs/study.json");
  if (!studyFile.is_open()) {
    struct CannotOpenFileException : std::exception {
      const char * what() const noexcept override {
        return "Study file cannot be opened";
      }
    };
    throw CannotOpenFileException();
  }
  const nlohmann::json studyJson = nlohmann::json::parse(studyFile);
  safrn::StudyConfig study = safrn::readStudyFromJson(studyJson);

  safrn::MomentFunction target(study, initJson);

  EXPECT_EQ(target.col.vertical, 0);
  EXPECT_EQ(target.col.column, 4);
  EXPECT_EQ(target.momentType.value, safrn::MomentType::Enum_t::COUNT);
  EXPECT_EQ(target.revealCount, true);
}
TEST(MomentFunction, Initialization) {
  const std::string initString = R"({
    "type": "MomentFunction",
    "bits_of_precision": 5,
    "col": {
      "vertical": 1,
      "columnIndex": 2
    },
    "momentType": "COUNT",
    "revealCount": true
  })";
  const nlohmann::json initJson = nlohmann::json::parse(initString);

  safrn::MomentFunction target(initJson);

  EXPECT_EQ(target.col.vertical, 1);
  EXPECT_EQ(target.col.column, 2);
  EXPECT_EQ(target.momentType.value, safrn::MomentType::Enum_t::COUNT);
  EXPECT_EQ(target.revealCount, true);
}
