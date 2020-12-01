/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */
#ifndef SAFRN_RECIPIENT_MOMENTS_H_
#define SAFRN_RECIPIENT_MOMENTS_H_

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <cstdio>
#include <string>
#include <vector>

#include <dataowner/Moments.h>
#include <dataowner/MomentsInfo.h>
#include <dataowner/fortissimo.h>
#include <framework/Framework.h>

/* Logging Configuration */
#include <ff/logging.h>

namespace safrn {
namespace recipient {

void MomentsPrettyPrint(
    std::vector<dataowner::SmallNum> const & results,
    const bool includeCount,
    size_t const bits_of_precision);

class MomentsReceiver : public Fronctocol {
public:
  size_t numDataowners = 0;
  size_t bitsOfPrecision;

  std::unique_ptr<dataowner::MomentsInfo const> m_info;

  std::vector<dataowner::LargeNum> results;

  MomentsReceiver(
      const size_t bits,
      std::unique_ptr<dataowner::MomentsInfo const> i) :
      bitsOfPrecision(bits),
      m_info(std::move(i)),
      results(
          m_info->highest_moment +
          (m_info->includeZerothMoment ? 1 : 0)) {
  }

  void init() override;
  void handleReceive(IncomingMessage & imsg) override;
  void handleComplete(Fronctocol &) override;
  void handlePromise(Fronctocol &) override;
  std::string name() override;
};

} // namespace recipient
} // namespace safrn

#endif // SAFRN_RECIPIENT_MOMENTS_H_
