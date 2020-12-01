/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* C and POSIX Headers */

/* C++ Headers */

/* 3rd Party Headers */
#include <mpc/Randomness.h>
#include <mpc/RandomnessDealer.h>

/* Safrn Headers */
#include <dataowner/LookupPatron.h>
#include <dealer/RandomTableLookup.h>

/* logging configuration */
#include <ff/logging.h>

namespace safrn {
namespace dataowner {

LookupRandomnessPatron::LookupRandomnessPatron(
    dealer::RandomTableLookupInfo const * const info,
    ff::mpc::
        CompareInfo<safrn::Identity, LargeNum, SmallNum> const * const
            compareInfo,
    const safrn::Identity * dealerIdentity,
    const size_t dispenserSize) :
    lookupDispenser(
        new ff::mpc::RandomnessDispenser<
            LookupRandomness,
            ff::mpc::DoNotGenerateInfo>(ff::mpc::DoNotGenerateInfo())),
    info(info),
    compareInfo(compareInfo),
    dealerIdentity(dealerIdentity),
    dispenserSize(dispenserSize),
    numCompareNeeded(1),
    numTypeCastFromBitNeeded(1),
    numTableLookupNeeded(
        1) { // dispenserSize = num Lookups we're going to need
  log_debug("Constructor");
}

void LookupRandomnessPatron::init() {
  log_debug("Calling init on LookupPatron");

  std::unique_ptr<Fronctocol> patron(
      new ff::mpc::
          CompareRandomnessPatron<SAFRN_TYPES, LargeNum, SmallNum>(
              this->compareInfo,
              dealerIdentity,
              this->numCompareNeeded * this->dispenserSize));
  this->invoke(std::move(patron), this->getPeers());
  this->state = awaitingCompare;
}

void LookupRandomnessPatron::handleReceive(IncomingMessage & imsg) {
  log_error("LookupPatron received unexpected "
            "handle receive");
  (void)imsg;
}

void LookupRandomnessPatron::handleComplete(Fronctocol & f) {
  log_debug("LookupPatron received handle complete");
  switch (this->state) {
    case awaitingCompare: {
      this->compareDispenser =
          std::move(static_cast<ff::mpc::CompareRandomnessPatron<
                        SAFRN_TYPES,
                        LargeNum,
                        SmallNum> &>(f)
                        .compareDispenser);

      std::unique_ptr<Fronctocol> patron(
          new ff::mpc::RandomnessPatron<
              SAFRN_TYPES,
              ff::mpc::TypeCastTriple<LargeNum>,
              ff::mpc::TypeCastFromBitInfo<LargeNum>>(
              *dealerIdentity,
              this->numTypeCastFromBitNeeded * this->dispenserSize,
              ff::mpc::TypeCastFromBitInfo<LargeNum>(
                  this->info->r_modulus_)));
      this->invoke(std::move(patron), this->getPeers());
      this->state = awaitingTypeCastFromBit;
    } break;
    case awaitingTypeCastFromBit: {
      log_debug("awaitingTypeCastFromBit");
      this->typeCastFromBitDispenser = std::move(
          static_cast<PromiseFronctocol<ff::mpc::RandomnessDispenser<
              ff::mpc::TypeCastTriple<LargeNum>,
              ff::mpc::TypeCastFromBitInfo<LargeNum>>> &>(f)
              .result);

      ::std::unique_ptr<Fronctocol> patron(
          new ::ff::mpc::RandomnessPatron<
              SAFRN_TYPES,
              dealer::RandomTableLookup,
              dealer::RandomTableLookupInfo>(
              *dealerIdentity,
              this->numTableLookupNeeded * this->dispenserSize,
              *this->info));
      this->invoke(::std::move(patron), this->getPeers());
      this->state = awaitingLookup;
    } break;
    case awaitingLookup: {
      log_debug("case awaitingLookup");
      this->randomTableLookupDispenser = std::move(
          static_cast<PromiseFronctocol<ff::mpc::RandomnessDispenser<
              dealer::RandomTableLookup,
              dealer::RandomTableLookupInfo>> &>(f)
              .result);

      this->generateOutputDispenser();
    } break;
    default:
      log_error("State machine in unexpected state");
  }
}

void LookupRandomnessPatron::handlePromise(Fronctocol &) {
  log_error("LookupPatron Fronctocol received unexpected"
            "handle promise");
}

void LookupRandomnessPatron::generateOutputDispenser() {
  for (size_t i = 0; i < this->dispenserSize; i++) {
    log_debug("Generating output dispenser %zu ", i);
    this->lookupDispenser->insert(LookupRandomness(
        std::move(this->compareDispenser->littleDispenser(
            this->numCompareNeeded)),
        std::move(this->typeCastFromBitDispenser->littleDispenser(
            this->numTypeCastFromBitNeeded)),
        std::move(this->randomTableLookupDispenser->littleDispenser(
            this->numTableLookupNeeded))));
  }
  log_debug("calling this->complete");

  this->complete();
}

std::string LookupRandomnessPatron::name() {
  return std::string("Lookup Randomness Patron");
}

} // namespace dataowner
} // namespace safrn
