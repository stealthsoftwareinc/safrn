/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* platform-specific includes */

/* c/c++ standard includes */

/* third-party library includes */

/* project-specific includes */

/* same module include */
#include <JSON/Query/FilterTerm.h>

safrn::FilterTerm::FilterTerm(const nlohmann::json & json) :
    left(json["left"]),
    right(json["right"]),
    comp(json_string(json["comp"])) {
}

safrn::FilterTerm::FilterTerm(
    const safrn::StudyConfig & study, const nlohmann::json & json) :
    left(study, json["left"]),
    right(json["right"]),
    comp(json_string(json["comp"])) {
}
