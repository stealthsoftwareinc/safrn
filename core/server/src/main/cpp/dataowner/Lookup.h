/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/*
 * SAFRN Lookup code
 */

#ifndef SAFRN_DATAOWNER_LOOKUP_H
#define SAFRN_DATAOWNER_LOOKUP_H

/* C and POSIX Headers */

/* C++ Headers */
#include <cmath>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include <fstream>

/* 3rd Party Headers */

/* Fortissimo Headers */
#include <ff/Fronctocol.h>
#include <ff/Message.h>
#include <mpc/Compare.h>
#include <mpc/ModConvUp.h>
#include <mpc/Randomness.h>
#include <mpc/RandomnessDealer.h>

#include <dataowner/fortissimo.h>
#include <framework/Framework.h>

#include <dealer/RandomTableLookup.h>

/* logging configuration */
#include <ff/logging.h>

namespace safrn {
namespace dataowner {

struct LookupRandomness {
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

  LookupRandomness(
      std::unique_ptr<ff::mpc::RandomnessDispenser<
          ff::mpc::CompareRandomness<LargeNum, SmallNum>,
          ff::mpc::DoNotGenerateInfo>> compareDispenser,
      std::unique_ptr<ff::mpc::RandomnessDispenser<
          ff::mpc::TypeCastTriple<LargeNum>,
          ff::mpc::TypeCastFromBitInfo<LargeNum>>>
          typeCastFromBitDispenser,
      std::unique_ptr<ff::mpc::RandomnessDispenser<
          dealer::RandomTableLookup,
          dealer::RandomTableLookupInfo>> randomTableLookupDispenser) :
      compareDispenser(std::move(compareDispenser)),
      typeCastFromBitDispenser(std::move(typeCastFromBitDispenser)),
      randomTableLookupDispenser(
          std::move(randomTableLookupDispenser)) {
  }

  LookupRandomness(LookupRandomness &&) = default;
  LookupRandomness(const LookupRandomness &) = delete;
};

class Lookup : public Fronctocol {
public:
  std::vector<Boolean_t> &
      output_p_value_shares; // store result by reference

  /*
   * Lookup in SAFRN using Fortissimo shuffle and
   *
   */
  Lookup(
      LargeNum const locationShare,
      std::vector<Boolean_t> & output_p_value_shares,
      std::vector<std::vector<Boolean_t>> const & tableData,
      LookupRandomness && randomess,
      dealer::RandomTableLookupInfo const * const info,
      size_t const tableValueByteLength,
      const safrn::Identity * revealer);

  void init() override;

  void handleReceive(IncomingMessage & imsg) override;

  void handleComplete(Fronctocol & f) override;

  void handlePromise(Fronctocol & fronctocol) override;

  std::string name() override;

private:
  enum LookupState { awaitingCompare, awaitingTypeCastFromBit };

  LookupState state = awaitingCompare;

  void sendIndexShare(LargeNum val);
  void computeFinalShare();

  const LargeNum locationShare;
  const std::string csvFileOfPublicTable;
  LookupRandomness randomness;
  dealer::RandomTableLookupInfo const * const info;
  size_t tableValueByteLength;

  LargeNum revealedValue;
  size_t revealedValueDownsized;

  const safrn::Identity * revealer;
  ff::mpc::CompareInfo<safrn::Identity, LargeNum, SmallNum> compareInfo;

  dealer::RandomTableLookup randomTable;

  size_t numPartiesAwaiting = 0;

  std::vector<std::vector<Boolean_t>> const & tableData;
};

} // namespace dataowner
} // namespace safrn

#define LOG_UNCLUDE
#include <ff/logging.h>

#endif //SAFRN_DATAOWNER_LOOKUP_H
