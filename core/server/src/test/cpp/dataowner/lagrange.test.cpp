/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* C and POSIX Headers */

/* C++ Headers */
//#include <map>
//#include <memory>

/* 3rd Party Headers */
#include <gtest/gtest.h>
#include <openssl/rand.h>

/* SAFRN Headers */
#include <dataowner/lagrange.h>

using namespace safrn;
using namespace dataowner;

uint32_t evaluate(Polynomial_t & coeffs, uint32_t val, uint32_t s);

TEST(LagrangeTester, modular_inversion_mod_11) {
  uint32_t s = 11;
  for (uint32_t i = 1; i < s; i++) {
    EXPECT_EQ(1, (i * invert(s, i) % s));
  }
}

TEST(LagrangeTester, modular_inversion_mod_65521) {
  uint32_t s = 65521;
  for (uint32_t i = 1; i < s; i++) {
    EXPECT_EQ(1, (i * invert(s, i) % s));
  }
}

TEST(LagrangeTester, product_x_minus_i_small) {
  uint32_t s = 11;
  uint32_t ell = 4;

  Polynomial_t true_coeffs = Polynomial_t(6);
  true_coeffs[0] = 1;
  true_coeffs[1] = 10;
  true_coeffs[2] = 6;
  true_coeffs[3] = 8;
  true_coeffs[4] = 7;
  true_coeffs[5] = 1;
  Polynomial_t coeffs;
  computeProductOneToEllPlusOne(s, ell, coeffs);
  for (uint32_t i = 0; i < ell + 2; i++) {
    EXPECT_EQ(true_coeffs[i], coeffs[i]);
  }
}

TEST(LagrangeTester, lagrange_poly_mod_11) {
  uint32_t s = 11;
  uint32_t ell = 7;
  Polynomial_t coeffs = computeLagrangeCoeffsForPrefixOr(s, ell);

  EXPECT_EQ(0, evaluate(coeffs, 1, s));
  for (uint32_t i = 2; i < ell + 1; i++) {
    EXPECT_EQ(1, evaluate(coeffs, i, s));
  }
}

TEST(LagrangeTester, lagrange_poly_mod_509) {
  uint32_t s = 509;
  uint32_t ell = 500;
  Polynomial_t coeffs = computeLagrangeCoeffsForPrefixOr(s, ell);

  EXPECT_EQ(0, evaluate(coeffs, 1, s));
  for (uint32_t i = 2; i < ell + 1; i++) {
    EXPECT_EQ(1, evaluate(coeffs, i, s));
  }
}

// s a prime
uint32_t evaluate(Polynomial_t & coeffs, uint32_t val, uint32_t s) {
  uint32_t ans = coeffs[0];
  uint32_t val_pow = 1;
  for (uint32_t i = 1; i < coeffs.size(); i++) {
    val_pow *= val;
    val_pow %= s;
    uint32_t t = (val_pow * coeffs[i]) % s;
    ans += t;
    ans %= s;
  }
  return ans;
}
