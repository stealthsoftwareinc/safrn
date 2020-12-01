/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* platform-specific includes */

/* c/c++ standard includes */

/* third-party library includes */

/* project-specific includes */

/* same module include */
#include <JSON/Config/StudyConfig.h>
#include <JSON/Query/MomentFunction.h>

safrn::MomentFunction::MomentFunction(const nlohmann::json & json) :
    SafrnFunction(FunctionType::MOMENT),
    col(json["col"]),
    momentType(json["momentType"]),
    revealCount(json["revealCount"]),
    bits_of_precision(json["bits_of_precision"]) {
}

safrn::MomentFunction::MomentFunction(
    const safrn::StudyConfig & study, const nlohmann::json & json) :
    SafrnFunction(FunctionType::MOMENT),
    col(study, json["col"]),
    momentType(json["momentType"]),
    revealCount(json["revealCount"]),
    bits_of_precision(json["bits_of_precision"]) {
}
