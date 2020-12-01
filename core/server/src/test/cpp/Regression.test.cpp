/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

#include <vector>

#include <gtest/gtest.h>

#include <QueryTester.h>

using namespace safrn;

TEST(Regression, no_intercept_4_parties) {
  std::vector<double> res;
  EXPECT_TRUE(
      testQuery("regression_no_intercept.json", res, TEST_4_PARTY));
}

TEST(Regression, intercept_4_parties) {
  std::vector<double> res;
  EXPECT_TRUE(
      testQuery("regression_intercept.json", res, TEST_4_PARTY));
}

TEST(Regression, no_intercept_2_parties) {
  std::vector<double> res;
  EXPECT_TRUE(
      testQuery("regression_no_intercept.json", res, TEST_2_PARTY));
}

TEST(Regression, intercept_2_parties) {
  std::vector<double> res;
  EXPECT_TRUE(
      testQuery("regression_intercept.json", res, TEST_2_PARTY));
}

TEST(Regression, no_intercept_7_parties) {
  std::vector<double> res;
  EXPECT_TRUE(
      testQuery("regression_no_intercept.json", res, TEST_7_PARTY));
}

TEST(Regression, intercept_7_parties) {
  std::vector<double> res;
  EXPECT_TRUE(
      testQuery("regression_intercept.json", res, TEST_7_PARTY));
}
