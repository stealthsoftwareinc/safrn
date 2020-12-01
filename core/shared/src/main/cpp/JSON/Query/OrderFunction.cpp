/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* platform-specific includes */

/* c/c++ standard includes */

/* third-party library includes */

/* project-specific includes */

/* same module include */
#include <JSON/Config/StudyConfig.h>
#include <JSON/Query/OrderFunction.h>

safrn::OrderFunction::OrderFunction(const nlohmann::json & json) :
    SafrnFunction(FunctionType::ORDER),
    col(json["col"]),
    is_percentile(json["is_percentile"]),
    lowest_first(json["lowest_first"]),
    value(json["value"]) {
}

safrn::OrderFunction::OrderFunction(
    const safrn::StudyConfig & study, const nlohmann::json & json) :
    SafrnFunction(FunctionType::ORDER),
    col(study, json["col"]),
    is_percentile(json["is_percentile"]),
    lowest_first(json["lowest_first"]),
    value(json["value"]) {
}
