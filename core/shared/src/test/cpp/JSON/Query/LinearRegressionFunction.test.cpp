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
#include <JSON/Query/LinearRegressionFunction.h>

TEST(LinearRegressionFunction, InitializationWithStudy) {
  const std::string initString = R"({
      "type": "LinearRegressionFunction",
      "fit_intercept": false,
      "table_cell_bytes": 4,
      "num_table_rows": 1000,
      "bits_of_precision": 5,
      "dep_var": {
        "vertical": 5,
        "columnIndex": 2
      },
      "indep_vars": [
        {
          "vertical": 0,
          "columnName": "math"
        },
        {
          "vertical": 5,
          "columnIndex": 6
        }
      ]
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

  safrn::LinearRegressionFunction target(study, initJson);

  EXPECT_EQ(target.dep_var.vertical, 5);
  EXPECT_EQ(target.dep_var.column, 2);
  EXPECT_EQ(target.indep_vars.size(), 2);
  EXPECT_EQ(target.indep_vars.at(0).vertical, 0);
  EXPECT_EQ(target.indep_vars.at(0).column, 4);
  EXPECT_EQ(target.indep_vars.at(1).vertical, 5);
  EXPECT_EQ(target.indep_vars.at(1).column, 6);
}

TEST(LinearRegressionFunction, Initialization) {
  const std::string initString = R"({
      "type": "LinearRegressionFunction",
      "fit_intercept": false,
      "table_cell_bytes": 4,
      "num_table_rows": 1000,
      "bits_of_precision": 5,
      "dep_var": {
        "vertical": 1,
        "columnIndex": 2
      },
      "indep_vars": [
        {
          "vertical": 3,
          "columnIndex": 4
        },
        {
          "vertical": 5,
          "columnIndex": 6
        }
      ]
    })";
  const nlohmann::json initJson = nlohmann::json::parse(initString);

  safrn::LinearRegressionFunction target(initJson);

  EXPECT_EQ(target.dep_var.vertical, 1);
  EXPECT_EQ(target.dep_var.column, 2);
  EXPECT_EQ(target.indep_vars.size(), 2);
  EXPECT_EQ(target.indep_vars.at(0).vertical, 3);
  EXPECT_EQ(target.indep_vars.at(0).column, 4);
  EXPECT_EQ(target.indep_vars.at(1).vertical, 5);
  EXPECT_EQ(target.indep_vars.at(1).column, 6);
}

TEST(LinearRegressionFunction, NotEnoughIndepVars) {
  const std::string initString = R"({
      "type": "LinearRegressionFunction",
      "fit_intercept": true,
      "table_cell_bytes": 4,
      "num_table_rows": 1000,
      "bits_of_precision": 5,
      "dep_var": {
        "vertical": 1,
        "columnIndex": 2
      },
      "indep_vars": []
    })";
  const nlohmann::json initJson = nlohmann::json::parse(initString);

  EXPECT_THROW(
      safrn::LinearRegressionFunction target(initJson),
      safrn::LinearRegressionFunction::NotEnoughIndepVars);
}
