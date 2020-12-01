/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

#include <iostream>
#include <nlohmann/json.hpp>

#include <QueryTester.h>

/** Logging config */
#include <ff/logging.h>

namespace safrn {

static inline std::string fileFix(char const * f) {
  return std::string("../../../../../server/src/test/data/").append(f);
}

static inline std::string fileFix(std::string const & f) {
  return fileFix(f.c_str());
}

TestStudySetup const TEST_7_PARTY(
    {{Identity("000000000000000000000000000A11CE", ROLE_DATAOWNER, 0),
      Identity("00000000000000000000000000000B0B", ROLE_DATAOWNER, 0),
      Identity("00000000000000000000000000CA111E", ROLE_DATAOWNER, 0),
      Identity("000000000000000000000000000DA51D", ROLE_DATAOWNER, 0),
      Identity("00000000000000000000000BAA000B0B", ROLE_DATAOWNER, 1),
      Identity("00000000000000000000000BAACA111E", ROLE_DATAOWNER, 1),
      Identity("00000000000000000000000BAA0DA51D", ROLE_DATAOWNER, 1),
      Identity(
          "0000000000000000000000000000DEA1", ROLE_DEALER, SIZE_MAX),
      Identity(
          "00000000000000000000000000FFFFFF",
          ROLE_RECIPIENT,
          SIZE_MAX)}},
    {{
        fileFix("vertical_0_party_0.csv"),
        fileFix("vertical_0_party_1.csv"),
        fileFix("vertical_0_party_2.csv"),
        fileFix("vertical_0_party_3.csv"),
        fileFix("vertical_1_party_0.csv"),
        fileFix("vertical_1_party_1.csv"),
        fileFix("vertical_1_party_2.csv"),
        std::string(""),
        std::string(""),
    }},
    fileFix("study7.json"));

/*{{
      for(size_t i = 0 ; i < vert_0_parties;i++)
      {
        std::string file_name = "vertical_0_party_";
        file_name += std::to_string(i);
        file_name += ".csv";
        fileFix(file_name);
      }
      for(size_t i = 0 ; i < vert_1_parties;i++)
      {
        std::string file_name = "vertical_1_party_";
        file_name += std::to_string(i);
        file_name += ".csv";
        fileFix(file_name);
      }
        std::string(""),
        std::string(""),
    }},*/

TestStudySetup const TEST_4_PARTY(
    {{Identity("000000000000000000000000000A11CE", ROLE_DATAOWNER, 0),
      Identity("00000000000000000000000000000B0B", ROLE_DATAOWNER, 1),
      Identity("00000000000000000000000000CA111E", ROLE_DATAOWNER, 0),
      Identity("000000000000000000000000000DA51D", ROLE_DATAOWNER, 1),
      Identity(
          "0000000000000000000000000000DEA1", ROLE_DEALER, SIZE_MAX),
      Identity(
          "00000000000000000000000000FFFFFF",
          ROLE_RECIPIENT,
          SIZE_MAX)}},
    {{
        fileFix("alice4.csv"),
        fileFix("bob4.csv"),
        fileFix("callie4.csv"),
        fileFix("david4.csv"),
        std::string(""),
        std::string(""),
    }},
    fileFix("study4.json"));

TestStudySetup const TEST_2_PARTY(
    {{Identity("000000000000000000000000000A11CE", ROLE_DATAOWNER, 0),
      Identity("00000000000000000000000000000B0B", ROLE_DATAOWNER, 1),
      Identity(
          "0000000000000000000000000000DEA1", ROLE_DEALER, SIZE_MAX),
      Identity(
          "00000000000000000000000000FFFFFF",
          ROLE_RECIPIENT,
          SIZE_MAX)}},
    {{
        fileFix("alice2.csv"),
        fileFix("bob2.csv"),
        std::string(""),
        std::string(""),
    }},
    fileFix("study2.json"));

bool testQuery(
    std::string const & query_file,
    std::vector<double> & results,
    TestStudySetup const & setup) {
  std::ifstream study_stream(setup.studyFile.c_str());
  if (!study_stream.is_open()) {
    log_error(
        "Could not open study config file %s.",
        setup.studyFile.c_str());
    return false;
  }

  nlohmann::json study_json = nlohmann::json::parse(study_stream);
  const StudyConfig scfg = readStudyFromJson(study_json);

  std::ifstream query_stream(fileFix(query_file));
  if (!query_stream.is_open()) {
    log_error("could not open query file.");
    return false;
  }

  nlohmann::json query_json = nlohmann::json::parse(query_stream);
  const Query query(scfg, query_json);

  std::map<Identity, std::unique_ptr<Fronctocol>> tests;

  std::string const lookupTableDirectory =
      std::string("../../../../../server/src/test/data/");

  std::vector<PeerSet> peersets;
  peersets.reserve(setup.participants.size());
  for (size_t i = 0; i < setup.participants.size(); i++) {
    peersets.emplace_back();
    tests[setup.participants[i]] = startup(
        setup.dataFiles[i],
        lookupTableDirectory,
        query,
        scfg,
        setup.participants[i],
        peersets.back());
  }

  return runTests(tests);
}

} // namespace safrn
