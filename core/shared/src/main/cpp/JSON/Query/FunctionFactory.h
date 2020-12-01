/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 *
 * DESCRIPTION: Selects which function class to
 * create based on the appropriate fields.
 */

#ifndef INC_515F8ADCF3D34BDD9850481CF17367D1
#define INC_515F8ADCF3D34BDD9850481CF17367D1

/* platform-specific includes */

/* c/c++ standard includes */
#include <memory>

/* third-party library includes */
#include <nlohmann/json.hpp>

/* project-specific includes */
#include <JSON/Config/StudyConfig.h>
#include <JSON/Query/SafrnFunction.h>

namespace safrn {

std::unique_ptr<SafrnFunction>
FunctionFactory(const nlohmann::json & functionJson);

std::unique_ptr<SafrnFunction> FunctionFactory(
    const safrn::StudyConfig & study,
    const nlohmann::json & functionJson);

} // namespace safrn

#endif
