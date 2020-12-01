/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

#include <dataowner/RegressionPayloadCompute.h>

#include <ff/logging.h>

namespace safrn {
namespace dataowner {

RegressionPayloadCompute::RegressionPayloadCompute(
    std::vector<LargeNum> && vec1,
    std::vector<LargeNum> && vec2,
    std::unique_ptr<ff::mpc::RandomnessDispenser<
        ff::mpc::BeaverTriple<LargeNum>,
        ff::mpc::BeaverInfo<LargeNum>>> beaverDispenser,
    MultiplyInfo<BeaverInfo<LargeNum>> const * const multiplyInfo,
    RegressionInfo const * const regressionInfo) :
    vec1(std::move(vec1)),
    vec2(std::move(vec2)),
    beaverDispenser(std::move(beaverDispenser)),
    multiplyInfo(multiplyInfo),
    regressionInfo(regressionInfo) {
}

void RegressionPayloadCompute::init() {
  //log_debug("Calling init on RegressionPayloadCompute");

  std::unique_ptr<ff::mpc::Batch<SAFRN_TYPES>> batch(
      new ff::mpc::Batch<SAFRN_TYPES>());

  this->multiplyResults.resize(
      regressionInfo->verticalNonDV_numIVs *
      (regressionInfo->verticalDV_numIVs + 1));
  for (size_t i = 0; i < regressionInfo->verticalNonDV_numIVs; i++) {
    for (size_t j = 0; j < regressionInfo->verticalDV_numIVs + 1; j++) {
      //log_debug("i %zu, j %zu",i,j);
      //log_debug("vec1 length %zu, vec2 length %zu", this->vec1.size(),this->vec2.size());

      batch->children.emplace_back(new ff::mpc::Multiply<
                                   SAFRN_TYPES,
                                   LargeNum,
                                   ff::mpc::BeaverInfo<LargeNum>>(
          this->vec1[i],
          this->vec2[j],
          &this->multiplyResults
               [i * (regressionInfo->verticalDV_numIVs + 1) + j],
          this->beaverDispenser->get(),
          this->multiplyInfo));
    }
  }
  this->invoke(std::move(batch), this->getPeers());
}

void RegressionPayloadCompute::handlePromise(Fronctocol &) {
  log_error("Unexpected handlePromise in RegressionPayloadCompute");
}

void RegressionPayloadCompute::handleReceive(IncomingMessage &) {
  log_error("Unexpected handleReceive in RegressionPayloadCompute");
}

void RegressionPayloadCompute::handleComplete(Fronctocol &) {
  log_debug("Calling handleComplete");
  size_t d = this->regressionInfo->num_IVs;
  size_t d_1 = this->regressionInfo->verticalNonDV_numIVs;
  this->output.arithmeticPayloadCols =
      std::vector<LargeNum>(d * d + d + 3);
  for (size_t i = 0; i < d; i++) {
    for (size_t j = 0; j < d; j++) {
      this->output.arithmeticPayloadCols[i * d + j] =
          accessMatrixShare(i, j, d, d_1);
    }
  }

  for (size_t i = 0; i < d_1; i++) {
    this->output.arithmeticPayloadCols[d * d + i] =
        this->multiplyResults
            [i * (regressionInfo->verticalDV_numIVs + 1) + d - d_1];
  }
  for (size_t i = d_1; i < d; i++) {
    this->output.arithmeticPayloadCols[d * d + i] =
        this->vec2
            [(d - d_1 + 1) + (d - d_1) * (d - d_1 + 1) / 2 +
             (i - d_1)]; // see above;
  }

  size_t d2_offset = (d - d_1 + 1) + ((d - d_1) * (d - d_1 + 3)) / 2;
  // y*y
  this->output.arithmeticPayloadCols[d * d + d] = this->vec2[d2_offset];
  //y
  this->output.arithmeticPayloadCols[d * d + d + 1] =
      this->vec2[d2_offset + 1];
  // 1
  this->output.arithmeticPayloadCols[d * d + d + 2] =
      this->vec2[d2_offset + 2];

  this->complete();
}

LargeNum RegressionPayloadCompute::accessMatrixShare(
    size_t i, size_t j, size_t d, size_t d_1) {
  //log_debug("Calling accessMatrixShare on %zu, %zu, %zu, %zu",i,j, d, d_1);
  if (i > j) {
    return accessMatrixShare(j, i, d, d_1);
  } else if (i < d_1 && j < d_1) {
    return this->vec1
        [d_1 + d_1 * (d_1 + 1) / 2 - (d_1 - i) * (d_1 - i + 1) / 2 +
         (j - i)]; // (d_1 + d_1-1 + ... + (d_1 - j + 1))+(i-j)
  } else if (i < d_1 && j >= d_1) {
    return this->multiplyResults
        [i * (regressionInfo->verticalDV_numIVs + 1) + (j - d_1)];
  } else {
    return this->vec2
        [(d - d_1 + 1) + (d - d_1) * (d - d_1 + 1) / 2 -
         (d - i) * (d - i + 1) / 2 + (j - i)]; // see above
  }
}

std::unique_ptr<ff::mpc::ZipReduceFronctocol<SAFRN_TYPES, LargeNum>>
RegressionPayloadComputeFactory::generate(
    std::unique_ptr<ff::mpc::Observation<LargeNum>> o1,
    std::unique_ptr<ff::mpc::Observation<LargeNum>> o2) {
  log_debug("HI");
  log_debug(
      "o1.arithmeticPayloadCols.size() %zu, "
      "o2.arithmeticPayloadCols.size() %zu",
      o1->arithmeticPayloadCols.size(),
      o2->arithmeticPayloadCols.size());
  return std::move(std::unique_ptr<
                   ff::mpc::ZipReduceFronctocol<SAFRN_TYPES, LargeNum>>(
      new RegressionPayloadCompute(
          std::move(o1->arithmeticPayloadCols),
          std::move(o2->arithmeticPayloadCols),
          std::move(this->dispenser->littleDispenser(
              this->info->verticalNonDV_numIVs *
              (this->info->verticalDV_numIVs + 1))),
          multiplyInfo,
          this->info)));
}

} // namespace dataowner
} // namespace safrn
