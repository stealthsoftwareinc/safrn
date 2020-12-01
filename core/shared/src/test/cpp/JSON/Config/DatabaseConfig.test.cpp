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
#include <JSON/Config/DatabaseConfig.h>

TEST(DatabaseConfig, Initialization) {
  std::ifstream queryFile(
      "../../../../../shared/src/test/cfgs/database.json");

  if (!queryFile.is_open()) {
    struct CannotOpenFileException : std::exception {
      const char * what() const noexcept override {
        return "File cannot be opened";
      }
    };

    throw CannotOpenFileException();
  }

  const nlohmann::json initJson = nlohmann::json::parse(queryFile);

  safrn::DatabaseConfig target(initJson);

  EXPECT_EQ(target.columnMaps.size(), 2);
  EXPECT_EQ(target.columnMaps[0].localName, "SSN");
  EXPECT_EQ(target.columnMaps[0].localSQLType, 4);
  EXPECT_EQ(target.columnMaps[0].verticalIndex, 1);
  EXPECT_EQ(target.columnMaps[0].columnIndex, 2);
  EXPECT_EQ(target.columnMaps[1].localName, "age");
  EXPECT_EQ(target.columnMaps[1].localSQLType, 4);
  EXPECT_EQ(target.columnMaps[1].verticalIndex, 3);
  EXPECT_EQ(target.columnMaps[1].columnIndex, 4);
  EXPECT_EQ(target.databaseSettings.driverName, "PostgreSQL Unicode");
  EXPECT_EQ(target.databaseSettings.userID, "ODBCTest");
  EXPECT_EQ(target.databaseSettings.password, "ODBCTest");
  EXPECT_EQ(target.databaseSettings.additionalParameters.size(), 3);
  EXPECT_EQ(
      target.databaseSettings.additionalParameters[0],
      "Server=127.0.0.1");
  EXPECT_EQ(
      target.databaseSettings.additionalParameters[1], "Port=5432");
  EXPECT_EQ(
      target.databaseSettings.additionalParameters[2],
      "Database=ODBCTest");
}
