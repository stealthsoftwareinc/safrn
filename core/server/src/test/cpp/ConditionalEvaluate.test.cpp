/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* C++ Headers */
#include <deque>
#include <map>
#include <memory>

/* 3rd Party Headers */
#include <gtest/gtest.h>
#include <openssl/rand.h>

/* Fortissimo Headers */

#include <framework/Framework.h>
#include <framework/TestRunner.h>

#include <dataowner/RegressionInfo.h>
#include <dataowner/RegressionPayloadCompute.h>
#include <mpc/ConditionalEvaluate.h>

#include <mpc/CompareDealer.h>
#include <mpc/ComparePatron.h>

/* Logging Configuration */
#include <ff/logging.h>

using Boolean_t = uint8_t;
using SmallNum = uint32_t;

using namespace safrn::dataowner;
using namespace safrn;

TEST(SISO_Sort, conditional_evaluate) {

  std::map<safrn::Identity, std::unique_ptr<Fronctocol>> test;

  const size_t batchSize = 100;
  const size_t numParties = 2;

  const size_t numArithmeticPayloadCols = 5;
  const SmallNum maxColValue = 7;

  const size_t firstVertical = 0;
  const size_t secondVertical = 1;
  const size_t indexDV = 1;
  const size_t unusedParseInfo = 0;
  const std::vector<size_t> vecOfLengthTwo(
      2); // size should equal (numIVs + 1)/2
  const std::vector<size_t> vecOfLengthOne(
      1); // size should equal (numIVs - 1)/2
  const size_t numIVs = 3;
  const size_t numXORPayloadCols = 0;
  EXPECT_EQ(
      numIVs % 2,
      1); // just for making the construction as lazy as possible
  Identity const revealer(
      "EEEEEEEEEEEEEEEEEEEEEEEEEEEEEE01", ROLE_DATAOWNER, 0);
  Identity const dealer(
      "EEEEEEEEEEEEEEEEEEEEEEEEEEEEEE00", ROLE_DEALER, 1);

  RegressionInfo const regressionInfoFirstVertical(
      firstVertical,
      indexDV,
      unusedParseInfo,
      unusedParseInfo,
      1,
      vecOfLengthTwo,
      vecOfLengthOne,
      &revealer,
      &dealer);

  RegressionInfo const regressionInfoSecondVertical(
      secondVertical,
      indexDV,
      unusedParseInfo,
      unusedParseInfo,
      1,
      vecOfLengthTwo,
      vecOfLengthOne,
      &revealer,
      &dealer);

  std::vector<SmallNum> plaintext_matrix(9, 0);
  std::vector<SmallNum> plaintext_DV(3, 0);

  std::vector<ff::mpc::ObservationList> oLists;
  oLists.resize(numParties);
  // This only works if numParties == 2 (which is the only value it should take)
  for (size_t i = 0; i < numParties; i++) {
    oLists.at(i).numKeyCols = 2;
    oLists.at(i).numArithmeticPayloadCols = numArithmeticPayloadCols;
    oLists.at(i).numXORPayloadCols = numXORPayloadCols; // 0
    for (size_t j = 0; j < batchSize; j++) {
      ff::mpc::Observation o;
      o.keyCols = {static_cast<SmallNum>((4UL * j) / 5UL),
                   static_cast<SmallNum>(j % 2)};
      std::vector<SmallNum> payloadCols(
          regressionInfoFirstVertical.payloadLength, 0);
      if ((4UL * j) / 5UL == (4UL * j + 4UL) / 5UL && i == 0UL) {
        payloadCols.at(0) = ff::mpc::randomModP<SmallNum>(maxColValue);
        payloadCols.at(1) = ff::mpc::randomModP<SmallNum>(maxColValue);
        payloadCols.at(2) = payloadCols.at(0) * payloadCols.at(0);
        payloadCols.at(3) = payloadCols.at(0) * payloadCols.at(1);
        payloadCols.at(4) = payloadCols.at(1) * payloadCols.at(1);

        plaintext_matrix.at(0) += payloadCols.at(2);
        plaintext_matrix.at(1) += payloadCols.at(3);
        plaintext_matrix.at(4) += payloadCols.at(4);
      } else if ((4 * j) / 5 == (4 * j - 4) / 5 && i == 1) {
        payloadCols.at(0) = ff::mpc::randomModP<SmallNum>(maxColValue);
        payloadCols.at(1) = ff::mpc::randomModP<SmallNum>(maxColValue);
        payloadCols.at(2) = payloadCols.at(0) * payloadCols.at(0);
        payloadCols.at(3) = payloadCols.at(0) * payloadCols.at(1);

        plaintext_matrix.at(8) += payloadCols.at(2);
        plaintext_DV.at(2) += payloadCols.at(3);
      }
      o.arithmeticPayloadCols = payloadCols;
      oLists.at(i).elements.push_back(o);
    }
  }

  for (size_t j = 0; j < batchSize; j++) {
    if ((4 * j) / 5 == (4 * j - 4) / 5) {
      plaintext_matrix.at(2) +=
          oLists.at(0).elements.at(j - 1).arithmeticPayloadCols.at(0) *
          oLists.at(1).elements.at(j).arithmeticPayloadCols.at(0);
      plaintext_matrix.at(5) +=
          oLists.at(0).elements.at(j - 1).arithmeticPayloadCols.at(1) *
          oLists.at(1).elements.at(j).arithmeticPayloadCols.at(0);

      plaintext_DV.at(0) +=
          oLists.at(0).elements.at(j - 1).arithmeticPayloadCols.at(0) *
          oLists.at(1).elements.at(j).arithmeticPayloadCols.at(1);
      plaintext_DV.at(1) +=
          oLists.at(0).elements.at(j - 1).arithmeticPayloadCols.at(1) *
          oLists.at(1).elements.at(j).arithmeticPayloadCols.at(1);
    }
  }

  plaintext_matrix.at(3) = plaintext_matrix.at(1);
  plaintext_matrix.at(6) = plaintext_matrix.at(2);
  plaintext_matrix.at(7) = plaintext_matrix.at(5);

  std::vector<ff::mpc::ObservationList> outputLists;
  outputLists.resize(numParties);

  log_debug(
      "Start prime: %u, end prime: %u",
      regressionInfoFirstVertical.startModulus,
      regressionInfoFirstVertical.endModulus);
  log_debug(
      "payloadLength %zu", regressionInfoFirstVertical.payloadLength);

  ff::mpc::ConditionalEvaluateInfo<safrn::Identity> const * const
      conditionalEvaluateInfo =
          ff::mpc::generateConditionalEvaluateInfo(
              batchSize,
              numArithmeticPayloadCols,
              numXORPayloadCols,
              regressionInfoFirstVertical.startModulus,
              &revealer);

  ff::mpc::CompareInfo<safrn::Identity> const * const compareInfo =
      ff::mpc::generateCompareInfo(
          regressionInfoFirstVertical.startModulus, &revealer);
  ff::mpc::
      MultiplyInfo<safrn::Identity, ff::mpc::ArithmeticBeaverInfo> const
          multiplyInfo(
              &revealer,
              ff::mpc::ArithmeticBeaverInfo(
                  regressionInfoFirstVertical.startModulus));
  ff::mpc::
      MultiplyInfo<safrn::Identity, ff::mpc::BooleanBeaverInfo> const
          booleanMultiplyInfo(&revealer, ff::mpc::BooleanBeaverInfo());

  ff::mpc::ConditionalEvaluateInfo<safrn::Identity> const * const
      alternateInfo =
          new ff::mpc::ConditionalEvaluateInfo<safrn::Identity>(
              batchSize,
              numArithmeticPayloadCols,
              numXORPayloadCols,
              compareInfo,
              &multiplyInfo,
              &booleanMultiplyInfo);

  std::vector<std::unique_ptr<ff::mpc::RandomnessDispenser<
      ff::mpc::TypeCastTriple,
      ff::mpc::TypeCastFromBitInfo>>>
      tctDispensers;
  std::vector<std::unique_ptr<ff::mpc::RandomnessDispenser<
      ff::mpc::BeaverTriple<SmallNum>,
      ff::mpc::ArithmeticBeaverInfo>>>
      arithmeticBeaverDispensers;
  std::vector<std::unique_ptr<ff::mpc::RandomnessDispenser<
      ff::mpc::BeaverTriple<Boolean_t>,
      ff::mpc::BooleanBeaverInfo>>>
      XORBeaverDispensers;

  for (size_t i = 0; i < numParties; i++) {
    tctDispensers.emplace_back(
        std::unique_ptr<ff::mpc::RandomnessDispenser<
            ff::mpc::TypeCastTriple,
            ff::mpc::TypeCastFromBitInfo>>(
            new ff::mpc::RandomnessDispenser<
                ff::mpc::TypeCastTriple,
                ff::mpc::TypeCastFromBitInfo>(
                ff::mpc::TypeCastFromBitInfo(
                    regressionInfoFirstVertical.startModulus))));
    arithmeticBeaverDispensers.emplace_back(
        std::unique_ptr<ff::mpc::RandomnessDispenser<
            ff::mpc::BeaverTriple<SmallNum>,
            ff::mpc::ArithmeticBeaverInfo>>(
            new ff::mpc::RandomnessDispenser<
                ff::mpc::BeaverTriple<SmallNum>,
                ff::mpc::ArithmeticBeaverInfo>(multiplyInfo.info)));
    XORBeaverDispensers.emplace_back(
        std::unique_ptr<ff::mpc::RandomnessDispenser<
            ff::mpc::BeaverTriple<Boolean_t>,
            ff::mpc::BooleanBeaverInfo>>(
            new ff::mpc::RandomnessDispenser<
                ff::mpc::BeaverTriple<Boolean_t>,
                ff::mpc::BooleanBeaverInfo>(
                ff::mpc::BooleanBeaverInfo())));
  }

  size_t tctsNeeded = batchSize - 1;
  size_t arithmeticBeaverTriplesNeeded = 2 * (batchSize - 1) *
          (regressionInfoFirstVertical.payloadLength) +
      (batchSize - 1) * ((numIVs + 1) / 2) * ((numIVs + 1) / 2);
  size_t XORBeaverTriplesNeeded =
      2 * (batchSize - 1) * (numXORPayloadCols);

  log_debug(
      "%zu tctsNeeded, %zu arithmeticBeaverTriplesNeeded, %zu "
      "XORBeaverTriplesNeeded",
      tctsNeeded,
      arithmeticBeaverTriplesNeeded,
      XORBeaverTriplesNeeded);

  for (size_t i = 0; i < tctsNeeded; i++) {
    std::vector<ff::mpc::TypeCastTriple> tcts;
    ff::mpc::TypeCastFromBitInfo(
        regressionInfoFirstVertical.startModulus)
        .generate(numParties, 1, tcts);
    for (size_t j = 0; j < numParties; j++) {
      tctDispensers.at(j)->insert(tcts.at(j));
    }
  }

  for (size_t i = 0; i < arithmeticBeaverTriplesNeeded; i++) {
    std::vector<ff::mpc::BeaverTriple<SmallNum>> beavers;
    multiplyInfo.info.generate(numParties, 1, beavers);
    for (size_t j = 0; j < numParties; j++) {
      arithmeticBeaverDispensers.at(j)->insert(beavers.at(j));
    }
  }

  for (size_t i = 0; i < XORBeaverTriplesNeeded; i++) {
    std::vector<ff::mpc::BeaverTriple<Boolean_t>> beavers;
    ff::mpc::BooleanBeaverInfo().generate(numParties, 1, beavers);
    for (size_t j = 0; j < numParties; j++) {
      XORBeaverDispensers.at(j)->insert(beavers.at(j));
    }
  }

  std::vector<RegressionPayloadComputeFactory> fronctocolFactories;
  for (size_t i = 0; i < numParties / 2; i++) {
    fronctocolFactories.emplace_back(
        &regressionInfoFirstVertical,
        std::move(arithmeticBeaverDispensers.at(i)->littleDispenser(
            (batchSize - 1) *
            regressionInfoFirstVertical
                .columnIndices_IV_OppFrom_DV_Vertical.size() *
            (regressionInfoFirstVertical
                 .columnIndices_IV_SameAs_DV_Vertical.size() +
             1))),
        &multiplyInfo);
  }
  for (size_t i = numParties / 2; i < numParties; i++) {
    fronctocolFactories.emplace_back(
        &regressionInfoSecondVertical,
        std::move(arithmeticBeaverDispensers.at(i)->littleDispenser(
            (batchSize - 1) *
            regressionInfoFirstVertical
                .columnIndices_IV_OppFrom_DV_Vertical.size() *
            (regressionInfoFirstVertical
                 .columnIndices_IV_SameAs_DV_Vertical.size() +
             1))),
        &multiplyInfo);
  }

  test[dealer] = std::unique_ptr<Fronctocol>(new Tester(
      [&](Fronctocol * self) {
        log_debug("Starting dealer test");

        std::unique_ptr<Fronctocol> rd2(
            new ff::mpc::CompareRandomnessHouse<SAFRN_TYPES>(
                compareInfo));
        self->invoke(std::move(rd2), self->getPeers());
      },
      [&](Fronctocol &, Fronctocol * self) { self->complete(); }));

  size_t income1_num_fronctocols_remaining = 2;
  test[revealer] = std::unique_ptr<Fronctocol>(new Tester(
      [&](Fronctocol * self) {
        log_debug("starting mpc income1");
        std::unique_ptr<Fronctocol> patron(
            new ff::mpc::CompareRandomnessPatron<SAFRN_TYPES>(
                compareInfo, &dealer, batchSize - 1));
        self->invoke(std::move(patron), self->getPeers());
      },
      [&](Fronctocol & f, Fronctocol * self) mutable {
        income1_num_fronctocols_remaining--;
        log_debug(
            "income1_num_fronctocols_remaining %lu",
            income1_num_fronctocols_remaining);
        if (income1_num_fronctocols_remaining == 1) {
          std::unique_ptr<ff::mpc::RandomnessDispenser<
              ff::mpc::CompareRandomness,
              ff::mpc::DoNotGenerateInfo>>
              compareDispenser = std::move(
                  static_cast<
                      ff::mpc::CompareRandomnessPatron<SAFRN_TYPES> &>(
                      f)
                      .compareDispenser);

          std::unique_ptr<ff::mpc::BatchConditionalEvaluate<
              SAFRN_TYPES,
              RegressionInfo,
              ff::mpc::BeaverTriple<SmallNum>,
              ff::mpc::ArithmeticBeaverInfo>>
              batchEval(new ff::mpc::BatchConditionalEvaluate<
                        SAFRN_TYPES,
                        RegressionInfo,
                        ff::mpc::BeaverTriple<SmallNum>,
                        ff::mpc::ArithmeticBeaverInfo>(
                  oLists.at(0),
                  conditionalEvaluateInfo,
                  std::move(ff::mpc::ConditionalEvaluateRandomness(
                      std::move(compareDispenser),
                      std::move(tctDispensers.at(0)),
                      std::move(arithmeticBeaverDispensers.at(0)),
                      std::move(XORBeaverDispensers.at(0)))),
                  fronctocolFactories.at(0)));

          PeerSet ps(self->getPeers());
          ps.remove(dealer);
          self->invoke(std::move(batchEval), ps);
        } else {
          log_debug("finishing mpc income1");
          ff::mpc::BatchConditionalEvaluate<
              SAFRN_TYPES,
              RegressionInfo,
              ff::mpc::BeaverTriple<SmallNum>,
              ff::mpc::ArithmeticBeaverInfo> & batchEval =
              static_cast<ff::mpc::BatchConditionalEvaluate<
                  SAFRN_TYPES,
                  RegressionInfo,
                  ff::mpc::BeaverTriple<SmallNum>,
                  ff::mpc::ArithmeticBeaverInfo> &>(f);
          outputLists.at(0) = std::move(batchEval.outputList);

          self->complete();
        }
      },
      failTestOnReceive,
      failTestOnPromise));

  Identity const univ1(
      "EEEEEEEEEEEEEEEEEEEEEEEEEEEEEE02", ROLE_DATAOWNER, 1);
  size_t univ1_num_fronctocols_remaining = 2;
  test[univ1] = std::unique_ptr<Fronctocol>(new Tester(
      [&](Fronctocol * self) {
        log_debug("starting mpc univ1");
        std::unique_ptr<Fronctocol> patron(
            new ff::mpc::CompareRandomnessPatron<SAFRN_TYPES>(
                compareInfo, &dealer, batchSize - 1));
        self->invoke(std::move(patron), self->getPeers());
      },
      [&](Fronctocol & f, Fronctocol * self) mutable {
        univ1_num_fronctocols_remaining--;
        log_debug(
            "univ1_num_fronctocols_remaining %lu",
            univ1_num_fronctocols_remaining);
        if (univ1_num_fronctocols_remaining == 1) {
          std::unique_ptr<ff::mpc::RandomnessDispenser<
              ff::mpc::CompareRandomness,
              ff::mpc::DoNotGenerateInfo>>
              compareDispenser = std::move(
                  static_cast<
                      ff::mpc::CompareRandomnessPatron<SAFRN_TYPES> &>(
                      f)
                      .compareDispenser);

          std::unique_ptr<ff::mpc::BatchConditionalEvaluate<
              SAFRN_TYPES,
              RegressionInfo,
              ff::mpc::BeaverTriple<SmallNum>,
              ff::mpc::ArithmeticBeaverInfo>>
              batchEval(new ff::mpc::BatchConditionalEvaluate<
                        SAFRN_TYPES,
                        RegressionInfo,
                        ff::mpc::BeaverTriple<SmallNum>,
                        ff::mpc::ArithmeticBeaverInfo>(
                  oLists.at(1),
                  alternateInfo,
                  std::move(ff::mpc::ConditionalEvaluateRandomness(
                      std::move(compareDispenser),
                      std::move(tctDispensers.at(1)),
                      std::move(arithmeticBeaverDispensers.at(1)),
                      std::move(XORBeaverDispensers.at(1)))),
                  fronctocolFactories.at(1)));

          PeerSet ps(self->getPeers());
          ps.remove(dealer);
          self->invoke(std::move(batchEval), ps);
        } else {
          log_debug("finishing mpc univ1");
          ff::mpc::BatchConditionalEvaluate<
              SAFRN_TYPES,
              RegressionInfo,
              ff::mpc::BeaverTriple<SmallNum>,
              ff::mpc::ArithmeticBeaverInfo> & batchEval =
              static_cast<ff::mpc::BatchConditionalEvaluate<
                  SAFRN_TYPES,
                  RegressionInfo,
                  ff::mpc::BeaverTriple<SmallNum>,
                  ff::mpc::ArithmeticBeaverInfo> &>(f);
          outputLists.at(1) = std::move(batchEval.outputList);

          self->complete();
        }
      },
      failTestOnReceive,
      failTestOnPromise));

  log_debug("launching tests");

  EXPECT_TRUE(runTests(test));

  std::vector<SmallNum> resultMatrix(9);
  std::vector<SmallNum> resultVector(3);
  log_debug(
      "Output size: %zu",
      outputLists.at(0).elements.at(0).arithmeticPayloadCols.size());
  for (size_t i = 0; i < batchSize - 1; i++) {
    for (size_t j = 0; j < 9; j++) {
      resultMatrix.at(j) +=
          outputLists.at(0).elements.at(i).arithmeticPayloadCols.at(j) +
          outputLists.at(1).elements.at(i).arithmeticPayloadCols.at(j);
      resultMatrix.at(j) %= regressionInfoFirstVertical.startModulus;
    }
    for (size_t j = 0; j < 3; j++) {
      resultVector.at(j) +=
          outputLists.at(0).elements.at(i).arithmeticPayloadCols.at(
              9 + j) +
          outputLists.at(1).elements.at(i).arithmeticPayloadCols.at(
              9 + j);
      resultVector.at(j) %= regressionInfoFirstVertical.startModulus;
    }
  }
  for (size_t i = 0; i < 9; i++) {
    log_debug(
        "matrix[%zu][%zu] = %u", i / 3, i % 3, resultMatrix.at(i));
    log_debug("plaintext_matrix = %u", plaintext_matrix.at(i));
    EXPECT_EQ(
        resultMatrix.at(i),
        plaintext_matrix.at(i) %
            regressionInfoFirstVertical.startModulus);
  }
  for (size_t i = 0; i < 3; i++) {
    log_debug("vector[%zu] = %u", i, resultVector.at(i));
    log_debug("plaintext_DV = %u", plaintext_DV.at(i));
    EXPECT_EQ(
        resultVector.at(i),
        plaintext_DV.at(i) % regressionInfoFirstVertical.startModulus);
  }
}
