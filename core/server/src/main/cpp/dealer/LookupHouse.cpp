/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

#include <dealer/LookupHouse.h>

#include <ff/logging.h>

namespace safrn {
namespace dealer {

LookupRandomnessHouse::LookupRandomnessHouse(
    RandomTableLookupInfo const * const info,
    ff::mpc::CompareInfo<
        safrn::Identity,
        dataowner::LargeNum,
        dataowner::SmallNum> const * const compareInfo) :
    info(info), compareInfo(compareInfo) {
  log_debug("LookupRandomnessHouse constructor");
}

void LookupRandomnessHouse::init() {
  log_debug("LookupRandomnessHouse init");
  this->numDealersRemaining =
      3; // counting only dealers for the whole party; we handle the pairwise dealer counts separately

  std::unique_ptr<Fronctocol> rd1(
      new ff::mpc::CompareRandomnessHouse<
          SAFRN_TYPES,
          dataowner::LargeNum,
          dataowner::SmallNum>(this->compareInfo));
  this->invoke(std::move(rd1), this->getPeers());

  std::unique_ptr<Fronctocol> rd2(
      new ff::mpc::RandomnessHouse<
          SAFRN_TYPES,
          ff::mpc::TypeCastTriple<dataowner::LargeNum>,
          ff::mpc::TypeCastFromBitInfo<dataowner::LargeNum>>());
  this->invoke(std::move(rd2), this->getPeers());

  std::unique_ptr<Fronctocol> rd3(new ff::mpc::RandomnessHouse<
                                  SAFRN_TYPES,
                                  dealer::RandomTableLookup,
                                  dealer::RandomTableLookupInfo>());
  this->invoke(std::move(rd3), this->getPeers());
}

void LookupRandomnessHouse::handleReceive(IncomingMessage &) {
  log_error("LookupRandomnessHouse received unexpected "
            "handle receive");
}

void LookupRandomnessHouse::handleComplete(Fronctocol &) {
  log_debug("LookupRandomnessHouse handleComplete");
  this->numDealersRemaining--;
  if (this->numDealersRemaining == 0) {
    log_info("Dealer done");
    this->complete();
  }
}

void LookupRandomnessHouse::handlePromise(Fronctocol &) {
  log_error("LookupRandomnessHouse received unexpected "
            "handle promise");
}

std::string LookupRandomnessHouse::name() {
  return std::string("LookupRandomHouse");
}

} // namespace dealer
} // namespace safrn
