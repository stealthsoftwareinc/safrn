/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

#include <string>

#include <dataowner/lagrange.h>
#include <ff/logging.h>

namespace safrn {
namespace dataowner {

void divideByDegreeOnePolynomial(
    Polynomial_t & coeffs,
    Polynomial_t & overwritten_output,
    uint32_t s,
    uint32_t a,
    uint32_t ell); // divides by (x-a) mod s
uint32_t multiplyDifferentXValues(uint32_t s, uint32_t ell, uint32_t i);

Polynomial_t
computeLagrangeCoeffsForPrefixOr(uint32_t s, uint32_t ell) {
  Polynomial_t ret;
  computeLagrangeCoeffsForPrefixOr(s, ell, ret);
  return ret;
}

void computeLagrangeCoeffsForPrefixOr(
    uint32_t s, uint32_t ell, Polynomial_t & coeffs_to_return) {
  coeffs_to_return = Polynomial_t(ell + 1, 0);

  Polynomial_t coeffs_of_product;
  computeProductOneToEllPlusOne(s, ell, coeffs_of_product);
  Polynomial_t intermediate_term(ell + 1, 0);

  for (uint32_t i = 2; i < ell + 2; i++) {
    divideByDegreeOnePolynomial(
        coeffs_of_product, intermediate_term, s, i, ell);
    multiplyByScalar(
        intermediate_term,
        invert(s, multiplyDifferentXValues(s, ell, i)),
        s,
        ell);
    add(coeffs_to_return, intermediate_term, s, ell);
  }
}

void computeProductOneToEllPlusOne(
    uint32_t s, uint32_t ell, Polynomial_t & coeffs_to_return) {
  coeffs_to_return = Polynomial_t(
      ell + 2,
      1); // at round i, we want to set the coefficient of x^i term to 1.
  coeffs_to_return[0] = s - 1;

  for (uint32_t i = 2; i < ell + 2; i++) {
    for (uint32_t j = i - 1; j != 0; j--) {
      coeffs_to_return[j] =
          (s - i) * coeffs_to_return[j] + coeffs_to_return[j - 1];
      coeffs_to_return[j] %= s;
    }
    coeffs_to_return[0] *= (s - i);
    coeffs_to_return[0] %= s;
  }
}

/*
Divides by (x-a)
*/
void divideByDegreeOnePolynomial(
    Polynomial_t & coeffs,
    Polynomial_t & output,
    uint32_t s,
    uint32_t a,
    uint32_t ell) {
  output[ell] = 1;
  for (uint32_t i = ell - 1; i + 1 != 0; i--) {
    output[i] = coeffs[i + 1] + a * output[i + 1];
    output[i] %= s;
  }
}

void add(
    Polynomial_t & left_term,
    Polynomial_t & right_term,
    uint32_t s,
    uint32_t degree) {
  for (uint32_t i = 0; i < degree + 1; i++) {
    left_term[i] += right_term[i];
    left_term[i] %= s;
  }
}

void multiplyByScalar(
    Polynomial_t & coeffs,
    uint32_t scalar,
    uint32_t s,
    uint32_t degree) {
  for (uint32_t i = 0; i < degree + 1; i++) {
    coeffs[i] *= scalar;
    coeffs[i] %= s;
  }
}

uint32_t
multiplyDifferentXValues(uint32_t s, uint32_t ell, uint32_t a) {
  uint32_t product = 1;

  for (uint32_t i = 1; i < ell + 2; i++) {
    if (i != a) {
      product = (product * ((s + a - i) % s)) % s;
    }
  }

  return product;
}

/*
 * via extended GCD algorithm to compute x,y with ax+sy = 1
 * Returns x, the modular inverse of a mod s.
 * notation from https://en.wikipedia.org/wiki/Extended_Euclidean_algorithm
*/
uint32_t invert(uint32_t s, uint32_t a) {
  uint32_t r_zero = s;
  uint32_t r_one = a;
  uint32_t t_zero = 0;
  uint32_t t_one = 1;
  uint32_t r_two, t_two, q;

  while (r_one != 0) {
    q = r_zero / r_one;
    r_two = r_zero - q * r_one;
    t_two = t_zero - q * t_one;
    r_zero = r_one;
    t_zero = t_one;
    r_one = r_two;
    t_one = t_two;
  }
  t_zero += s;
  t_zero %= s;
  return t_zero;
}

} // namespace dataowner
} // namespace safrn
