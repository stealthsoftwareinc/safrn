/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

#ifndef SAFRN_SERVER_STARTUP_REGRESSION_H_
#define SAFRN_SERVER_STARTUP_REGRESSION_H_

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include <mpc/ModUtils.h>
#include <mpc/ObservationList.h>

#include <JSON/Config/StudyConfig.h>
#include <JSON/Query/ColumnSpec.h>
#include <JSON/Query/JoinOn.h>
#include <JSON/Query/JoinStatement.h>
#include <JSON/Query/LinearRegressionFunction.h>
#include <JSON/Query/MomentFunction.h>
#include <JSON/Query/Query.h>

#include <Identity.h>
#include <PeerSet.h>

#include <dataowner/Regression.h>
#include <dataowner/RegressionInfo.h>
#include <dataowner/fortissimo.h>
#include <dealer/RegressionHouse.h>

namespace safrn {

extern bool findPayloadRegression(
    LinearRegressionFunction const & func,
    std::vector<size_t> & leftPayloads,
    std::vector<size_t> & rightPayloads,
    size_t leftVert,
    size_t rightVert,
    size_t * depVertical,
    StudyConfig const & scfg);

extern dataowner::RegressionInfo const * setupRegressionInfo(
    safrn::dataowner::GlobalInfo const * const global_info_pointer,
    std::vector<size_t> const & left_payloads,
    std::vector<size_t> const & right_payloads,
    size_t dependentVertical,
    size_t leftVertical,
    const bool fit_intercept,
    PeerSet const & peers,
    Identity const & id);

extern std::unique_ptr<Fronctocol> setupRegression(
    safrn::dataowner::GlobalInfo const * const global_info_pointer,
    std::vector<size_t> const & keys,
    std::vector<size_t> const & left_payloads,
    std::vector<size_t> const & right_payloads,
    size_t dependentVertical,
    size_t leftVertical,
    const bool fit_intercept,
    Identity const & id,
    StudyConfig const & scfg,
    PeerSet const & peers,
    std::string const & csvFile,
    std::string const & F_table_file,
    std::string const & t_table_file);

} // namespace safrn

#endif //SAFRN_SERVER_STARTUP_REGRESSION_H_
