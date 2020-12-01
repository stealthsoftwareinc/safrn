/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

#ifndef SAFRN_TEST_QUERY_TESTER_H_
#define SAFRN_TEST_QUERY_TESTER_H_

#include <string>
#include <vector>

#include <framework/Framework.h>
#include <framework/TestRunner.h>

#include <Identity.h>
#include <PeerSet.h>
#include <Startup.h>

#include <JSON/Config/StudyConfig.h>
#include <JSON/Query/Query.h>

namespace safrn {

struct TestStudySetup {
  std::vector<Identity> const participants;
  std::vector<std::string> const dataFiles;

  std::string const studyFile;

  TestStudySetup(
      std::vector<Identity> && ps,
      std::vector<std::string> && dfs,
      std::string sf) :
      participants(std::move(ps)),
      dataFiles(std::move(dfs)),
      studyFile(sf) {
  }
};

extern TestStudySetup const TEST_7_PARTY;
extern TestStudySetup const TEST_4_PARTY;
extern TestStudySetup const TEST_2_PARTY;

bool testQuery(
    std::string const & query_file,
    std::vector<double> & results,
    TestStudySetup const & setup);

} // namespace safrn

#endif //SAFRN_TEST_QUERY_TESTER_H_
