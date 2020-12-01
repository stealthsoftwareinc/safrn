/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 *
 * DESCRIPTION: Class for describing the
 * combination of multiple columns and their
 * transforms that are required for a join
 * operation.
 */

#ifndef A00B4411838E4B1FB00A91A7E8DE0D8F
#define A00B4411838E4B1FB00A91A7E8DE0D8F

/* platform-specific includes */

/* c/c++ standard includes */
#include <exception>

/* third-party library includes */
#include <nlohmann/json.hpp>

/* project-specific includes */
#include <JSON/Config/StudyConfig.h>
#include <JSON/Query/ColumnSpec.h>
#include <JSON/Query/JoinOn.h>
#include <JSON/Query/JoinType.h>
#include <JSON/Query/Polynomial.h>
#include <JSON/Query/QueryTypes.h>

namespace safrn {

struct JoinStatement {
public:
  explicit JoinStatement(const nlohmann::json & json);
  JoinStatement(
      const safrn::StudyConfig & study, const nlohmann::json & json);

  const JoinType type;
  const std::vector<std::pair<JoinOn, JoinOn>> joinOns;

  class NotEnoughJoinOns : std::exception {
    const char * what() const noexcept override {
      return "Not enough joinOns.";
    }
  };

private:
  static std::vector<std::pair<JoinOn, JoinOn>>
  jsonToJoinOns(const nlohmann::json & arrayJson);

  static std::vector<std::pair<JoinOn, JoinOn>> jsonToJoinOns(
      const safrn::StudyConfig & study,
      const nlohmann::json & arrayJson);
};

} // namespace safrn

#endif
