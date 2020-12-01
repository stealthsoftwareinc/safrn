/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* platform-specific includes */

/* c/c++ standard includes */

/* third-party library includes */

/* project-specific includes */
#include <JSON/Query/SafrnFunction.h>

/* same module include */
#include <JSON/Config/StudyConfig.h>
#include <JSON/Query/LinearRegressionFunction.h>

safrn::LinearRegressionFunction::LinearRegressionFunction(
    const nlohmann::json & json) :
    SafrnFunction(FunctionType::LIN_REGRESSION),
    fit_intercept(json["fit_intercept"]),
    num_bytes_in_f_t_table_cells(json["table_cell_bytes"]),
    max_f_t_table_rows(json["num_table_rows"]),
    bits_of_precision(json["bits_of_precision"]),
    dep_var(json["dep_var"]),
    indep_vars(IndepVarsFromJSON(json["indep_vars"])) {
}

safrn::LinearRegressionFunction::LinearRegressionFunction(
    const safrn::StudyConfig & study, const nlohmann::json & json) :
    SafrnFunction(FunctionType::LIN_REGRESSION),
    fit_intercept(json["fit_intercept"]),
    num_bytes_in_f_t_table_cells(json["table_cell_bytes"]),
    max_f_t_table_rows(json["num_table_rows"]),
    bits_of_precision(json["bits_of_precision"]),
    dep_var(study, json["dep_var"]),
    indep_vars(IndepVarsFromJSON(study, json["indep_vars"])) {
}

std::vector<safrn::ColumnSpec>
safrn::LinearRegressionFunction::IndepVarsFromJSON(
    const nlohmann::json & json) {

  if (json.empty()) {
    throw NotEnoughIndepVars();
  }

  std::vector<ColumnSpec> result;
  for (const auto & jsonIndep : json) {
    result.emplace_back(ColumnSpec(jsonIndep));
  }
  return result;
}

std::vector<safrn::ColumnSpec>
safrn::LinearRegressionFunction::IndepVarsFromJSON(
    const safrn::StudyConfig & study, const nlohmann::json & json) {

  if (json.empty()) {
    throw NotEnoughIndepVars();
  }

  std::vector<ColumnSpec> result;
  for (const auto & jsonIndep : json) {
    result.emplace_back(ColumnSpec(study, jsonIndep));
  }
  return result;
}
