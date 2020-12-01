/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 *
 * DESCRIPTION: Class for describing parameters
 * needed to describe a linear regression function.
 */

#ifndef INC_98E550668188499FA5AE1603BD84A195
#define INC_98E550668188499FA5AE1603BD84A195

/* platform-specific includes */

/* c/c++ standard includes */
#include <exception>
#include <vector>

/* third-party library includes */
#include <nlohmann/json.hpp>

/* project-specific includes */
#include <JSON/Config/StudyConfig.h>
#include <JSON/Query/ColumnSpec.h>
#include <JSON/Query/QueryTypes.h>
#include <JSON/Query/SafrnFunction.h>

namespace safrn {

class LinearRegressionFunction : public SafrnFunction {
public:
  explicit LinearRegressionFunction(const nlohmann::json & json);
  LinearRegressionFunction(
      const safrn::StudyConfig & study, const nlohmann::json & json);

  const ColumnSpec dep_var;
  const std::vector<ColumnSpec> indep_vars;
  const bool fit_intercept;
  size_t num_bytes_in_f_t_table_cells;
  size_t max_f_t_table_rows;
  size_t bits_of_precision;

  class NotEnoughIndepVars : std::exception {
    const char * what() const noexcept override {
      return "Not enough independent variables.";
    }
  };

private:
  static std::vector<ColumnSpec>
  IndepVarsFromJSON(const nlohmann::json & json);
  static std::vector<ColumnSpec> IndepVarsFromJSON(
      const safrn::StudyConfig & study, const nlohmann::json & json);
};

} // namespace safrn

#endif
