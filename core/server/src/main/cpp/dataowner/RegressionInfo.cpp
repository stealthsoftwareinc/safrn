/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

#include <dataowner/RegressionInfo.h>

#include <algorithm>

#include <ff/logging.h>

namespace safrn {
namespace dataowner {

static inline size_t computePayloadLength(size_t d1, size_t d2) {
  return std::max(
      d1 + d1 * (d1 + 1) / 2, (d2 + 1) + d2 * (d2 + 3) / 2 + 3);
}

static inline LargeNum computeEndModulus(size_t numBits) {
  log_debug("Finding a prime with %zu bits", numBits);
  return ff::mpc::nextPrime(
      static_cast<LargeNum>(LargeNum(1) << numBits));
}

RegressionInfo::RegressionInfo(
    GlobalInfo const * const globals,
    size_t selfVertical,
    size_t verticalDV,
    size_t vDV_nIVs,
    size_t vnDV_nIVs,
    size_t numCrossParties,
    bool fit_intercept,
    const safrn::Identity * revealer,
    const safrn::Identity * dealer) :
    selfVertical(selfVertical),
    verticalDV(verticalDV),
    verticalDV_numIVs(vDV_nIVs),
    verticalNonDV_numIVs(vnDV_nIVs),
    numCrossParties(numCrossParties),
    num_IVs(vDV_nIVs + vnDV_nIVs),
    fitIntercept(fit_intercept),
    revealer(revealer),
    dealer(dealer),
    payloadLength(computePayloadLength(vnDV_nIVs, vDV_nIVs)),
    bytesInLookupTableCells(globals->bytesInLookupTableCells),
    max_F_t_table_num_rows(globals->max_F_t_table_num_rows),
    keyModulus(
        ff::mpc::nextPrime(static_cast<LargeNum>(globals->key_max))),
    startModulus(ff::mpc::nextPrime(static_cast<LargeNum>(
        (LargeNum(1) << (4 * globals->bitsOfPrecision + 2)) *
        LargeNum(static_cast<uint64_t>(
            floor(globals->maxIntersectionSize)))))),
    endModulus(computeEndModulus(
        2 *
        static_cast<size_t>(ceil(
            static_cast<double>(this->num_IVs) *
            (log2(this->num_IVs) + 2 * globals->bitsOfPrecision + 2 +
             log2(globals->maxIntersectionSize)))))),
    compareInfo(this->startModulus, this->revealer),
    compareInfoEndModulus(this->endModulus, this->revealer),
    startModulusMultiplyInfo(
        this->revealer,
        ff::mpc::BeaverInfo<LargeNum>(this->startModulus)),
    endModulusMultiplyInfo(
        this->revealer,
        ff::mpc::BeaverInfo<LargeNum>(this->endModulus)),
    booleanMultiplyInfo(this->revealer, ff::mpc::BooleanBeaverInfo()),
    divideInfo(
        this->revealer,
        this->endModulus,
        this->compareInfoEndModulus.ell,
        this->compareInfoEndModulus
            .lambda, // this is wrong but also we don't use it anymore, fix in next ff MR
        this->compareInfoEndModulus
            .lagrangePolynomialSet, // this is wrong but also we don't use it anymore, fix in next ff MR
        &this->compareInfoEndModulus),
    modConvUpInfo(this->endModulus, this->startModulus, this->revealer),
    zipAdjacentInfo(
        2 * globals->maxListSize,
        this->payloadLength,
        0,
        this->startModulus,
        this->revealer) {
}

} // namespace dataowner
} // namespace safrn
