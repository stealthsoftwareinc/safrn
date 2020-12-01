/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/*
 * SAFRN Moments code
 */

#ifndef SAFRN_DATAOWNER_MOMENTS_H
#define SAFRN_DATAOWNER_MOMENTS_H

/* C and POSIX Headers */

/* C++ Headers */
#include <cmath>
#include <cstdint>
#include <fstream>
#include <map>
#include <string>
#include <vector>

/* 3rd Party Headers */

/* Fortissimo Headers */
#include <ff/Fronctocol.h>
#include <ff/Message.h>

#include <mpc/Batch.h>
#include <mpc/ModConvUp.h>
#include <mpc/ObservationList.h>
#include <mpc/Randomness.h>
#include <mpc/RandomnessDealer.h>
#include <mpc/SISOSort.h>
#include <mpc/ZipAdjacent.h>

#include <dataowner/GlobalInfo.h>
#include <dataowner/MomentsInfo.h>
#include <dataowner/MomentsPatron.h>
#include <dataowner/fortissimo.h>
#include <framework/Framework.h>

#include <dealer/RandomSquareMatrix.h>

/* logging configuration */
#include <ff/logging.h>

namespace safrn {
namespace dataowner {

class Moments : public Fronctocol {
public:
  /*
   * Moments in SAFRN using Fortissimo shuffle-sort and divide
   * to compute and reveal E[X^n] for n = 1, 2, 3 (currently)
   */
  Moments(
      ff::mpc::ObservationList<LargeNum> && olist,
      GlobalInfo const * const globals,
      std::unique_ptr<const MomentsInfo> info);

  void init() override;

  void handleReceive(IncomingMessage & imsg) override;

  void handleComplete(Fronctocol & f) override;

  void handlePromise(Fronctocol & fronctocol) override;

  std::string name() override;

private:
  enum MomentsState {
    awaitingRandomnessAndSISOSort,
    awaitingRandomnessOnly,
    awaitingSISOSort,
    awaitingZipAdjacent,
    awaitingBatchedModConvUp,
    awaitingDivision
  };

  MomentsState state = awaitingRandomnessAndSISOSort;

  MomentsRandomness randomness;

  void computePayloadVectorAndPadList();
  void setupCrossParties();
  void setupCrossVerticalShares();
  void shareWithCrossVerticalParties();
  void invokeRandomnessPatron();

  const std::string csvFile;
  std::unique_ptr<const MomentsInfo> info;
  GlobalInfo const * const globals;

  std::vector<
      ff::mpc::ZipAdjacentInfo<safrn::Identity, LargeNum, SmallNum>>
      zipAdjacentInfo;

  ff::mpc::ObservationList<LargeNum> ownList;

  std::vector<ff::mpc::ObservationList<LargeNum>>
      outgoingListShares; // one for each cross-vertical party.
  std::vector<ff::mpc::ObservationList<LargeNum>>
      sharedLists; // one for each cross-vertical party.

  std::vector<ff::mpc::ObservationList<LargeNum>> vectorZippedAdjacent;

  std::vector<LargeNum> powerSumsStartModulus;
  std::vector<LargeNum> powerSums;
  std::vector<LargeNum> expectationOfNthPow;

  size_t numPartiesAwaiting = 0;

  std::map<safrn::Identity, size_t> indexOfCrossParties;

  bool randomnessDone = false;
  bool abortFlag = false;
};

} // namespace dataowner
} // namespace safrn

#define LOG_UNCLUDE
#include <ff/logging.h>

#endif //SAFRN_DATAOWNER_MOMENTS_H
