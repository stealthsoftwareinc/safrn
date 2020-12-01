/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

#ifndef SAFRN_SERVER_STARTUP_MOMENTS_H_
#define SAFRN_SERVER_STARTUP_MOMENTS_H_

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

#include <dataowner/Moments.h>
#include <dataowner/MomentsInfo.h>
#include <dataowner/fortissimo.h>
#include <dealer/MomentsHouse.h>

namespace safrn {

extern bool findPayloadMoment(
    MomentFunction const & func,
    std::vector<size_t> & leftPayloads,
    std::vector<size_t> & rightPayloads,
    size_t leftVert,
    size_t rightVert,
    size_t * data_vert,
    StudyConfig const & scfg);

dataowner::MomentsInfo const * setupMomentsInfo(
    safrn::dataowner::GlobalInfo const * const global_info,
    std::vector<size_t> const & left_payloads,
    std::vector<size_t> const & right_payloads,
    const size_t dataVertical,
    const size_t leftVertical,
    const bool include_count,
    const size_t moment,
    PeerSet const & peers,
    Identity const & id);

std::unique_ptr<Fronctocol> setupMoments(
    safrn::dataowner::GlobalInfo const * const global_info,
    std::vector<size_t> const & keys,
    std::vector<size_t> const & left_payloads,
    std::vector<size_t> const & right_payloads,
    const size_t dataVertical,
    const size_t leftVertical,
    const bool include_count,
    const size_t moment,
    Identity const & id,
    StudyConfig const & scfg,
    PeerSet const & peers,
    std::string const & csvFile);

} // namespace safrn

#endif //SAFRN_SERVER_STARTUP_MOMENTS_H_
