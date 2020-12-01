/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

#ifndef SAFRN_DEALER_MOMENTS_HOUSE_H_
#define SAFRN_DEALER_MOMENTS_HOUSE_H_

/* C and POSIX Headers */

/* C++ Headers */
#include <cmath>
#include <cstdint>
#include <list>
#include <memory>
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

#include <dataowner/GlobalInfo.h>
#include <dataowner/MomentsInfo.h>

#include <mpc/DivideDealer.h>
#include <mpc/ModConvUpDealer.h>
#include <mpc/ZipAdjacentDealer.h>

#include <dealer/RandomSquareMatrix.h>

#include <framework/Framework.h>
#include <mpc/SISOSortDealer.h>

/* logging configuration */
#include <ff/logging.h>

namespace safrn {
namespace dealer {

class MomentsRandomnessHouse : public Fronctocol {
public:
  void init() override;
  void handleReceive(IncomingMessage & imsg) override;
  void handleComplete(Fronctocol & f) override;
  void handlePromise(Fronctocol & f) override;
  std::string name() override;

  MomentsRandomnessHouse(
      dataowner::GlobalInfo const * const g_info,
      std::unique_ptr<dataowner::MomentsInfo const> m_info);

private:
  dataowner::GlobalInfo const * const globals;
  std::unique_ptr<dataowner::MomentsInfo const> info;

  size_t numDealersRemaining = 0;
};

class MomentsRandomnessBasement : public Fronctocol {
public:
  void init() override;
  void handleReceive(IncomingMessage & imsg) override;
  void handleComplete(Fronctocol & f) override;
  void handlePromise(Fronctocol & f) override;
  std::string name() override;

  MomentsRandomnessBasement(dataowner::MomentsInfo const * const info);

private:
  dataowner::MomentsInfo const * const info;

  size_t numDealersRemaining = 0;
};

} // namespace dealer
} // namespace safrn

#define LOG_UNCLUDE
#include <ff/logging.h>

#endif //SAFRN_DEALER_MOMENTS_HOUSE_H_
