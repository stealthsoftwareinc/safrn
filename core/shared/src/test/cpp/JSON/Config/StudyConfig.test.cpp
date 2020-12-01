/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* platform-specific includes */

/* c/c++ standard includes */
#include <fstream>
#include <string>

/* third-party library includes */
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <sst/rep/from_hex.h>

/* project-specific includes */

/* same module include */
#include <JSON/Columns/CategoricalColumn.h>
#include <JSON/Columns/IntegerColumn.h>
#include <JSON/Columns/RealColumn.h>
#include <JSON/Config/StudyConfig.h>
#include <JSON/Query/MomentFunction.h>
#include <Util/Utils.h>

using namespace std;

namespace {
vector<unsigned char> StringToCharVector(const string & input) {
  vector<unsigned char> to_return;
  sst::rep::from_hex(
      input.c_str(),
      input.c_str() + input.size(),
      back_inserter(to_return));
  return to_return;
}
} // namespace

TEST(StudyConfig, Initialization) {
  OpenSSL_add_all_algorithms();
  std::ifstream studyFile(
      "../../../../../shared/src/test/cfgs/study.json");
  std::ifstream peersFile(
      "../../../../../shared/src/test/cfgs/peers.json");

  if (!studyFile.is_open()) {
    struct CannotOpenFileException : std::exception {
      const char * what() const noexcept override {
        return "Study file cannot be opened";
      }
    };
    throw CannotOpenFileException();
  }
  if (!peersFile.is_open()) {
    struct CannotOpenFileException : std::exception {
      const char * what() const noexcept override {
        return "Peers file cannot be opened";
      }
    };
    throw CannotOpenFileException();
  }

  const nlohmann::json studyJson = nlohmann::json::parse(studyFile);
  const nlohmann::json peersJson = nlohmann::json::parse(peersFile);

  // Read study.json and peers.json into 'target'.
  safrn::StudyConfig target =
      safrn::readStudyFromJson(studyJson, peersJson);

  // Test that the json files were parsed correctly.
  //   - Test outermost/high-level member variables.
  safrn::dbuid_t tmp;
  safrn::strToDbuid("ffffffffffffffffffffffffffffffff", tmp);
  EXPECT_EQ(target.studyId, tmp);
  EXPECT_EQ(target.lexicon.size(), 2);
  EXPECT_EQ(target.allowedQueries.size(), 2);
  EXPECT_EQ(target.peers.size(), 6);
  //   - Test nested fields: allowedQueries.
  EXPECT_EQ(
      safrn::FunctionType::MOMENT, target.allowedQueries[0]->type);
  const safrn::MomentFunction & f =
      *((safrn::MomentFunction *)target.allowedQueries[0].get());
  EXPECT_EQ(safrn::MomentType::Enum_t::MEAN, f.momentType.value);
  EXPECT_EQ(0, f.col.vertical);
  EXPECT_EQ(1, f.col.column);
  EXPECT_EQ(true, f.revealCount);
  //   - Test nested fields: lexicon.
  const vector<safrn::Vertical> & verticals = target.lexicon;
  const safrn::Vertical & vert0 = verticals[0];
  const safrn::Vertical & vert1 = verticals[1];
  EXPECT_EQ(vert0.verticalIndex, 0);
  EXPECT_EQ(vert1.verticalIndex, 1);
  EXPECT_EQ(vert0.columns.size(), 6);
  EXPECT_EQ(vert1.columns.size(), 3);
  //   - Test nested fields: columns of first vertical.
  //       - Vertical: 0, Column: 0
  const safrn::ColumnBase & col00 = *(vert0.columns[0]);
  EXPECT_EQ(col00.type, safrn::ColumnDatatype::INTEGER);
  EXPECT_EQ(col00.index, 0);
  EXPECT_EQ(col00.name, "social");
  const safrn::IntegerColumn & typecol00 =
      *((safrn::IntegerColumn *)vert0.columns[0].get());
  EXPECT_EQ(typecol00.isSigned, false);
  EXPECT_EQ(typecol00.bits, 30);
  //       - Vertical: 0, Column: 1
  const safrn::ColumnBase & col01 = *(vert0.columns[1]);
  EXPECT_EQ(col01.type, safrn::ColumnDatatype::INTEGER);
  EXPECT_EQ(col01.index, 1);
  EXPECT_EQ(col01.name, "age");
  const safrn::IntegerColumn & typecol01 =
      *((safrn::IntegerColumn *)vert0.columns[1].get());
  EXPECT_EQ(typecol01.isSigned, false);
  EXPECT_EQ(typecol01.bits, 7);
  //       - Vertical: 0, Column: 2
  const safrn::ColumnBase & col02 = *(vert0.columns[2]);
  EXPECT_EQ(col02.type, safrn::ColumnDatatype::REAL);
  EXPECT_EQ(col02.index, 2);
  EXPECT_EQ(col02.name, "gpa");
  const safrn::RealColumn & typecol02 =
      *((safrn::RealColumn *)vert0.columns[2].get());
  EXPECT_EQ(typecol02.precision, 2);
  EXPECT_EQ(typecol02.scale, 1);
  //       - Vertical: 0, Column: 3
  const safrn::ColumnBase & col03 = *(vert0.columns[3]);
  EXPECT_EQ(col03.type, safrn::ColumnDatatype::BOOL);
  EXPECT_EQ(col03.index, 3);
  EXPECT_EQ(col03.name, "graduated");
  //       - Vertical: 0, Column: 4
  const safrn::ColumnBase & col04 = *(vert0.columns[4]);
  EXPECT_EQ(col04.type, safrn::ColumnDatatype::CATEGORICAL);
  EXPECT_EQ(col04.index, 4);
  EXPECT_EQ(col04.name, "math");
  const safrn::CategoricalColumn & typecol04 =
      *((safrn::CategoricalColumn *)vert0.columns[4].get());
  EXPECT_EQ(typecol04.categoricalName, "major");
  //       - Vertical: 0, Column: 5
  const safrn::ColumnBase & col05 = *(vert0.columns[5]);
  EXPECT_EQ(col05.type, safrn::ColumnDatatype::CATEGORICAL);
  EXPECT_EQ(col05.index, 5);
  EXPECT_EQ(col05.name, "electronics");
  const safrn::CategoricalColumn & typecol05 =
      *((safrn::CategoricalColumn *)vert0.columns[5].get());
  EXPECT_EQ(typecol05.categoricalName, "major");
  //   - Test nested fields: columns of second vertical.
  //       - Vertical: 1, Column: 0
  const safrn::ColumnBase & col10 = *(vert1.columns[0]);
  EXPECT_EQ(col10.type, safrn::ColumnDatatype::INTEGER);
  EXPECT_EQ(col10.index, 0);
  EXPECT_EQ(col10.name, "ssn");
  const safrn::IntegerColumn & typecol10 =
      *((safrn::IntegerColumn *)vert1.columns[0].get());
  EXPECT_EQ(typecol10.isSigned, false);
  EXPECT_EQ(typecol10.bits, 30);
  //       - Vertical: 1, Column: 1
  const safrn::ColumnBase & col11 = *(vert1.columns[1]);
  EXPECT_EQ(col11.type, safrn::ColumnDatatype::REAL);
  EXPECT_EQ(col11.index, 1);
  EXPECT_EQ(col11.name, "income");
  const safrn::RealColumn & typecol11 =
      *((safrn::RealColumn *)vert1.columns[1].get());
  EXPECT_EQ(typecol11.precision, 12);
  EXPECT_EQ(typecol11.scale, 2);
  //       - Vertical: 1, Column: 2
  const safrn::ColumnBase & col12 = *(vert1.columns[2]);
  EXPECT_EQ(col12.type, safrn::ColumnDatatype::REAL);
  EXPECT_EQ(col12.index, 2);
  EXPECT_EQ(col12.name, "debt");
  const safrn::RealColumn & typecol12 =
      *((safrn::RealColumn *)vert1.columns[2].get());
  EXPECT_EQ(typecol12.precision, 12);
  EXPECT_EQ(typecol12.scale, 2);

  //   - Test nested fields: peers.
  const map<safrn::dbuid_t, safrn::Peer> & peers = target.peers;
  size_t peer_index = 0;
  for (const pair<safrn::dbuid_t, safrn::Peer> & peer_i : peers) {
    safrn::dbuid_t tmp;
    if (peer_index == 0) {
      safrn::strToDbuid("fffffffffffffffffffffffffffffff0", tmp);
      EXPECT_EQ(peer_i.first, tmp);
      EXPECT_EQ(peer_i.second.organizationId, tmp);
      EXPECT_EQ(peer_i.second.organizationName, "analyst joe");
      EXPECT_EQ(peer_i.second.domainOrIp, "localhost");
      EXPECT_EQ(peer_i.second.port, 8000);
      EXPECT_EQ(
          EVP_MD_type(peer_i.second.certificateFingerprintAlgorithm),
          NID_sha256);
      EXPECT_EQ(
          peer_i.second.certificateFingerprint,
          StringToCharVector("16051b80f8775d8360d735130bed5ee367e83ead2"
                             "5890023d03556250a9f022f"));
      EXPECT_EQ(peer_i.second.role, safrn::ROLE_ANALYST);
      EXPECT_EQ(peer_i.second.analyst.allowedQueryCount, 10);
    } else if (peer_index == 1) {
      safrn::strToDbuid("fffffffffffffffffffffffffffffff1", tmp);
      EXPECT_EQ(peer_i.first, tmp);
      EXPECT_EQ(peer_i.second.organizationId, tmp);
      EXPECT_EQ(peer_i.second.organizationName, "irs");
      EXPECT_EQ(peer_i.second.domainOrIp, "localhost");
      EXPECT_EQ(peer_i.second.port, 8001);
      EXPECT_EQ(
          EVP_MD_type(peer_i.second.certificateFingerprintAlgorithm),
          NID_sha256);
      EXPECT_EQ(
          peer_i.second.certificateFingerprint,
          StringToCharVector("0e95afb3d05f44775619dbaeab4e6fc73eb38f8f4"
                             "99e6a5fe3e3a6676c443afb"));
      EXPECT_EQ(peer_i.second.role, safrn::ROLE_DATAOWNER);
      EXPECT_EQ(peer_i.second.dataowner.verticalIdx, 1);
    } else if (peer_index == 2) {
      safrn::strToDbuid("fffffffffffffffffffffffffffffff2", tmp);
      EXPECT_EQ(peer_i.first, tmp);
      EXPECT_EQ(peer_i.second.organizationId, tmp);
      EXPECT_EQ(peer_i.second.organizationName, "duke");
      EXPECT_EQ(peer_i.second.domainOrIp, "localhost");
      EXPECT_EQ(peer_i.second.port, 8002);
      EXPECT_EQ(
          EVP_MD_type(peer_i.second.certificateFingerprintAlgorithm),
          NID_sha256);
      EXPECT_EQ(
          peer_i.second.certificateFingerprint,
          StringToCharVector("c6a4947e72651b7b2d5804320aca060a2fc3df272"
                             "09b79cf9386c2b7b943859a"));
      EXPECT_EQ(peer_i.second.role, safrn::ROLE_DATAOWNER);
      EXPECT_EQ(peer_i.second.dataowner.verticalIdx, 0);
    } else if (peer_index == 3) {
      safrn::strToDbuid("fffffffffffffffffffffffffffffff3", tmp);
      EXPECT_EQ(peer_i.first, tmp);
      EXPECT_EQ(peer_i.second.organizationId, tmp);
      EXPECT_EQ(peer_i.second.organizationName, "ucla");
      EXPECT_EQ(peer_i.second.domainOrIp, "localhost");
      EXPECT_EQ(peer_i.second.port, 8003);
      EXPECT_EQ(
          EVP_MD_type(peer_i.second.certificateFingerprintAlgorithm),
          NID_sha256);
      EXPECT_EQ(
          peer_i.second.certificateFingerprint,
          StringToCharVector("5820237714f10cbc2334a65adba33fc29f379158d"
                             "34bc3cc5639e7a789f225eb"));
      EXPECT_EQ(peer_i.second.role, safrn::ROLE_DATAOWNER);
      EXPECT_EQ(peer_i.second.dataowner.verticalIdx, 0);
    } else if (peer_index == 4) {
      safrn::strToDbuid("fffffffffffffffffffffffffffffff4", tmp);
      EXPECT_EQ(peer_i.first, tmp);
      EXPECT_EQ(peer_i.second.organizationId, tmp);
      EXPECT_EQ(peer_i.second.organizationName, "analyst jane");
      EXPECT_EQ(peer_i.second.domainOrIp, "localhost");
      EXPECT_EQ(peer_i.second.port, 8004);
      EXPECT_EQ(
          EVP_MD_type(peer_i.second.certificateFingerprintAlgorithm),
          NID_sha256);
      EXPECT_EQ(
          peer_i.second.certificateFingerprint,
          StringToCharVector("1234567714f10cbc2334a65adba33fc29f379158d"
                             "34bc3cc5639e7a789f225eb"));
      EXPECT_EQ(peer_i.second.role, safrn::ROLE_RECIPIENT);
    } else if (peer_index == 5) {
      safrn::strToDbuid("fffffffffffffffffffffffffffffff5", tmp);
      EXPECT_EQ(peer_i.first, tmp);
      EXPECT_EQ(peer_i.second.organizationId, tmp);
      EXPECT_EQ(peer_i.second.organizationName, "random oracle");
      EXPECT_EQ(peer_i.second.domainOrIp, "localhost");
      EXPECT_EQ(peer_i.second.port, 8005);
      EXPECT_EQ(
          EVP_MD_type(peer_i.second.certificateFingerprintAlgorithm),
          NID_sha256);
      EXPECT_EQ(
          peer_i.second.certificateFingerprint,
          StringToCharVector("0987657714f10cbc2334a65adba33fc29f379158d"
                             "34bc3cc5639e7a789f225eb"));
      EXPECT_EQ(peer_i.second.role, safrn::ROLE_DEALER);
    }
    ++peer_index;
  }
}
