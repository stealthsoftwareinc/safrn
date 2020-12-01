/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

#ifndef SAFRN_DATAOWNER_REGRESSION_PATRON_H_
#define SAFRN_DATAOWNER_REGRESSION_PATRON_H_

/* C and POSIX Headers */

/* C++ Headers */
#include <cmath>
#include <cstdint>
#include <list>
#include <map>
#include <string>
#include <vector>

/* 3rd Party Headers */
#include <ff/Fronctocol.h>
#include <ff/Message.h>
#include <mpc/DivideDealer.h>
#include <mpc/ModConvUpDealer.h>
#include <mpc/Randomness.h>
#include <mpc/RandomnessDealer.h>
#include <mpc/ZipAdjacentDealer.h>
#include <mpc/templates.h>

/* Safrn Headers */
#include <dataowner/LookupPatron.h>
#include <dataowner/RegressionInfo.h>
#include <dataowner/fortissimo.h>
#include <dealer/RandomSquareMatrix.h>
#include <dealer/RandomTableLookup.h>
#include <framework/Framework.h>

/* logging configuration */
#include <ff/logging.h>

namespace safrn {
namespace dataowner {

class RegressionRandomnessPatron : public Fronctocol {
public:
  void init() override;
  void handleReceive(IncomingMessage & imsg) override;
  void handleComplete(Fronctocol & f) override;
  void handlePromise(Fronctocol & f) override;
  std::string name() override;

  std::unique_ptr<ff::mpc::RandomnessDispenser<
      RegressionRandomness,
      ff::mpc::DoNotGenerateInfo>>
      regressionDispenser;

  RegressionRandomnessPatron(
      RegressionInfo const * const info,
      safrn::Identity const * const dealerIdentity,
      const size_t F_rows,
      const size_t t_rows,
      dealer::RandomTableLookupInfo const * const F_info,
      dealer::RandomTableLookupInfo const * const t_info,
      const size_t dispenserSize);

private:
  void generateOutputDispenser();

  enum RegressionPatronPromiseState {
    awaitingModConvUp,
    awaitingDivide,
    awaitingConditionalEvaluate,
    awaitingBeaverTripleForFactory,
    awaitingBeaverTripleForMatrixMultiply,
    awaitingRandomSquareMatrix,
    awaitingBeaverTripleForFinalMultiply,
    awaitingCompareEndModulus,
    awaitingCompare,
    awaitingTypeCastFromBit,
    awaitingF_lookup,
    awaitingt_lookup
  };
  RegressionPatronPromiseState state = awaitingModConvUp;

  RegressionInfo const * const info;
  const safrn::Identity * dealerIdentity;
  const size_t F_rows;
  const size_t t_rows;
  dealer::RandomTableLookupInfo const * const F_info;
  dealer::RandomTableLookupInfo const * const t_info;
  const size_t dispenserSize;

  const size_t numModConvUpNeeded;
  const size_t numDivideNeeded;
  const size_t numConditionalEvaluateNeeded;
  const size_t numBeaverTripleForFactoryNeeded;
  const size_t numBeaverTripleForMatrixMultiplyNeeded;
  const size_t numRandomSquareMatrixNeeded;
  const size_t numBeaverTripleForFinalMultiplyNeeded;
  const size_t numCompareEndModulusNeeded;
  const size_t numCompareNeeded;
  const size_t numTypeCastFromBitNeeded;
  const size_t numTableLookupFNeeded;
  const size_t numTableLookuptNeeded;

  size_t numPartiesAwaiting;
  size_t numCrossParties;
  std::map<safrn::Identity, size_t> indexOfCrossParties;

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
      arithmeticMultiplyForFactoryDispensers;
  std::unique_ptr<ff::mpc::RandomnessDispenser<
      ff::mpc::BeaverTriple<LargeNum>,
      ff::mpc::BeaverInfo<LargeNum>>>
      arithmeticMultiplyForMatrixDispenser;
  std::unique_ptr<ff::mpc::RandomnessDispenser<
      dealer::RandomSquareMatrix<LargeNum>,
      dealer::RandomSquareMatrixInfo<LargeNum, LargeNum>>>
      randomMatrixAndDetInverseDispenser;
  std::unique_ptr<ff::mpc::RandomnessDispenser<
      ff::mpc::BeaverTriple<LargeNum>,
      ff::mpc::BeaverInfo<LargeNum>>>
      arithmeticMultiplyForFinalDispenser;
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

  /* Additional Vars for Batch Compare */
  /* Vars for typecastfrombit */
};

} // namespace dataowner
} // namespace safrn

#include <ff/logging.h>

#endif //SAFRN_DATAOWNER_REGRESSION
