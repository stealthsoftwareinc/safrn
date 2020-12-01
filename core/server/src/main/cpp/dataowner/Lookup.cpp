/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

#include <dataowner/Lookup.h>

/* logging configuration */
#include <ff/logging.h>

namespace safrn {
namespace dataowner {

Lookup::Lookup(
    LargeNum const locationShare,
    std::vector<Boolean_t> & output_p_value_shares,
    std::vector<std::vector<Boolean_t>> const & tableData,
    LookupRandomness && randomness,
    dealer::RandomTableLookupInfo const * const info,
    size_t const tableValueByteLength,
    const safrn::Identity * revealer) :
    output_p_value_shares(output_p_value_shares),
    locationShare(locationShare),
    tableData(tableData),
    randomness(std::move(randomness)),
    info(info),
    tableValueByteLength(tableValueByteLength),
    revealer(revealer),
    compareInfo(info->r_modulus_, revealer) {
  this->randomTable =
      this->randomness.randomTableLookupDispenser->get();
  this->numPartiesAwaiting = 0;
}

void Lookup::init() {
  log_debug("Calling init");

  std::unique_ptr<Fronctocol> compare(
      new ff::mpc::Compare<SAFRN_TYPES, LargeNum, SmallNum>(
          0, // possibly switch
          ff::mpc::modSub(
              this->locationShare,
              this->randomTable.r_,
              this->info->r_modulus_),
          &this->compareInfo,
          std::move(this->randomness.compareDispenser->get())));

  this->invoke(std::move(compare), this->getPeers());
}

void Lookup::handleReceive(IncomingMessage & msg) {
  log_debug(
      "Calling handleReceive with %zu parties remaining",
      this->numPartiesAwaiting);
  if (this->numPartiesAwaiting == 0) // i.e. if we are not the revealer
  {
    msg.template read<LargeNum>(this->revealedValue);
    log_debug(
        "Modulus %s", ff::mpc::dec(this->info->r_modulus_).c_str());
    log_debug(
        "Revealed value %s", ff::mpc::dec(this->revealedValue).c_str());
    this->computeFinalShare();
  } else {
    log_debug(
        "Revealed value %s", ff::mpc::dec(this->revealedValue).c_str());
    LargeNum sent_val;
    msg.read<LargeNum>(sent_val);
    this->revealedValue += sent_val;
    this->revealedValue %= this->info->r_modulus_;
    this->numPartiesAwaiting--;
    if (this->numPartiesAwaiting == 0) {
      // send to other parties then
      log_debug(
          "Revealed value %s",
          ff::mpc::dec(this->revealedValue).c_str());
      this->getPeers().forEachDataowner(
          [&, this](const Identity & other) {
            if (other != *this->revealer) {
              std::unique_ptr<OutgoingMessage> omsg(
                  new OutgoingMessage(other));
              omsg->template write<LargeNum>(this->revealedValue);
              this->send(std::move(omsg));
            }
          });

      this->computeFinalShare();
    }
  }
}

void Lookup::handleComplete(Fronctocol & f) {
  log_debug("Calling handleComplete");

  switch (this->state) {
    case awaitingCompare: {
      Boolean_t comparisonResultBoolean =
          static_cast<
              ff::mpc::Compare<SAFRN_TYPES, LargeNum, SmallNum> &>(f)
              .outputShare;

      std::unique_ptr<ff::mpc::TypeCastFromBit<SAFRN_TYPES, LargeNum>>
          typeCast(new ff::mpc::TypeCastFromBit<SAFRN_TYPES, LargeNum>(
              comparisonResultBoolean,
              this->info->r_modulus_,
              this->revealer,
              this->randomness.typeCastFromBitDispenser->get()));

      this->state = awaitingTypeCastFromBit;
      this->invoke(std::move(typeCast), this->getPeers());
    } break;
    case awaitingTypeCastFromBit: {
      LargeNum comparisonResultArithmetic =
          static_cast<
              ff::mpc::TypeCastFromBit<SAFRN_TYPES, LargeNum> &>(f)
              .outputBitShare;
      this->sendIndexShare(
          ((this->info->r_modulus_ + this->locationShare -
            this->randomTable.r_) +
           comparisonResultArithmetic * this->info->table_size_) %
          this->info->r_modulus_);
    } break;
    default:
      log_error("Compare state machine in unexpected state");
  }
}

void Lookup::handlePromise(Fronctocol &) {
  log_error("Unexpected handlePromise");
}

std::string Lookup::name() {
  return std::string("Lookup");
}

void Lookup::sendIndexShare(LargeNum val) {
  if (this->getSelf() != *this->revealer) {

    this->getPeers().forEachDataowner(
        [&, this](const Identity & other) {
          if (other == *this->revealer) {
            std::unique_ptr<OutgoingMessage> omsg(
                new OutgoingMessage(other));
            omsg->template write<LargeNum>(val);
            this->send(std::move(omsg));
          }
        });
    this->numPartiesAwaiting = 0;
  } else {
    this->getPeers().forEachDataowner(
        [&, this](const Identity & other) {
          if (other != this->getSelf()) {
            this->numPartiesAwaiting++;
          }
        });
    this->revealedValue = val;
  }
}

void Lookup::computeFinalShare() {
  this->revealedValueDownsized =
      static_cast<size_t>(this->revealedValue);
  this->output_p_value_shares.resize(this->tableValueByteLength);
  for (size_t i = 0; i < this->randomTable.u_.size(); i++) {
    // A[x-r+i] * u[i]
    if (this->randomTable.u_[i] == 0x01) {
      for (size_t j = 0; j < this->tableValueByteLength; j++) {
        output_p_value_shares[j] ^=
            this->tableData
                [(this->revealedValueDownsized + i) %
                 this->info->table_size_][j];
      }
    }
  }
  this->complete();
}

} // namespace dataowner
} // namespace safrn
