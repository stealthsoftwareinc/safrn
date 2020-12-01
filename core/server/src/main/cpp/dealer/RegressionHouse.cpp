/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

#include <dealer/RegressionHouse.h>

#include <ff/logging.h>

namespace safrn {
namespace dealer {

RegressionRandomnessHouse::RegressionRandomnessHouse(
    dataowner::GlobalInfo const * const g_info,
    std::unique_ptr<dataowner::RegressionInfo const> info) :
    globals(g_info), info(std::move(info)) {
}

void RegressionRandomnessHouse::init() {
  log_debug("RegressionRandomnessHouse init");
  this->numDealersRemaining = 1;
  // high-level
  std::unique_ptr<Fronctocol> rd(
      new RegressionRandomnessBasement(this->info.get()));
  PeerSet ps(this->getPeers());
  ps.removeRecipients();
  this->invoke(std::move(rd), ps);

  this->getPeers().forEachDataowner([&, this](const Identity & other) {
    this->getPeers().forEachDataowner([&, this](
                                          const Identity & other_two) {
      if (other.vertical < other_two.vertical) {

        PeerSet ps = PeerSet();
        ps.add(this->getSelf());
        ps.add(other);
        ps.add(other_two);

        // low-level
        std::unique_ptr<Fronctocol> rd7(
            new ff::mpc::SISOSortRandomnessHouse<
                SAFRN_TYPES,
                dataowner::LargeNum,
                dataowner::SmallNum>(
                2 * this->globals->maxListSize,
                this->info->startModulus,
                this->info->keyModulus,
                this->info->dealer,
                this->info
                    ->revealer)); // For now, we only allow 2 key cols. With bignums, we probably should never do anything else
        this->invoke(std::move(rd7), ps);
        this->numDealersRemaining++;
      }
    });
  });
}

void RegressionRandomnessHouse::handleReceive(IncomingMessage &) {
  log_error("RegressionRandomnessHouse received unexpected "
            "handle receive");
}

void RegressionRandomnessHouse::handleComplete(Fronctocol &) {
  log_debug("RegressionRandomnessHouse handleComplete");
  this->numDealersRemaining--;
  if (this->numDealersRemaining == 0) {
    log_debug("Dealer done");
    this->complete();
  }
}

void RegressionRandomnessHouse::handlePromise(Fronctocol &) {
  log_error("RegressionRandomnessHouse received unexpected "
            "handle promise");
}

std::string RegressionRandomnessHouse::name() {
  return std::string("Regression Randomness House");
}

RegressionRandomnessBasement::RegressionRandomnessBasement(
    dataowner::RegressionInfo const * const info) :
    info(info) {
}

void RegressionRandomnessBasement::init() {
  log_debug("RegressionRandomnessBasement init");

  this->numDealersRemaining =
      10; // counting only dealers for the whole party; we handle the pairwise dealer counts separately

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

  log_debug("here");

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
            this->numDealersRemaining++; // adjusting count
          }
        });
  });

  log_debug("here");

  this->getPeers().forEachDataowner([&, this](const Identity & other) {
    this->getPeers().forEachDataowner(
        [&, this](const Identity & other_two) {
          if (other.vertical < other_two.vertical) {

            PeerSet ps = PeerSet();
            ps.add(this->getSelf());
            ps.add(other);
            ps.add(other_two);

            // low-level
            std::unique_ptr<Fronctocol> rd4(
                new ff::mpc::RandomnessHouse<
                    SAFRN_TYPES,
                    ff::mpc::BeaverTriple<dataowner::LargeNum>,
                    ff::mpc::BeaverInfo<dataowner::LargeNum>>());
            this->invoke(std::move(rd4), ps);
            this->numDealersRemaining++; // adjusting count
          }
        });
  });

  std::unique_ptr<Fronctocol> rd5(
      new ff::mpc::RandomnessHouse<
          SAFRN_TYPES,
          ff::mpc::BeaverTriple<dataowner::LargeNum>,
          ff::mpc::BeaverInfo<dataowner::LargeNum>>());
  this->invoke(std::move(rd5), this->getPeers());

  std::unique_ptr<Fronctocol> rd6(
      new ff::mpc::RandomnessHouse<
          SAFRN_TYPES,
          dealer::RandomSquareMatrix<dataowner::LargeNum>,
          dealer::RandomSquareMatrixInfo<
              dataowner::LargeNum,
              dataowner::LargeNum>>());
  this->invoke(std::move(rd6), this->getPeers());

  log_debug("here");

  std::unique_ptr<Fronctocol> rd7(
      new ff::mpc::RandomnessHouse<
          SAFRN_TYPES,
          ff::mpc::BeaverTriple<dataowner::LargeNum>,
          ff::mpc::BeaverInfo<dataowner::LargeNum>>());
  this->invoke(std::move(rd7), this->getPeers());

  std::unique_ptr<Fronctocol> rd8(
      new ff::mpc::CompareRandomnessHouse<
          SAFRN_TYPES,
          dataowner::LargeNum,
          dataowner::SmallNum>(&this->info->compareInfoEndModulus));
  this->invoke(std::move(rd8), this->getPeers());

  std::unique_ptr<Fronctocol> rd8b(
      new ff::mpc::CompareRandomnessHouse<
          SAFRN_TYPES,
          dataowner::LargeNum,
          dataowner::SmallNum>(&this->info->compareInfo));
  this->invoke(std::move(rd8b), this->getPeers());

  std::unique_ptr<Fronctocol> rd9(
      new ff::mpc::RandomnessHouse<
          SAFRN_TYPES,
          ff::mpc::TypeCastTriple<dataowner::LargeNum>,
          ff::mpc::TypeCastFromBitInfo<dataowner::LargeNum>>());
  this->invoke(std::move(rd9), this->getPeers());

  log_debug("here");

  /** For F-test */
  std::unique_ptr<Fronctocol> rd10(new LookupRandomnessHouse(
      new dealer::RandomTableLookupInfo(this->info->endModulus, 0UL),
      &this->info->compareInfoEndModulus));
  this->invoke(std::move(rd10), this->getPeers());

  /** For t-test, need a different info object for a different possible table size */
  std::unique_ptr<Fronctocol> rd11(new LookupRandomnessHouse(
      new dealer::RandomTableLookupInfo(this->info->endModulus, 0UL),
      &this->info->compareInfoEndModulus));
  this->invoke(std::move(rd11), this->getPeers());

  log_debug("here");
}

void RegressionRandomnessBasement::handleReceive(IncomingMessage &) {
  log_error("RegressionRandomnessBasement received unexpected "
            "handle receive");
}

void RegressionRandomnessBasement::handleComplete(Fronctocol &) {
  log_debug("RegressionRandomnessBasement handleComplete");
  this->numDealersRemaining--;
  if (this->numDealersRemaining == 0) {
    log_debug("Dealer done");
    this->complete();
  }
}

void RegressionRandomnessBasement::handlePromise(Fronctocol &) {
  log_error("RegressionRandomnessBasement received unexpected "
            "handle promise");
}

std::string RegressionRandomnessBasement::name() {
  return std::string("Regression Randomness Basement");
}

} // namespace dealer
} // namespace safrn
