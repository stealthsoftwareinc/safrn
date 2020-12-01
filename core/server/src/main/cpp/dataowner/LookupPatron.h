/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

#ifndef SAFRN_DATAOWNER_LOOKUP_PATRON_H_
#define SAFRN_DATAOWNER_LOOKUP_PATRON_H_

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
#include <mpc/Compare.h>
#include <mpc/CompareDealer.h>
#include <mpc/ModConvUp.h>
#include <mpc/Randomness.h>
#include <mpc/RandomnessDealer.h>
#include <mpc/templates.h>

/* Safrn Headers */
#include <dataowner/Lookup.h>
#include <dataowner/fortissimo.h>
#include <dealer/RandomTableLookup.h>
#include <framework/Framework.h>

/* logging configuration */
#include <ff/logging.h>

namespace safrn {
namespace dataowner {

class LookupRandomnessPatron : public Fronctocol {
public:
  void init() override;
  void handleReceive(IncomingMessage & imsg) override;
  void handleComplete(Fronctocol & f) override;
  void handlePromise(Fronctocol & f) override;
  std::string name() override;

  std::unique_ptr<ff::mpc::RandomnessDispenser<
      LookupRandomness,
      ff::mpc::DoNotGenerateInfo>>
      lookupDispenser;

  LookupRandomnessPatron(
      dealer::RandomTableLookupInfo const * const info,
      ff::mpc::
          CompareInfo<safrn::Identity, LargeNum, SmallNum> const * const
              compareInfo,
      safrn::Identity const * const dealerIdentity,
      const size_t dispenserSize);

private:
  void generateOutputDispenser();

  enum LookupPatronPromiseState {
    awaitingCompare,
    awaitingTypeCastFromBit,
    awaitingLookup
  };
  LookupPatronPromiseState state = awaitingCompare;

  dealer::RandomTableLookupInfo const * const info;
  ff::mpc::
      CompareInfo<safrn::Identity, LargeNum, SmallNum> const * const
          compareInfo;
  safrn::Identity const * const dealerIdentity;
  const size_t dispenserSize;

  const size_t numCompareNeeded;
  const size_t numTypeCastFromBitNeeded;
  const size_t numTableLookupNeeded;

  size_t numPartiesAwaiting;

  std::unique_ptr<ff::mpc::RandomnessDispenser<
      ff::mpc::CompareRandomness<LargeNum, SmallNum>,
      ff::mpc::DoNotGenerateInfo>>
      compareDispenser;
  std::unique_ptr<ff::mpc::RandomnessDispenser<
      ff::mpc::TypeCastTriple<LargeNum>,
      ff::mpc::TypeCastFromBitInfo<LargeNum>>>
      typeCastFromBitDispenser;
  std::unique_ptr<ff::mpc::RandomnessDispenser<
      dealer::RandomTableLookup,
      dealer::RandomTableLookupInfo>>
      randomTableLookupDispenser;

  /* Additional Vars for Batch Compare */
  /* Vars for typecastfrombit */
};

} // namespace dataowner
} // namespace safrn

#include <ff/logging.h>

#endif //SAFRN_DATAOWNER_REGRESSION
