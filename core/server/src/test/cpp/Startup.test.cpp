/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

#include <cmath>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include <mpc/ObservationList.h>

#include <dataowner/fortissimo.h>
#include <framework/Framework.h>
#include <framework/TestRunner.h>

#include <JSON/Config/StudyConfig.h>
#include <JSON/Query/Query.h>

#include <Startup.h>
#include <StartupUtils.h>

/* logging config */
#include <ff/logging.h>

using namespace safrn;

TEST(Startup, double_convert) {
  double value = 3.1415926535;

  dataowner::LargeNum modulus = (dataowner::LargeNum(1) << 61) - 1;
  dataowner::LargeNum converted_value =
      convertDoubleToLargeNum(value, 32, modulus);

  log_debug(
      "converted_value: %s", ff::mpc::dec(converted_value).c_str());
  //dataowner::LargeNum diff = (dataowner::LargeNum(1) << 29) - converted_value;
  //int64_t cast_diff = static_cast<int64_t>(diff);
  //cast_diff = abs(cast_diff);

  EXPECT_EQ(converted_value, dataowner::LargeNum(13493037704));
}

TEST(Startup, readCSV_working) {
  std::string csvFile(
      "../../../../../server/src/test/data/readCSV_working.csv");
  std::vector<size_t> keyCols = {{0, 1}};
  std::vector<size_t> plCols = {{2, 3, 4}};

  StudyConfig scfg;

  scfg.lexicon.emplace_back();
  scfg.lexicon[0].verticalIndex = 0;
  nlohmann::json j;
  j["type"] = "real";

  j["columnIndex"] = 0;
  j["name"] = "key0";
  scfg.lexicon[0].columns.emplace_back(new ColumnBase(j));
  j["columnIndex"] = 1;
  j["name"] = "key1";
  scfg.lexicon[0].columns.emplace_back(new ColumnBase(j));
  j["columnIndex"] = 2;
  j["name"] = "pl0";
  scfg.lexicon[0].columns.emplace_back(new ColumnBase(j));
  j["columnIndex"] = 3;
  j["name"] = "pl1";
  scfg.lexicon[0].columns.emplace_back(new ColumnBase(j));
  j["columnIndex"] = 4;
  j["name"] = "pl2";
  scfg.lexicon[0].columns.emplace_back(new ColumnBase(j));

  Identity id("00000000000000000000000000000001", ROLE_DATAOWNER, 0);

  dataowner::SmallNum mod = 65521;
  size_t bitsOfPrecisison = 5;

  ff::mpc::ObservationList<dataowner::LargeNum> olist;

  EXPECT_TRUE(readCSV(
      csvFile,
      olist,
      keyCols,
      plCols,
      4,
      scfg,
      id,
      mod,
      bitsOfPrecisison));

  if_debug {
    for (size_t i = 0; i < olist.elements.size(); i++) {
      for (size_t j = 0; j < olist.numKeyCols; j++) {
        log_info(
            "key[%zu]: %s",
            j,
            ff::mpc::dec(olist.elements[i].keyCols[j]).c_str());
      }
      for (size_t j = 0; j < olist.numArithmeticPayloadCols; j++) {
        log_info(
            "payload[%zu]: %s",
            j,
            ff::mpc::dec(olist.elements[i].arithmeticPayloadCols[j])
                .c_str());
      }
    }
  }

  EXPECT_EQ(3, olist.numKeyCols);
  EXPECT_EQ(3, olist.numArithmeticPayloadCols);
  EXPECT_EQ(0, olist.numXORPayloadCols);
  EXPECT_EQ(2, olist.elements.size());

  EXPECT_EQ(dataowner::LargeNum(123), olist.elements[0].keyCols[0]);
  EXPECT_EQ(dataowner::LargeNum(456), olist.elements[0].keyCols[1]);
  EXPECT_EQ(dataowner::LargeNum(0), olist.elements[0].keyCols[2]);

  EXPECT_EQ(
      dataowner::LargeNum(4),
      olist.elements[0].arithmeticPayloadCols[0]);
  EXPECT_EQ(
      dataowner::LargeNum(7),
      olist.elements[0].arithmeticPayloadCols[1]);
  EXPECT_EQ(
      dataowner::LargeNum(353),
      olist.elements[0].arithmeticPayloadCols[2]);

  EXPECT_EQ(dataowner::LargeNum(234), olist.elements[1].keyCols[0]);
  EXPECT_EQ(dataowner::LargeNum(567), olist.elements[1].keyCols[1]);
  EXPECT_EQ(dataowner::LargeNum(0), olist.elements[1].keyCols[2]);

  EXPECT_EQ(
      dataowner::LargeNum(15),
      olist.elements[1].arithmeticPayloadCols[0]);
  EXPECT_EQ(
      dataowner::LargeNum(18),
      olist.elements[1].arithmeticPayloadCols[1]);
  EXPECT_EQ(
      dataowner::LargeNum(694),
      olist.elements[1].arithmeticPayloadCols[2]);
}
