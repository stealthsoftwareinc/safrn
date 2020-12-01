/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/*
 * SAFRN MomentsInfo object
 */

#ifndef SAFRN_DATAOWNER_MOMENTS_INFO_H
#define SAFRN_DATAOWNER_MOMENTS_INFO_H

/* C and POSIX Headers */

/* C++ Headers */
#include <cmath>
#include <cstdint>
#include <vector>

/* 3rd Party Headers */

/* Safrn Headers */
#include <dataowner/GlobalInfo.h>
#include <dataowner/fortissimo.h>
#include <dealer/RandomSquareMatrix.h>
#include <ff/Fronctocol.h>
#include <ff/Message.h>
#include <framework/Framework.h>
#include <mpc/Divide.h>
#include <mpc/ZipAdjacent.h>
#include <mpc/simplePrime.h>
#include <mpc/templates.h>

/* logging configuration */
#include <ff/logging.h>

namespace safrn {
namespace dataowner {

struct MomentsInfo {

  /** selfVertical is which vertical this party belongs to
    * dataVertical is which vertical holds the relevant data column
    * The reason that these two may be different is if this party is
    * not in the vertical for which the moment is being computed, which
    * can happen e.g. if this party's data is being used as a prefilter.
    */
  size_t selfVertical; // 0 or 1
  size_t dataVertical; // 0 or 1

  /** TODO: read from study config */
  const bool includeZerothMoment = true; // i.e. do we reveal count?
  /** TODO: read from query, max of 3 */
  const size_t highest_moment =
      3; // 0, 1, 2, 3, count, mean, variance, skew

  /** We currently support joins via an equality constraint
    * on a single column only. Here the second column
    * represents the vertical, and is used to ensure keys are
    * lexicographically distinct during the sorting.
    */
  const size_t numKeyCols = 2;

  /** determined from the above */
  size_t payloadLength; // = highest_moment + 1;

  LargeNum keyModulus;
  LargeNum startModulus;
  LargeNum endModulus;

  Identity const * dealer;
  Identity const * revealer;

  ff::mpc::
      CompareInfo<safrn::Identity, ff::mpc::LargeNum, ff::mpc::SmallNum>
          compareInfo;
  ff::mpc::CompareInfo<safrn::Identity, LargeNum, SmallNum>
      compareInfoEndModulus;
  ff::mpc::MultiplyInfo<safrn::Identity, BeaverInfo<LargeNum>>
      startModulusMultiplyInfo;
  ff::mpc::MultiplyInfo<safrn::Identity, ff::mpc::BeaverInfo<LargeNum>>
      endModulusMultiplyInfo;
  ff::mpc::MultiplyInfo<safrn::Identity, ff::mpc::BooleanBeaverInfo>
      booleanMultiplyInfo;
  ff::mpc::DivideInfo<safrn::Identity, LargeNum, SmallNum> divideInfo;
  ff::mpc::ModConvUpInfo<safrn::Identity, SmallNum, LargeNum, LargeNum>
      modConvUpInfo;
  ff::mpc::ZipAdjacentInfo<safrn::Identity, LargeNum, SmallNum>
      zipAdjacentInfo;

  size_t numCrossParties;
  size_t numComparesNeededPerBatchedCompute;
  size_t numTctsNeededPerBatchedCompute;
  size_t numArithBeaversNeededPerBatchedCompute;
  size_t numXORBeaversNeededPerBatchedCompute;

  MomentsInfo(
      GlobalInfo const * const globals,
      const size_t numCrossParties,
      const size_t selfVertical,
      const size_t dataVertical,
      const bool includeZerothMoment,
      const size_t highest_moment, // max of 4 right now, i.e. 0,1,2,3
      Identity const * dealer,
      Identity const * revealer);
};

struct MomentsRandomness {

  std::unique_ptr<ff::mpc::RandomnessDispenser<
      ff::mpc::ModConvUpRandomness<SmallNum, LargeNum, LargeNum>,
      ff::mpc::DoNotGenerateInfo>>
      modConvUpDispenser;

  std::unique_ptr<ff::mpc::RandomnessDispenser<
      ff::mpc::DivideRandomness<LargeNum, SmallNum>,
      ff::mpc::DoNotGenerateInfo>>
      divideDispenser;

  std::vector<std::unique_ptr<ff::mpc::RandomnessDispenser<
      ff::mpc::ZipAdjacentRandomness<LargeNum, SmallNum>,
      ff::mpc::DoNotGenerateInfo>>>
      zipAdjacentDispensers;

  MomentsRandomness(
      std::unique_ptr<ff::mpc::RandomnessDispenser<
          ff::mpc::ModConvUpRandomness<SmallNum, LargeNum, LargeNum>,
          ff::mpc::DoNotGenerateInfo>> modConvUpDispenser,
      std::unique_ptr<ff::mpc::RandomnessDispenser<
          ff::mpc::DivideRandomness<LargeNum, SmallNum>,
          ff::mpc::DoNotGenerateInfo>> divideDispenser,
      std::vector<std::unique_ptr<ff::mpc::RandomnessDispenser<
          ff::mpc::ZipAdjacentRandomness<LargeNum, SmallNum>,
          ff::mpc::DoNotGenerateInfo>>> && zipAdjacentDispensers) :
      modConvUpDispenser(std::move(modConvUpDispenser)),
      divideDispenser(std::move(divideDispenser)),
      zipAdjacentDispensers(std::move(zipAdjacentDispensers)) {
  }

  MomentsRandomness() :
      modConvUpDispenser(nullptr), divideDispenser(nullptr) {
  }
};

} // namespace dataowner
} // namespace safrn

#define LOG_UNCLUDE
#include <ff/logging.h>

#endif //SAFRN_DATAOWNER_MOMENTS_INFO_H
