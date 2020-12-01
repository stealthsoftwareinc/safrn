/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* platform-specific includes */

/* c/c++ standard includes */

/* third-party library includes */

/* project-specific includes */

/* same module include */
#include <JSON/Query/JoinOn.h>

safrn::JoinOn::JoinOn(const nlohmann::json & json) :
    col(json["col"]), formula(json["formula"]) {
}

safrn::JoinOn::JoinOn(
    const safrn::StudyConfig & study, const nlohmann::json & json) :
    col(study, json["col"]), formula(json["formula"]) {
}
