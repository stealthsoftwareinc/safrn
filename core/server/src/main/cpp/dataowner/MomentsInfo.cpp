/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

#include <dataowner/MomentsInfo.h>

#include <algorithm>

#include <ff/logging.h>

namespace safrn {
namespace dataowner {

static inline LargeNum computeModulus(size_t numBits) {
  log_info("Finding a prime with %zu bits", numBits);
  return ff::mpc::nextPrime(
      static_cast<LargeNum>(LargeNum(1) << numBits));
}

MomentsInfo::MomentsInfo(
    GlobalInfo const * const globals,
    const size_t numCrossParties,
    const size_t selfVertical,
    const size_t dataVertical,
    const bool includeZerothMoment,
    const size_t highest_moment, // max of 4 right now, i.e. 0,1,2,3
    Identity const * dealer,
    Identity const * revealer) :
    numCrossParties(numCrossParties),
    selfVertical(selfVertical),
    dataVertical(dataVertical),
    includeZerothMoment(includeZerothMoment),
    highest_moment(highest_moment),
    dealer(dealer),
    revealer(revealer),
    payloadLength(highest_moment + 1),
    keyModulus(
        ff::mpc::nextPrime(static_cast<LargeNum>(globals->key_max))),
    startModulus(computeModulus(
        2 + 3 * globals->bitsOfPrecision +
        static_cast<size_t>(ceil(log2(globals->maxIntersectionSize))))),
    endModulus(computeModulus(
        2 + 4 * globals->bitsOfPrecision +
        static_cast<size_t>(ceil(log2(globals->maxIntersectionSize))))),
    compareInfo(this->startModulus, revealer),
    compareInfoEndModulus(this->endModulus, revealer),
    startModulusMultiplyInfo(
        revealer, BeaverInfo<LargeNum>(this->startModulus)),
    endModulusMultiplyInfo(
        revealer, ff::mpc::BeaverInfo<LargeNum>(this->endModulus)),
    booleanMultiplyInfo(revealer, ff::mpc::BooleanBeaverInfo()),
    divideInfo(
        revealer,
        this->endModulus,
        this->compareInfoEndModulus.ell,
        this->compareInfoEndModulus
            .lambda, // this is wrong but also we don't use it anymore, fix in next ff MR
        this->compareInfoEndModulus
            .lagrangePolynomialSet, // this is wrong but also we don't use it anymore, fix in next ff MR
        &this->compareInfoEndModulus),
    modConvUpInfo(this->endModulus, this->startModulus, revealer),
    zipAdjacentInfo(
        2 * globals->maxListSize,
        this->payloadLength,
        0,
        this->startModulus,
        revealer) {

  log_debug("dealer: %s", dbuidToStr(this->dealer->orgId).c_str());
  log_debug("revealer: %s", dbuidToStr(this->revealer->orgId).c_str());
}

} // namespace dataowner
} // namespace safrn
