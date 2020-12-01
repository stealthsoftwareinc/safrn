/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* C++ Headers */
#include <deque>
#include <map>
#include <memory>

/* 3rd Party Headers */
#include <gtest/gtest.h>
#include <mpc/templates.h>
#include <openssl/rand.h>

/* Safrn Headers */

#include <dataowner/GlobalInfo.h>
#include <dataowner/Moments.h>
#include <dataowner/MomentsInfo.h>
#include <dealer/MomentsHouse.h>
#include <framework/Framework.h>
#include <framework/TestRunner.h>
#include <recipient/MomentsReceiver.h>

#include <QueryTester.h>
/* Logging Configuration */
#include <ff/logging.h>

using namespace safrn;

TEST(Moments, moments) {
  std::vector<double> res;
  EXPECT_TRUE(testQuery("moments_query.json", res, TEST_4_PARTY));

  //  std::map<Identity, std::unique_ptr<Fronctocol>> test;
  //
  //  Identity const revealer(
  //      "EEEEEEEEEEEEEEEEEEEEEEEEEEEEEE01", ROLE_DATAOWNER, 0);
  //  Identity const dealer(
  //      "EEEEEEEEEEEEEEEEEEEEEEEEEEEEEE00", ROLE_DEALER, 1);
  //
  //  const std::string v0p0_file =
  //      "../../../../../server/src/test/data/vert_0.csv";
  //  //  const std::string v0p1_file =  "../../../../../server/src/test/data/vertical_0_party_1.csv";
  //  const std::string v1p0_file =
  //      //    "../../../../../server/src/test/data/vert_1.csv";
  //      "../../../../../server/src/test/data/"
  //      "vert_1_with_extra_ones_column.csv";
  //  //  const std::string v1p1_file =   "../../../../../server/src/test/data/vertical_1_party_1.csv";
  //
  //  size_t firstVertical = 0;
  //  size_t secondVertical = 1;
  //  size_t dataVertical = 0;
  //  size_t columnIndexData = 1;
  //  size_t columnIndexKey = 0;
  //  size_t numCrossParties = 1;
  //  bool includeZerothMoment = true;
  //  size_t numMoments = 4;
  //
  //  dataowner::GlobalInfo globals = dataowner::GlobalInfo();
  //
  //  dataowner::MomentsInfo const v0_info(
  //      &globals,
  //      firstVertical,
  //      dataVertical,
  //      columnIndexData,
  //      columnIndexKey,
  //      includeZerothMoment,
  //      numMoments);
  //
  //  dataowner::MomentsInfo const v1_info(
  //      &globals,
  //      secondVertical,
  //      dataVertical,
  //      columnIndexData,
  //      columnIndexKey,
  //      includeZerothMoment,
  //      numMoments);
  //
  //  test[dealer] = std::unique_ptr<Fronctocol>(new Tester(
  //      [&](Fronctocol * self) {
  //        std::unique_ptr<Fronctocol> house(
  //            new dealer::MomentsRandomnessHouse(
  //                &globals,
  //                std::unique_ptr<dataowner::MomentsInfo const>(&v0_info)));
  //        PeerSet ps(self->getPeers());
  //        self->invoke(std::move(house), ps);
  //      },
  //      finishTestOnComplete));
  //
  //  ff::mpc::ObservationList oList;
  //
  //  if (!safrn::readCSV(
  //          v0p0_file,
  //          oList,
  //          keys,
  //          (leftVertical == id.vertical) ? left_payloads :
  //                                          right_payloads,
  //          scfg,
  //          id,
  //          rinfo->startModulus,
  //          rinfo->bitsOfPrecision)) {
  //    return nullptr;
  //  }
  //
  //  test[revealer] = std::unique_ptr<Fronctocol>(new Tester(
  //      [&](Fronctocol * self) {
  //        std::unique_ptr<Fronctocol> v0p0Moments(
  //            new dataowner::Moments(olist, &globals, &v0_info));
  //        PeerSet ps(self->getPeers());
  //        self->invoke(std::move(v0p0Moments), ps);
  //      },
  //      finishTestOnComplete));
  //
  //  /*
  //
  //  test[Identity(
  //      "EEEEEEEEEEEEEEEEEEEEEEEEEEEEEE02", ROLE_DATAOWNER, 0)] =
  //      std::unique_ptr<Fronctocol>(new Tester(
  //          [&](Fronctocol * self) {
  //            std::unique_ptr<Fronctocol> v0p1Moments(
  //                new dataowner::Moments(v0p1_file,
  //                  &v0_info));
  //            PeerSet ps(self->getPeers());
  //            self->invoke(std::move(v0p1Moments), ps);
  //          },
  //          finishTestOnComplete));
  //          */
  //
  //  test[Identity(
  //      "EEEEEEEEEEEEEEEEEEEEEEEEEEEEEE03", ROLE_DATAOWNER, 1)] =
  //      std::unique_ptr<Fronctocol>(new Tester(
  //          [&](Fronctocol * self) {
  //            std::unique_ptr<Fronctocol> v1p0Moments(
  //                new dataowner::Moments(v1p0_file, &globals, &v1_info));
  //            PeerSet ps(self->getPeers());
  //            self->invoke(std::move(v1p0Moments), ps);
  //          },
  //          finishTestOnComplete));
  //
  //  /*
  //  test[Identity(
  //      "EEEEEEEEEEEEEEEEEEEEEEEEEEEEEE04", ROLE_DATAOWNER, 1)] =
  //      std::unique_ptr<Fronctocol>(new Tester(
  //          [&](Fronctocol * self) {
  //            std::unique_ptr<Fronctocol> v1p1Moments(
  //                new dataowner::Moments(v1p1_file,
  //                  &v1_info));
  //            PeerSet ps(self->getPeers());
  //            self->invoke(std::move(v1p1Moments), ps);
  //          },
  //          finishTestOnComplete));
  //          */
  //
  //  test[Identity(
  //      "EEEEEEEEEEEEEEEEEEEEEEEEEEEEEE05", ROLE_RECIPIENT, SIZE_MAX)] =
  //      std::unique_ptr<Fronctocol>(new Tester(
  //          [&](Fronctocol * self) {
  //            std::unique_ptr<Fronctocol> recipient(
  //                new recipient::MomentsReceiver(
  //                    v0_info.endModulus,
  //                    v0_info.highest_moment,
  //                    v0_info.includeZerothMoment,
  //                    globals.bitsOfPrecision));
  //            PeerSet ps(self->getPeers());
  //            self->invoke(std::move(recipient), ps);
  //          },
  //          finishTestOnComplete));
  //
  //  log_debug("launching tests");
  //
  //  EXPECT_TRUE(runTests(test));
  EXPECT_TRUE(true); // PHB
}
