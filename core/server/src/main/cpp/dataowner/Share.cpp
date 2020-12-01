/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

#include <mpc/Multiply.h>

#include <dataowner/Share.h>

#include <ff/logging.h>

namespace safrn {
namespace dataowner {

template<typename Number_t, typename Info_t>
void Share<Number_t, Info_t>::handleComplete(Fronctocol &) {
  log_error("Dataowner Share Fronctocol received unexpected "
            "handle complete");
}

template<typename Number_t, typename Info_t>
void Share<Number_t, Info_t>::handlePromise(Fronctocol &) {
  log_error("Dataowner Share Fronctocol received unexpected "
            "handle promise");
}

template<typename Number_t, typename Info_t>
void Share<Number_t, Info_t>::handleReceive(IncomingMessage & imsg) {
  log_debug("Dataowner Share Fronctocol handle receive");
  if (*this->sharer != this->getSelf()) {
    imsg.read<Number_t>(myShare);
    this->complete();
  } else {
    log_error("Dataowner Share Fronctocol did not expect any "
              "incoming messages");
    (void)imsg;
  }
}

template<typename Number_t, typename Info_t>
std::string Share<Number_t, Info_t>::name() {
  return std::string("Share");
}

template<>
void Share<SmallNum, BeaverInfo<SmallNum>>::init() {
  if (*this->sharer == this->getSelf()) {
    this->getPeers().forEachDataowner([this](Identity const & peer) {
      if (peer == this->getSelf()) {
        return;
      }

      std::unique_ptr<OutgoingMessage> omsg(new OutgoingMessage(peer));

      SmallNum send_share = ff::mpc::randomModP(info.modulus);

      this->myShare += (this->info.modulus - send_share);
      this->myShare %= this->info.modulus;

      omsg->write<SmallNum>(send_share);
      this->send(std::move(omsg));
    });
    this->complete();
  }
}

template<>
void Share<Boolean_t, Boolean_t>::init() {
  if (*this->sharer == this->getSelf()) {
    this->getPeers().forEachDataowner([this](Identity const & peer) {
      if (peer == this->getSelf()) {
        return;
      }

      std::unique_ptr<OutgoingMessage> omsg(new OutgoingMessage(peer));

      Boolean_t send_share =
          ff::mpc::randomModP(static_cast<Boolean_t>(2));

      this->myShare =
          static_cast<Boolean_t>(this->myShare + send_share);
      this->myShare %= 2;

      omsg->write<Boolean_t>(send_share);
      this->send(std::move(omsg));
    });
    this->complete();
  }
}

template class Share<SmallNum, BeaverInfo<SmallNum>>;
template class Share<Boolean_t, Boolean_t>;

} // namespace dataowner
} // namespace safrn
