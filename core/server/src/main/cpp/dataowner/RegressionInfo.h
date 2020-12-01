/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/*
 * SAFRN RegressionInfo object
 */

#ifndef SAFRN_DATAOWNER_REGRESSION_INFO_H
#define SAFRN_DATAOWNER_REGRESSION_INFO_H

/* C and POSIX Headers */

/* C++ Headers */
#include <cmath>
#include <cstdint>
#include <vector>

/* 3rd Party Headers */

/* Safrn Headers */
#include <dataowner/GlobalInfo.h>
#include <dataowner/Lookup.h>
#include <dataowner/fortissimo.h>
#include <dealer/RandomSquareMatrix.h>
#include <dealer/RandomTableLookup.h>
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

struct RegressionInfo {

  size_t selfVertical; // 0 or 1

  // DV for Dependent Variable
  size_t verticalDV; // 0 or 1

  size_t verticalDV_numIVs;
  size_t verticalNonDV_numIVs;

  /* IV for Independent Variable
   * By convention, sort so when keys match, Opp first, then Same
   */
  size_t numCrossParties;

  size_t num_IVs; // sum of the sizes of the two lists above

  bool const fitIntercept;

  const safrn::Identity * revealer;
  const safrn::Identity * dealer;

  size_t payloadLength;

  const size_t bytesInLookupTableCells;
  const size_t max_F_t_table_num_rows;

  LargeNum keyModulus;
  LargeNum startModulus;
  LargeNum endModulus;

  ff::mpc::
      CompareInfo<safrn::Identity, ff::mpc::LargeNum, ff::mpc::SmallNum>
          compareInfo;
  ff::mpc::CompareInfo<safrn::Identity, LargeNum, SmallNum>
      compareInfoEndModulus;
  ff::mpc::MultiplyInfo<safrn::Identity, ff::mpc::BeaverInfo<LargeNum>>
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

  size_t numComparesNeededPerBatchedCompute;
  size_t numTctsNeededPerBatchedCompute;
  size_t numArithBeaversNeededPerBatchedCompute;
  size_t numXORBeaversNeededPerBatchedCompute;

  RegressionInfo(
      GlobalInfo const * const globals,
      size_t selfVertical,
      size_t verticalDV,
      size_t vDV_nIVs,
      size_t vnDV_nIVs,
      size_t numCrossParties,
      bool fit_intercept,
      const safrn::Identity * revealer,
      const safrn::Identity * dealer);
};

struct RegressionRandomness {

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

  std::vector<std::unique_ptr<ff::mpc::RandomnessDispenser<
      ff::mpc::BeaverTriple<LargeNum>,
      ff::mpc::BeaverInfo<LargeNum>>>>
      beaverTripleForFactoryDispensers;

  std::unique_ptr<ff::mpc::RandomnessDispenser<
      ff::mpc::BeaverTriple<LargeNum>,
      ff::mpc::BeaverInfo<LargeNum>>>
      beaverTripleForMatrixMultiplyDispenser;

  std::unique_ptr<ff::mpc::RandomnessDispenser<
      dealer::RandomSquareMatrix<LargeNum>,
      dealer::RandomSquareMatrixInfo<LargeNum, LargeNum>>>
      randomMatrixAndDetInverseDispenser;

  std::unique_ptr<ff::mpc::RandomnessDispenser<
      ff::mpc::BeaverTriple<LargeNum>,
      ff::mpc::BeaverInfo<LargeNum>>>
      beaverTripleForFinalMultiplyDispenser;

  std::unique_ptr<ff::mpc::RandomnessDispenser<
      ff::mpc::CompareRandomness<LargeNum, SmallNum>,
      ff::mpc::DoNotGenerateInfo>>
      compareEndModulusDispenser;

  std::unique_ptr<ff::mpc::RandomnessDispenser<
      ff::mpc::CompareRandomness<LargeNum, SmallNum>,
      ff::mpc::DoNotGenerateInfo>>
      compareDispenser;

  std::unique_ptr<ff::mpc::RandomnessDispenser<
      ff::mpc::TypeCastTriple<LargeNum>,
      ff::mpc::TypeCastFromBitInfo<LargeNum>>>
      typeCastFromBitDispenser;

  std::unique_ptr<ff::mpc::RandomnessDispenser<
      LookupRandomness,
      ff::mpc::DoNotGenerateInfo>>
      F_lookupDispenser;

  std::unique_ptr<ff::mpc::RandomnessDispenser<
      LookupRandomness,
      ff::mpc::DoNotGenerateInfo>>
      t_lookupDispenser;

  RegressionRandomness(
      std::unique_ptr<ff::mpc::RandomnessDispenser<
          ff::mpc::ModConvUpRandomness<SmallNum, LargeNum, LargeNum>,
          ff::mpc::DoNotGenerateInfo>> modConvUpDispenser,
      std::unique_ptr<ff::mpc::RandomnessDispenser<
          ff::mpc::DivideRandomness<LargeNum, SmallNum>,
          ff::mpc::DoNotGenerateInfo>> divideDispenser,
      std::vector<std::unique_ptr<ff::mpc::RandomnessDispenser<
          ff::mpc::ZipAdjacentRandomness<LargeNum, SmallNum>,
          ff::mpc::DoNotGenerateInfo>>> && zipAdjacentDispensers,
      std::vector<std::unique_ptr<ff::mpc::RandomnessDispenser<
          ff::mpc::BeaverTriple<LargeNum>,
          ff::mpc::BeaverInfo<LargeNum>>>> &&
          beaverTripleForFactoryDispensers,
      std::unique_ptr<ff::mpc::RandomnessDispenser<
          ff::mpc::BeaverTriple<LargeNum>,
          ff::mpc::BeaverInfo<LargeNum>>>
          beaverTripleForMatrixMultiplyDispenser,
      std::unique_ptr<ff::mpc::RandomnessDispenser<
          dealer::RandomSquareMatrix<LargeNum>,
          dealer::RandomSquareMatrixInfo<LargeNum, LargeNum>>>
          randomMatrixAndDetInverseDispenser,
      std::unique_ptr<ff::mpc::RandomnessDispenser<
          ff::mpc::BeaverTriple<LargeNum>,
          ff::mpc::BeaverInfo<LargeNum>>>
          beaverTripleForFinalMultiplyDispenser,
      std::unique_ptr<ff::mpc::RandomnessDispenser<
          ff::mpc::CompareRandomness<LargeNum, SmallNum>,
          ff::mpc::DoNotGenerateInfo>> compareEndModulusDispenser,
      std::unique_ptr<ff::mpc::RandomnessDispenser<
          ff::mpc::CompareRandomness<LargeNum, SmallNum>,
          ff::mpc::DoNotGenerateInfo>> compareDispenser,
      std::unique_ptr<ff::mpc::RandomnessDispenser<
          ff::mpc::TypeCastTriple<LargeNum>,
          ff::mpc::TypeCastFromBitInfo<LargeNum>>>
          typeCastFromBitDispenser,
      std::unique_ptr<ff::mpc::RandomnessDispenser<
          LookupRandomness,
          ff::mpc::DoNotGenerateInfo>> F_lookupDispenser,
      std::unique_ptr<ff::mpc::RandomnessDispenser<
          LookupRandomness,
          ff::mpc::DoNotGenerateInfo>> t_lookupDispenser) :
      modConvUpDispenser(std::move(modConvUpDispenser)),
      divideDispenser(std::move(divideDispenser)),
      zipAdjacentDispensers(std::move(zipAdjacentDispensers)),
      beaverTripleForFactoryDispensers(
          std::move(beaverTripleForFactoryDispensers)),
      beaverTripleForMatrixMultiplyDispenser(
          std::move(beaverTripleForMatrixMultiplyDispenser)),
      randomMatrixAndDetInverseDispenser(
          std::move(randomMatrixAndDetInverseDispenser)),
      beaverTripleForFinalMultiplyDispenser(
          std::move(beaverTripleForFinalMultiplyDispenser)),
      compareEndModulusDispenser(std::move(compareEndModulusDispenser)),
      compareDispenser(std::move(compareDispenser)),
      typeCastFromBitDispenser(std::move(typeCastFromBitDispenser)),
      F_lookupDispenser(std::move(F_lookupDispenser)),
      t_lookupDispenser(std::move(t_lookupDispenser)) {
  }

  RegressionRandomness() :
      modConvUpDispenser(nullptr),
      divideDispenser(nullptr),
      beaverTripleForMatrixMultiplyDispenser(nullptr),
      randomMatrixAndDetInverseDispenser(nullptr),
      compareEndModulusDispenser(nullptr),
      compareDispenser(nullptr),
      typeCastFromBitDispenser(nullptr),
      F_lookupDispenser(nullptr),
      t_lookupDispenser(nullptr) {
  }
};

} // namespace dataowner
} // namespace safrn

#define LOG_UNCLUDE
#include <ff/logging.h>

#endif //SAFRN_DATAOWNER_REGRESSION_INFO_H
