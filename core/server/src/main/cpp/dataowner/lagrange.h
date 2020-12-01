/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/*
 * Generates the Lagrange coeffs we need for PrefixOr
 * i.e. returns a polynomial of degree ell
 * with f(1) = 0 and f(2)=f(3)=...=f(ell+1) = 1
 * Helper functions in lagrange.cpp

 * NOTE: Errors occur with s > 2^15 and s < 2^16, presumably
 * bc of some weird forced overflow, e.g. for s = 65521
 */

/* C and POSIX Headers */

#include <cstdint>
#include <memory>
#include <vector>

/* polynomial is stored as a "little-endian" std::vector<uint32_t>,
i.e. coeffs[0] is the constant term*/

#ifndef SAFRN_DATAOWNER_LAGRANGE_H_
#define SAFRN_DATAOWNER_LAGRANGE_H_

namespace safrn {
namespace dataowner {

using Polynomial_t = std::vector<uint32_t>;

void computeLagrangeCoeffsForPrefixOr(
    uint32_t s, uint32_t ell, Polynomial_t & ref);

Polynomial_t computeLagrangeCoeffsForPrefixOr(uint32_t s, uint32_t ell);

void add(
    Polynomial_t & left_term,
    Polynomial_t & right_term,
    uint32_t s,
    uint32_t degree);

void multiplyByScalar(
    Polynomial_t & coeffs,
    uint32_t scalar,
    uint32_t s,
    uint32_t degree);

/* more or less hidden functions, exposed only for testing */
void computeProductOneToEllPlusOne(
    uint32_t s,
    uint32_t ell,
    Polynomial_t & ret); // product (x-i) from one to ell

uint32_t invert(uint32_t s, uint32_t a);

} // namespace dataowner
} // namespace safrn

#endif //SAFRN_DATAOWNER_LAGRANGE_H_
