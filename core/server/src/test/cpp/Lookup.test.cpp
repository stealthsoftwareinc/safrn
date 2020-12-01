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

#include <dataowner/Lookup.h>
#include <dealer/RandomTableLookup.h>
#include <framework/Framework.h>
#include <framework/TestRunner.h>

#include <dataowner/LookupPatron.h>
#include <dealer/LookupHouse.h>

/* Logging Configuration */
#include <ff/logging.h>

using namespace safrn;

TEST(Lookup, lookup) {
  std::map<Identity, std::unique_ptr<Fronctocol>> test;

  Identity const revealer(
      "EEEEEEEEEEEEEEEEEEEEEEEEEEEEEE01", ROLE_DATAOWNER, 0);
  Identity const dealer(
      "EEEEEEEEEEEEEEEEEEEEEEEEEEEEEE00", ROLE_DEALER, 1);

  const dataowner::LargeNum r_modulus =
      (dataowner::LargeNum(1) << 61) - 1;
  const dataowner::SmallNum table_size = 1000;

  dealer::RandomTableLookupInfo info(r_modulus, table_size);
  ff::mpc::CompareInfo<
      safrn::Identity,
      dataowner::LargeNum,
      dataowner::SmallNum>
      compareInfo(r_modulus, &revealer);

  dataowner::SmallNum value_to_extract = 125;
  dataowner::LargeNum sharev0p0 =
      ff::mpc::randomModP<dataowner::LargeNum>(r_modulus);
  dataowner::LargeNum sharev1p0 =
      (r_modulus + value_to_extract - sharev0p0) % r_modulus;
  std::vector<Boolean_t> outputv0p0;
  std::vector<Boolean_t> outputv1p0;
  dataowner::SmallNum list_entry_offset = 3;

  size_t const bytesPerEntry = 8;

  std::vector<std::vector<Boolean_t>> tableData(
      table_size, std::vector<Boolean_t>(bytesPerEntry));

  for (size_t i = 0; i < table_size; i++) {
    for (size_t j = 0; j < bytesPerEntry; j++) {
      tableData.at(i).at(j) =
          static_cast<Boolean_t>((i + list_entry_offset * j) % 256);
    }
  }

  test[dealer] = std::unique_ptr<Fronctocol>(new Tester(
      [&](Fronctocol * self) {
        std::unique_ptr<Fronctocol> house(
            new dealer::LookupRandomnessHouse(&info, &compareInfo));
        PeerSet ps(self->getPeers());
        self->invoke(std::move(house), ps);
      },
      finishTestOnComplete));

  size_t v0p0_num_fronctocols_remaining = 2;
  test[revealer] = std::unique_ptr<Fronctocol>(new Tester(
      [&](Fronctocol * self) {
        std::unique_ptr<Fronctocol> patron(
            new dataowner::LookupRandomnessPatron(
                &info, &compareInfo, &dealer, 1UL));
        self->invoke(std::move(patron), self->getPeers());
      },
      [&](Fronctocol & f, Fronctocol * self) mutable {
        v0p0_num_fronctocols_remaining--;
        log_debug("Handle complete");
        if (v0p0_num_fronctocols_remaining == 1) {

          dataowner::LookupRandomness randomness(
              static_cast<dataowner::LookupRandomnessPatron &>(f)
                  .lookupDispenser->get());

          std::unique_ptr<Fronctocol> v0p0Lookup(new dataowner::Lookup(
              sharev0p0,
              outputv0p0,
              tableData,
              std::move(randomness),
              &info,
              bytesPerEntry,
              &revealer));
          PeerSet ps(self->getPeers());
          ps.removeDealer();
          self->invoke(std::move(v0p0Lookup), ps);

        } else {
          log_debug("finishing v0p0");
          self->complete();
        }
      }));

  /*

  test[Identity(
      "EEEEEEEEEEEEEEEEEEEEEEEEEEEEEE02", ROLE_DATAOWNER, 0)] =
      std::unique_ptr<Fronctocol>(new Tester(
          [&](Fronctocol * self) {
            std::unique_ptr<Fronctocol> v0p1regression(
                new dataowner::Regression(v0p1_file,
                  &v0_info));
            PeerSet ps(self->getPeers());
            self->invoke(std::move(v0p1regression), ps);
          },
          finishTestOnComplete));
          */

  size_t v1p0_num_fronctocols_remaining = 2;
  test[Identity(
      "EEEEEEEEEEEEEEEEEEEEEEEEEEEEEE03", ROLE_DATAOWNER, 1)] =
      std::unique_ptr<Fronctocol>(new Tester(
          [&](Fronctocol * self) {
            std::unique_ptr<Fronctocol> patron(
                new dataowner::LookupRandomnessPatron(
                    &info, &compareInfo, &dealer, 1UL));
            self->invoke(std::move(patron), self->getPeers());
          },
          [&](Fronctocol & f, Fronctocol * self) mutable {
            v1p0_num_fronctocols_remaining--;
            log_debug("Handle complete");
            if (v1p0_num_fronctocols_remaining == 1) {

              dataowner::LookupRandomness randomness(
                  static_cast<dataowner::LookupRandomnessPatron &>(f)
                      .lookupDispenser->get());

              std::unique_ptr<Fronctocol> v1p0Lookup(
                  new dataowner::Lookup(
                      sharev1p0,
                      outputv1p0,
                      tableData,
                      std::move(randomness),
                      &info,
                      bytesPerEntry,
                      &revealer));
              PeerSet ps(self->getPeers());
              ps.removeDealer();
              self->invoke(std::move(v1p0Lookup), ps);

            } else {
              log_debug("finishing v1p0");
              self->complete();
            }
          }));

  /*
  test[Identity(
      "EEEEEEEEEEEEEEEEEEEEEEEEEEEEEE04", ROLE_DATAOWNER, 1)] =
      std::unique_ptr<Fronctocol>(new Tester(
          [&](Fronctocol * self) {
            std::unique_ptr<Fronctocol> v1p1regression(
                new dataowner::Regression(v1p1_file,
                  &v1_info));
            PeerSet ps(self->getPeers());
            self->invoke(std::move(v1p1regression), ps);
          },
          finishTestOnComplete));
          */

  log_debug("launching tests");

  EXPECT_TRUE(runTests(test));

  for (size_t i = 0; i < bytesPerEntry; i++) {
    EXPECT_EQ(
        (outputv0p0.at(i) ^ outputv1p0.at(i)),
        static_cast<Boolean_t>(
            (value_to_extract + list_entry_offset * i) % 256));
  }
}
