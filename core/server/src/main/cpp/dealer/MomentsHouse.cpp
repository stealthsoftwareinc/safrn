/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

#include <dealer/MomentsHouse.h>

#include <ff/logging.h>

namespace safrn {
namespace dealer {

MomentsRandomnessHouse::MomentsRandomnessHouse(
    dataowner::GlobalInfo const * const g_info,
    std::unique_ptr<dataowner::MomentsInfo const> m_info) :
    globals(g_info), info(std::move(m_info)) {
}

void MomentsRandomnessHouse::init() {
  log_debug("MomentsRandomnessHouse init");
  this->numDealersRemaining = 1;

  std::unique_ptr<Fronctocol> rd(
      new MomentsRandomnessBasement(this->info.get()));
  PeerSet ps(this->getPeers());
  ps.removeRecipients();
  this->invoke(std::move(rd), ps);

  this->getPeers().forEachDataowner([&, this](const Identity & other) {
    this->getPeers().forEachDataowner(
        [&, this](const Identity & other_two) {
          if (other.vertical < other_two.vertical) {

            PeerSet ps = PeerSet();
            ps.add(this->getSelf());
            ps.add(other);
            ps.add(other_two);

            std::unique_ptr<Fronctocol> rd7(
                new ff::mpc::SISOSortRandomnessHouse<
                    SAFRN_TYPES,
                    dataowner::LargeNum,
                    dataowner::SmallNum>(
                    // size of list shared between pair of parties
                    2 * this->globals->maxListSize,
                    this->info->startModulus,
                    this->info->keyModulus,
                    this->info->dealer,
                    this->info->revealer));
            this->invoke(std::move(rd7), ps);
            this->numDealersRemaining++;
          }
        });
  });
}

void MomentsRandomnessHouse::handleReceive(IncomingMessage &) {
  log_error("MomentsRandomnessHouse received unexpected "
            "handle receive");
}

void MomentsRandomnessHouse::handleComplete(Fronctocol &) {
  log_debug("MomentsRandomnessHouse handleComplete");
  this->numDealersRemaining--;
  if (this->numDealersRemaining == 0) {
    log_info("Dealer done");
    this->complete();
  }
}

void MomentsRandomnessHouse::handlePromise(Fronctocol &) {
  log_error("MomentsRandomnessHouse received unexpected "
            "handle promise");
}

std::string MomentsRandomnessHouse::name() {
  return std::string("Moments Randomness House");
}

MomentsRandomnessBasement::MomentsRandomnessBasement(
    dataowner::MomentsInfo const * const m_info) :
    info(m_info) {
}

void MomentsRandomnessBasement::init() {
  log_debug("MomentsRandomnessBasement init");
  this->numDealersRemaining =
      2; // counting the ones without cross-vertical differentiation

  std::unique_ptr<Fronctocol> rd(
      new ff::mpc::ModConvUpRandomnessHouse<
          SAFRN_TYPES,
          ff::mpc::SmallNum,
          dataowner::LargeNum,
          dataowner::LargeNum>(&this->info->modConvUpInfo));
  this->invoke(std::move(rd), this->getPeers());

  std::unique_ptr<Fronctocol> rd2(
      new ff::mpc::DivideRandomnessHouse<
          SAFRN_TYPES,
          dataowner::LargeNum,
          dataowner::SmallNum>(&this->info->divideInfo));
  this->invoke(std::move(rd2), this->getPeers());

  this->getPeers().forEachDataowner([&, this](const Identity & other) {
    this->getPeers().forEachDataowner(
        [&, this](const Identity & other_two) {
          if (other.vertical < other_two.vertical) {

            PeerSet ps = PeerSet();
            ps.add(this->getSelf());
            ps.add(other);
            ps.add(other_two);
            std::unique_ptr<Fronctocol> rd3(
                new ff::mpc::ZipAdjacentRandomnessHouse<
                    SAFRN_TYPES,
                    dataowner::LargeNum,
                    dataowner::SmallNum>(&this->info->zipAdjacentInfo));
            this->invoke(std::move(rd3), ps);
            /** adjust count for cross vertical pairs */
            this->numDealersRemaining++;
          }
        });
  });
}

void MomentsRandomnessBasement::handleReceive(IncomingMessage &) {
  log_error("MomentsRandomnessBasement received unexpected "
            "handle receive");
}

void MomentsRandomnessBasement::handleComplete(Fronctocol &) {
  log_debug("MomentsRandomnessBasement handleComplete");
  this->numDealersRemaining--;
  if (this->numDealersRemaining == 0) {
    log_info("Dealer done");
    this->complete();
  }
}

void MomentsRandomnessBasement::handlePromise(Fronctocol &) {
  log_error("MomentsRandomnessBasement received unexpected "
            "handle promise");
}

std::string MomentsRandomnessBasement::name() {
  return std::string("Moments Randomness Basement");
}

} // namespace dealer
} // namespace safrn
