/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

#ifndef SAFRN_DEALER_LOOKUP_HOUSE_H_
#define SAFRN_DEALER_LOOKUP_HOUSE_H_

/* C and POSIX Headers */

/* C++ Headers */
#include <cmath>
#include <cstdint>
#include <list>
#include <string>
#include <vector>

/* 3rd Party Headers */

/* Fortissimo Headers */
#include <dataowner/fortissimo.h>
#include <ff/Fronctocol.h>
#include <ff/Message.h>

#include <mpc/Randomness.h>
#include <mpc/RandomnessDealer.h>
#include <mpc/templates.h>

#include <mpc/Compare.h>
#include <mpc/CompareDealer.h>
#include <mpc/ModConvUp.h>

#include <dealer/RandomTableLookup.h>

#include <dataowner/fortissimo.h>
#include <framework/Framework.h>

/* logging configuration */
#include <ff/logging.h>

namespace safrn {
namespace dealer {

class LookupRandomnessHouse : public Fronctocol {
public:
  void init() override;
  void handleReceive(IncomingMessage & imsg) override;
  void handleComplete(Fronctocol & f) override;
  void handlePromise(Fronctocol & f) override;
  std::string name() override;

  /** we only pass in the identity of the revealer because it's needed in the
    * constructor for CompareInfo, the House object doesn't actually need it
    */
  LookupRandomnessHouse(
      RandomTableLookupInfo const * const info,
      ff::mpc::CompareInfo<
          safrn::Identity,
          dataowner::LargeNum,
          dataowner::SmallNum> const * const compareInfo);

private:
  RandomTableLookupInfo const * const info;
  ff::mpc::CompareInfo<
      safrn::Identity,
      dataowner::LargeNum,
      dataowner::SmallNum> const * const compareInfo;

  size_t numDealersRemaining = 0;
};

} // namespace dealer
} // namespace safrn

#define LOG_UNCLUDE
#include <ff/logging.h>

#endif //SAFRN_DEALER_LOOKUP_HOUSE_H_
