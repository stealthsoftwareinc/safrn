/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

#include <dataowner/MomentsPatron.h>

#include <ff/logging.h>

namespace safrn {
namespace dataowner {

MomentsRandomnessPatron::MomentsRandomnessPatron(
    MomentsInfo const * const info,
    const safrn::Identity * dealerIdentity,
    const size_t dispenserSize) :
    MomentsDispenser(
        new ff::mpc::RandomnessDispenser<
            MomentsRandomness,
            ff::mpc::DoNotGenerateInfo>(ff::mpc::DoNotGenerateInfo())),
    info(info),
    dealerIdentity(dealerIdentity),
    dispenserSize(dispenserSize),
    numConditionalEvaluateNeeded(1),
    numModConvUpNeeded(this->info->payloadLength),
    numDivideNeeded(
        this->info
            ->payloadLength) { // 2 for R^2 and Standard Error { // dispenserSize = num Moments we're going to need
  log_debug("Constructor");
}

void MomentsRandomnessPatron::init() {
  log_debug("Calling init on MomentsPatron");

  this->numCrossParties = 0;

  size_t i = 0;
  this->getPeers().forEachDataowner([&, this](const Identity & other) {
    if (other.vertical != this->getSelf().vertical) {
      this->numCrossParties++;
      this->indexOfCrossParties.insert(
          std::pair<Identity, size_t>(other, i));
      i++;
    }
  });
  std::unique_ptr<Fronctocol> patron(
      new ff::mpc::ModConvUpRandomnessPatron<
          SAFRN_TYPES,
          SmallNum,
          LargeNum,
          LargeNum>(
          &this->info->modConvUpInfo,
          dealerIdentity,
          this->numModConvUpNeeded * this->dispenserSize));

  this->invoke(std::move(patron), this->getPeers());

  this->state = awaitingModConvUp;
}

void MomentsRandomnessPatron::handleReceive(IncomingMessage & imsg) {
  log_error("MomentsPatron Fronctocol received unexpected "
            "handle receive");
  (void)imsg;
}

void MomentsRandomnessPatron::handleComplete(Fronctocol & f) {
  log_debug("MomentsPatron received handle complete");
  switch (this->state) {
    case awaitingModConvUp: {

      log_debug("awaitingModConvUp");
      this->modConvUpDispenser =
          std::move(static_cast<ff::mpc::ModConvUpRandomnessPatron<
                        SAFRN_TYPES,
                        SmallNum,
                        LargeNum,
                        LargeNum> &>(f)
                        .modConvUpDispenser);

      log_debug(
          "this->numDivideNeeded * this->dispenserSize %zu",
          this->numDivideNeeded * this->dispenserSize);
      std::unique_ptr<Fronctocol> patron(
          new ff::mpc::
              DivideRandomnessPatron<SAFRN_TYPES, LargeNum, SmallNum>(
                  &this->info->divideInfo,
                  dealerIdentity,
                  this->numDivideNeeded * this->dispenserSize));
      this->invoke(std::move(patron), this->getPeers());
      this->state = awaitingDivide;
    } break;
    case awaitingDivide: {
      log_debug("awaitingDivide");
      this->divideDispenser =
          std::move(static_cast<ff::mpc::DivideRandomnessPatron<
                        SAFRN_TYPES,
                        LargeNum,
                        SmallNum> &>(f)
                        .divideDispenser);

      this->getPeers().forEachDataowner(
          [&, this](const Identity & other) {
            if (other.vertical != this->getSelf().vertical) {
              zipAdjacentDispensers.emplace_back(nullptr);

              std::unique_ptr<Fronctocol> patron(
                  new ff::mpc::ZipAdjacentRandomnessPatron<
                      SAFRN_TYPES,
                      LargeNum,
                      SmallNum>(
                      &this->info->zipAdjacentInfo,
                      dealerIdentity,
                      this->numConditionalEvaluateNeeded *
                          this->dispenserSize));

              PeerSet ps = PeerSet();
              ps.add(this->getSelf());
              ps.add(other);
              ps.add(*dealerIdentity);

              this->invoke(std::move(patron), ps);
            }
          });
      this->numPartiesAwaiting = this->numCrossParties;
      this->state = awaitingConditionalEvaluate;
    } break;
    case awaitingConditionalEvaluate: {
      log_debug("awaitingConditionalEvaluate");
      PeerSet ps = f.getPeers(); // should be only one other party
      ps.forEachDataowner([&, this](const Identity & other) {
        if (other.vertical != this->getSelf().vertical) {
          this->zipAdjacentDispensers[indexOfCrossParties[other]] =
              std::move(
                  static_cast<ff::mpc::ZipAdjacentRandomnessPatron<
                      SAFRN_TYPES,
                      LargeNum,
                      SmallNum> &>(f)
                      .zipAdjacentDispenser);
        }
      });

      this->numPartiesAwaiting--;
      if (this->numPartiesAwaiting == 0) {
        this->generateOutputDispenser();
      }
    } break;
    default:
      log_error("State machine in unexpected state");
  }
}

void MomentsRandomnessPatron::handlePromise(Fronctocol &) {
  log_error("MomentsPatron Fronctocol received unexpected"
            "handle promise");
}

void MomentsRandomnessPatron::generateOutputDispenser() {
  for (size_t i = 0; i < this->dispenserSize; i++) {

    std::vector<std::unique_ptr<ff::mpc::RandomnessDispenser<
        ff::mpc::ZipAdjacentRandomness<LargeNum, SmallNum>,
        ff::mpc::DoNotGenerateInfo>>>
        littleZipAdjacentDispensers;

    for (size_t j = 0; j < this->numCrossParties; j++) {
      littleZipAdjacentDispensers.emplace_back(
          std::move(this->zipAdjacentDispensers[j]->littleDispenser(
              this->numConditionalEvaluateNeeded)));
    }

    this->MomentsDispenser->insert(MomentsRandomness(
        std::move(this->modConvUpDispenser->littleDispenser(
            this->numModConvUpNeeded)),
        std::move(this->divideDispenser->littleDispenser(
            this->numDivideNeeded)),
        std::move(littleZipAdjacentDispensers)));
  }
  log_debug("calling this->complete");

  this->complete();
}

std::string MomentsRandomnessPatron::name() {
  return std::string("Moments Randomness Patron");
}

} // namespace dataowner
} // namespace safrn
