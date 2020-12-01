/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

#ifndef SAFRN_DATAOWNER_REGRESSION_PAYLOAD_COMPUTE_H_
#define SAFRN_DATAOWNER_REGRESSION_PAYLOAD_COMPUTE_H_

/* C and POSIX Headers */

/* C++ Headers */
#include <cstdint>
#include <vector>

/* 3rd Party Headers */

/* Fortissimo Headers */
#include <ff/Fronctocol.h>
#include <ff/Message.h>

#include <mpc/Batch.h>
#include <mpc/Multiply.h>
#include <mpc/Randomness.h>
#include <mpc/RandomnessDealer.h>
#include <mpc/UnboundedFaninOr.h>
#include <mpc/templates.h>

#include <dataowner/RegressionInfo.h>
#include <mpc/AbstractZipReduceFactory.h>

#include <dataowner/RegressionInfo.h>
#include <dataowner/fortissimo.h>
#include <framework/Framework.h>

/* logging configuration */
#include <ff/logging.h>

namespace safrn {
namespace dataowner {

class RegressionPayloadCompute
    : public ff::mpc::ZipReduceFronctocol<SAFRN_TYPES, LargeNum> {
public:
  /*
   * Takes in two Arithmetic vectors (from two observations)
   * and computes all necessary cross product terms
   */
  RegressionPayloadCompute(
      std::vector<LargeNum> && vec1,
      std::vector<LargeNum> && vec2,
      std::unique_ptr<ff::mpc::RandomnessDispenser<
          ff::mpc::BeaverTriple<LargeNum>,
          ff::mpc::BeaverInfo<LargeNum>>> beaverDispenser,
      MultiplyInfo<BeaverInfo<LargeNum>> const * const multiplyInfo,
      RegressionInfo const * const regressionInfo);

  void init() override;
  void handleReceive(IncomingMessage & imsg) override;
  void handleComplete(Fronctocol & f) override;
  void handlePromise(Fronctocol & f) override;

private:
  std::vector<LargeNum> vec1;
  std::vector<LargeNum> vec2;
  std::unique_ptr<ff::mpc::RandomnessDispenser<
      ff::mpc::BeaverTriple<LargeNum>,
      ff::mpc::BeaverInfo<LargeNum>>>
      beaverDispenser;
  MultiplyInfo<BeaverInfo<LargeNum>> const * const multiplyInfo;
  RegressionInfo const * const regressionInfo;

  std::vector<LargeNum> multiplyResults;

  LargeNum accessMatrixShare(size_t i, size_t j, size_t d, size_t d_1);
};

class RegressionPayloadComputeFactory
    : public ff::mpc::ZipReduceFactory<SAFRN_TYPES, LargeNum> {

public:
  RegressionPayloadComputeFactory(
      RegressionInfo const * const info,
      std::unique_ptr<ff::mpc::RandomnessDispenser<
          ff::mpc::BeaverTriple<LargeNum>,
          ff::mpc::BeaverInfo<LargeNum>>> dispenser,
      MultiplyInfo<BeaverInfo<LargeNum>> const * const multiplyInfo) :
      info(info),
      dispenser(std::move(dispenser)),
      multiplyInfo(multiplyInfo) {
  }

  std::unique_ptr<ff::mpc::ZipReduceFronctocol<SAFRN_TYPES, LargeNum>>
  generate(
      std::unique_ptr<ff::mpc::Observation<LargeNum>> o1,
      std::unique_ptr<ff::mpc::Observation<LargeNum>> o2) override;

private:
  RegressionInfo const * const info;
  std::unique_ptr<ff::mpc::RandomnessDispenser<
      ff::mpc::BeaverTriple<LargeNum>,
      ff::mpc::BeaverInfo<LargeNum>>>
      dispenser;
  MultiplyInfo<BeaverInfo<LargeNum>> const * const multiplyInfo;
};

} // namespace dataowner
} // namespace safrn

#define LOG_UNCLUDE
#include <ff/logging.h>

#endif // SAFRN_DATAOWNER_REGRESSION_PAYLOAD_COMPUTE_H_
