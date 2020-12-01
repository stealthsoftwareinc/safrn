/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

#ifndef SAFRN_DATAOWNER_MOMENTS_PATRON_H_
#define SAFRN_DATAOWNER_MOMENTS_PATRON_H_

/* C and POSIX Headers */

/* C++ Headers */
#include <cmath>
#include <cstdint>
#include <list>
#include <map>
#include <string>
#include <vector>

/* 3rd Party Headers */

/* Fortissimo Headers */
#include <ff/Fronctocol.h>
#include <ff/Message.h>

#include <dataowner/MomentsInfo.h>
#include <mpc/Randomness.h>
#include <mpc/RandomnessDealer.h>
#include <mpc/templates.h>

#include <mpc/DivideDealer.h>
#include <mpc/ModConvUpDealer.h>
#include <mpc/ZipAdjacentDealer.h>

#include <dataowner/fortissimo.h>
#include <framework/Framework.h>

/* logging configuration */
#include <ff/logging.h>

namespace safrn {
namespace dataowner {

class MomentsRandomnessPatron : public Fronctocol {
public:
  void init() override;
  void handleReceive(IncomingMessage & imsg) override;
  void handleComplete(Fronctocol & f) override;
  void handlePromise(Fronctocol & f) override;
  std::string name() override;

  std::unique_ptr<ff::mpc::RandomnessDispenser<
      MomentsRandomness,
      ff::mpc::DoNotGenerateInfo>>
      MomentsDispenser;

  MomentsRandomnessPatron(
      MomentsInfo const * const info,
      safrn::Identity const * const dealerIdentity,
      const size_t dispenserSize);

private:
  void generateOutputDispenser();

  enum MomentsPatronPromiseState {
    awaitingModConvUp,
    awaitingDivide,
    awaitingConditionalEvaluate
  };
  MomentsPatronPromiseState state = awaitingModConvUp;

  MomentsInfo const * const info;
  const safrn::Identity * dealerIdentity;
  const size_t dispenserSize;

  const size_t numModConvUpNeeded;
  const size_t numDivideNeeded;
  const size_t numConditionalEvaluateNeeded;

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

  /* Additional Vars for Batch Compare */
  /* Vars for typecastfrombit */
};

} // namespace dataowner
} // namespace safrn

#include <ff/logging.h>

#endif //SAFRN_DATAOWNER_Moments
