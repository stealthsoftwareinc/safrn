/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

#ifndef SAFRN_SERVER_STARTUP_H_
#define SAFRN_SERVER_STARTUP_H_

#include <memory>
#include <string>

#include <JSON/Config/StudyConfig.h>
#include <JSON/Query/Query.h>

#include <Identity.h>
#include <PeerSet.h>

#include <dataowner/fortissimo.h>

namespace safrn {

/**
 * Prepares a fronctocol to run based on a query and a study config.
 *
 * @param the CSV file to parse (empty if non-dataowner)
 * @param the Query object
 * @param the StudyConfig object
 * @param the Identity of this party
 * @param (return by reference) the peers participating in the query
 * @return a fronctocol to run (nullptr if not a participant, or invalid query)
 */
std::unique_ptr<Fronctocol> startup(
    std::string const & csvFile,
    std::string const & lookupTableDirectory,
    Query const & q,
    StudyConfig const & scfg,
    Identity const & id,
    PeerSet & peers);

} // namespace safrn

#endif //SAFRN_SERVER_STARTUP_H_
