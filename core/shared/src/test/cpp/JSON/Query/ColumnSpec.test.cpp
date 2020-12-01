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
#include <JSON/Config/StudyConfig.h>
#include <JSON/Query/ColumnSpec.h>

/* same module include */

TEST(ColumnSpec, Initialization) {
  const std::string initString = R"({"vertical": 1,
                                     "columnIndex": 2})";
  const nlohmann::json initJson = nlohmann::json::parse(initString);
  safrn::ColumnSpec target(initJson);

  EXPECT_EQ(target.vertical, 1);
  EXPECT_EQ(target.column, 2);
}

TEST(ColumnSpec, WithStudyConfigWithName) {
  const std::string initString = R"({"vertical": 1,
                                     "columnName": "income",
                                     "columnIndex": 1})";
  const nlohmann::json initJson = nlohmann::json::parse(initString);

  // Read in a sample study.config.
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

  safrn::ColumnSpec target(study, initJson);

  EXPECT_EQ(target.vertical, 1);
  EXPECT_EQ(target.column, 1);
}

TEST(ColumnSpec, WithStudyConfigNoColumn) {
  const std::string initString = R"({"vertical": 1,
                                     "columnName": "income"})";
  const nlohmann::json initJson = nlohmann::json::parse(initString);

  // Read in a sample study.config.
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

  safrn::ColumnSpec target(study, initJson);

  EXPECT_EQ(target.vertical, 1);
  EXPECT_EQ(target.column, 1);
}
