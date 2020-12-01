/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

#ifndef SAFRN_SERVER_STARTUP_UTILS_H_
#define SAFRN_SERVER_STARTUP_UTILS_H_

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

dataowner::LargeNum convertDoubleToLargeNum(
    double val, size_t bitsOfPrecision, dataowner::LargeNum modulus);

/**
 * Reads a CSV file.
 *
 * @param file name
 * @param (return by reference) empty observation list to fill.
 * @param list of key column indices, WRT lexicon.
 * @param list of payload column indices, WRT lexicon.
 * @param index of a column which needs precision doubled (SIZE_MAX to ignore)
 * @param the study config (holds the lexicon)
 * @param the identity of this participant.
 * @param the modulus
 * @param bits of precision when converting inputs (double) to modulus field
 *
 * @return true for success, false otherwise.
 */
bool readCSV(
    std::string const & file,
    ff::mpc::ObservationList<dataowner::LargeNum> & oList,
    std::vector<size_t> const & keyCols,
    std::vector<size_t> const & payloadCols,
    size_t extra_precis_col,
    StudyConfig const & scfg,
    Identity const & id,
    dataowner::LargeNum const mod,
    size_t bitsOfPrecision);

} // namespace safrn

#endif //SAFRN_SERVER_STARTUP_UTILS_H_
