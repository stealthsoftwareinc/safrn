/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* platform-specific includes */

/* c/c++ standard includes */
#include <fstream>
#include <iostream>
#include <string>

/* third-party library includes */
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

/* project-specific includes */
#include <JSON/Query/Query.h>

/* same module include */

TEST(Query, IntegrationTestForMoment) {
  std::ifstream queryFile(
      "../../../../../shared/src/test/cfgs/moment_query.json");

  if (!queryFile.is_open()) {
    struct CannotOpenFileException : std::exception {
      const char * what() const noexcept override {
        return "File cannot be opened";
      }
    };

    throw CannotOpenFileException();
  }

  const nlohmann::json initJson = nlohmann::json::parse(queryFile);

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

  safrn::Query target(study, initJson);

  /* we don't need to check any fields,
   * we just need to make sure it doesn't
   * fail to construct the object from json. */
}

TEST(Query, IntegrationTestForRegression) {
  std::ifstream queryFile(
      "../../../../../shared/src/test/cfgs/regression_query.json");

  if (!queryFile.is_open()) {
    struct CannotOpenFileException : std::exception {
      const char * what() const noexcept override {
        return "File cannot be opened";
      }
    };

    throw CannotOpenFileException();
  }

  const nlohmann::json initJson = nlohmann::json::parse(queryFile);

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

  safrn::Query target(study, initJson);

  /* we don't need to check any fields,
   * we just need to make sure it doesn't
   * fail to construct the object from json. */
}

TEST(Query, MissingOptionalParameters) {
  const std::string initString =
      R"({
    "prefilters": [
      [
        [
          {
            "left": [
              {
                "coefficient": 1,
                "values": [
                  {
                    "col": {
                      "vertical": 2,
                      "columnIndex": 3
                    },
                    "exp": 4
                  }
                ]
              }
            ],
            "right": 5,
            "comp": "<"
          }
        ]
      ]
    ],
    "function": {
      "type": "OrderFunction",
      "col": {
        "vertical": 14,
        "columnIndex": 15
      },
      "is_percentile": true,
      "lowest_first": true,
      "value": 16
    }
  })";

  const nlohmann::json initJson = nlohmann::json::parse(initString);

  safrn::Query target(initJson);
}

TEST(Query, TooManyPrefilters) {
  const std::string initString =
      R"({
    "prefilters": [
      [
        [
          {
            "left": [
              {
                "coefficient": 1,
                "values": [
                  {
                    "col": {
                      "vertical": 2,
                      "columnIndex": 3
                    },
                    "exp": 4
                  }
                ]
              }
            ],
            "right": 5,
            "comp": "LT"
          }
        ]
      ],
      [
        [
          {
            "left": [
              {
                "coefficient": 1,
                "values": [
                  {
                    "col": {
                      "vertical": 2,
                      "columnIndex": 3
                    },
                    "exp": 4
                  }
                ]
              }
            ],
            "right": 5,
            "comp": "LT"
          }
        ]
      ],
      [
        [
          {
            "left": [
              {
                "coefficient": 1,
                "values": [
                  {
                    "col": {
                      "vertical": 2,
                      "columnIndex": 3
                    },
                    "exp": 4
                  }
                ]
              }
            ],
            "right": 5,
            "comp": "LT"
          }
        ]
      ]
    ],
    "function": {
      "type": "OrderFunction",
      "col": {
        "vertical": 14,
        "columnIndex": 15
      },
      "is_percentile": true,
      "lowest_first": true,
      "value": 16
    }
  })";

  const nlohmann::json initJson = nlohmann::json::parse(initString);

  EXPECT_THROW(
      safrn::Query target(initJson), safrn::Query::TooManyPrefilters);
}
