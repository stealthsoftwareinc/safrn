/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* C and POSIX Headers */

/* C++ Headers */
#include <map>
#include <memory>

/* 3rd Party Headers */
#include <gtest/gtest.h>

/* Fortissimo Headers */

#include <mpc/Randomness.h>
#include <mpc/RandomnessDealer.h>

/* SAFRN Headers */
#include <Identity.h>
#include <dealer/RandomSquareMatrix.h>
#include <framework/Framework.h>
#include <framework/TestRunner.h>

/* Logging Configuration */
#include <ff/logging.h>

using namespace ff::mpc;
using namespace safrn::dealer;

TEST(RandomSquareMatrix, DealerGenerate) {
  std::map<safrn::Identity, std::unique_ptr<safrn::Fronctocol>> test;

  RandomSquareMatrixInfo<uint32_t, uint32_t> info(3, 97);
  size_t num_desired = 10;

  std::unique_ptr<safrn::Promise<RandomnessDispenser<
      RandomSquareMatrix<uint32_t>,
      RandomSquareMatrixInfo<uint32_t, uint32_t>>>>
      p1;
  std::unique_ptr<safrn::Promise<RandomnessDispenser<
      RandomSquareMatrix<uint32_t>,
      RandomSquareMatrixInfo<uint32_t, uint32_t>>>>
      p2;

  std::unique_ptr<RandomnessDispenser<
      RandomSquareMatrix<uint32_t>,
      RandomSquareMatrixInfo<uint32_t, uint32_t>>>
      r1;
  std::unique_ptr<RandomnessDispenser<
      RandomSquareMatrix<uint32_t>,
      RandomSquareMatrixInfo<uint32_t, uint32_t>>>
      r2;

  safrn::Identity dealer_id(
      "EEEEEEEEEEEEEEEEEEEEEEEEEEEEEE00", safrn::ROLE_DEALER);
  safrn::Identity alice_id(
      "EEEEEEEEEEEEEEEEEEEEEEEEEEEEEE01", safrn::ROLE_DATAOWNER, 1);
  safrn::Identity bob_id(
      "EEEEEEEEEEEEEEEEEEEEEEEEEEEEEE02", safrn::ROLE_DATAOWNER, 0);

  test[dealer_id] =
      std::unique_ptr<safrn::Fronctocol>(new safrn::Tester(
          [](safrn::Fronctocol * self) {
            log_debug("Starting dealer test");
            std::unique_ptr<safrn::Fronctocol> rd(
                new RandomnessHouse<
                    SAFRN_TYPES,
                    RandomSquareMatrix<uint32_t>,
                    RandomSquareMatrixInfo<uint32_t, uint32_t>>());

            self->invoke(std::move(rd), self->getPeers());
          },
          safrn::finishTestOnComplete));

  test[alice_id] = std::unique_ptr<safrn::Fronctocol>(new safrn::Tester(
      [&](safrn::Fronctocol * self) {
        log_debug("starting alice test");
        std::unique_ptr<safrn::PromiseFronctocol<RandomnessDispenser<
            RandomSquareMatrix<uint32_t>,
            RandomSquareMatrixInfo<uint32_t, uint32_t>>>>
            drg(new RandomnessPatron<
                SAFRN_TYPES,
                RandomSquareMatrix<uint32_t>,
                RandomSquareMatrixInfo<uint32_t, uint32_t>>(
                dealer_id, num_desired, info));

        p1 = self->promise(std::move(drg), self->getPeers());
        self->await(*p1);
      },
      safrn::failTestOnComplete,
      safrn::failTestOnReceive,
      [&](safrn::Fronctocol & f, safrn::Fronctocol * self) {
        log_debug("finishing alice test");
        r1 = p1->getResult(f);
        EXPECT_FALSE(r1 == nullptr);
        self->complete();
      }));

  test[bob_id] = std::unique_ptr<safrn::Fronctocol>(new safrn::Tester(
      [&](safrn::Fronctocol * self) {
        log_debug("starting bob test");
        std::unique_ptr<safrn::PromiseFronctocol<RandomnessDispenser<
            RandomSquareMatrix<uint32_t>,
            RandomSquareMatrixInfo<uint32_t, uint32_t>>>>
            drg(new RandomnessPatron<
                SAFRN_TYPES,
                RandomSquareMatrix<uint32_t>,
                RandomSquareMatrixInfo<uint32_t, uint32_t>>(
                dealer_id, num_desired, info));

        p2 = self->promise(std::move(drg), self->getPeers());
        self->await(*p2);
      },
      safrn::failTestOnComplete,
      safrn::failTestOnReceive,
      [&](safrn::Fronctocol & f, safrn::Fronctocol * self) {
        log_debug("finishing bob test");
        r2 = p2->getResult(f);
        EXPECT_EQ(true, r2 != nullptr);
        self->complete();
      }));

  EXPECT_TRUE(runTests(test));
};
