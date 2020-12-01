/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* platform-specific includes */

/* c/c++ standard includes */
#include <functional>
#include <map>
#include <string>

/* third-party library includes */

/* project-specific includes */
#include <JSON/Config/StudyConfig.h>
#include <JSON/Query/LinearRegressionFunction.h>
#include <JSON/Query/MomentFunction.h>
#include <JSON/Query/OrderFunction.h>
#include <JSON/Query/SafrnFunction.h>

/* same module include */
#include <JSON/Query/FunctionFactory.h>

std::unique_ptr<safrn::SafrnFunction>
safrn::FunctionFactory(const nlohmann::json & functionJson) {
  if (functionJson["type"] == "OrderFunction") {
    return std::unique_ptr<SafrnFunction>(
        new OrderFunction(functionJson));
  } else if (functionJson["type"] == "MomentFunction") {
    return std::unique_ptr<SafrnFunction>(
        new MomentFunction(functionJson));
  } else if (functionJson["type"] == "LinearRegressionFunction") {
    return std::unique_ptr<SafrnFunction>(
        new LinearRegressionFunction(functionJson));
  }

  return nullptr;
}

std::unique_ptr<safrn::SafrnFunction> safrn::FunctionFactory(
    const safrn::StudyConfig & study,
    const nlohmann::json & functionJson) {
  if (functionJson["type"] == "OrderFunction") {
    return std::unique_ptr<SafrnFunction>(
        new OrderFunction(study, functionJson));
  } else if (functionJson["type"] == "MomentFunction") {
    return std::unique_ptr<SafrnFunction>(
        new MomentFunction(study, functionJson));
  } else if (functionJson["type"] == "LinearRegressionFunction") {
    return std::unique_ptr<SafrnFunction>(
        new LinearRegressionFunction(study, functionJson));
  }

  return nullptr;
}
