/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 *
 * DESCRIPTION: Class that captures fields needed
 * to describe an Internal SAFRN Query.
 */

#ifndef B781D53106C74582916CA0293239855D
#define B781D53106C74582916CA0293239855D

/* platform-specific includes */

/* c/c++ standard includes */
#include <exception>
#include <fstream>
#include <memory>
#include <set>
#include <string>
#include <tuple>
#include <vector>

/* third-party library includes */
#include <nlohmann/json.hpp>

/* project-specific includes */
#include <JSON/Config/StudyConfig.h>
#include <JSON/Query/JoinStatement.h>
#include <JSON/Query/QueryFilter.h>
#include <JSON/Query/QueryTypes.h>
#include <JSON/Query/SafrnFunction.h>

namespace safrn {

class Query {
public:
  explicit Query(const nlohmann::json & json);
  Query(const safrn::StudyConfig & study, const nlohmann::json & json);

  // DEPRECATED. Currently, prefilters are not supported (ignored).
  const std::vector<QueryFilter> prefilters;

  // A query allows arbitrary JOINs between any of the verticals, subject to constraints:
  //   - Each vertical in the virtual schema may appear at most once
  //   - JOINs will be processed from ``left-to-right'', e.g. mathematically:
  //        (((A JOIN B) JOIN C) JOIN D) JOIN E
  // The following encodes the desired JOIN semantics. The vector will have
  // length equal to the number of verticals used in the query.
  // The JoinType will refer to the JOIN type to be applied to the next/right
  // vertical; so there should always be exactly one (the last) element in
  // join_statements_ whose JoinType is SELF.

  // TODO: Add a tree structure to support the following example: (A JOIN B) JOIN (C JOIN D)
  // when we need to implement more complex or compound joins,
  // this structure will need to be updated accordingly.
  const std::unique_ptr<JoinStatement> joinStatement;

  // The computation to perform.
  const std::unique_ptr<SafrnFunction> function;

  // TODO: Add postFilters

  class TooManyPrefilters : std::exception {
    const char * what() const noexcept override {
      return "Incorrect number of prefilters.";
    }
  };

private:
  static std::vector<QueryFilter>
  createPrefilters(const nlohmann::json & json);

  static std::vector<QueryFilter> createPrefilters(
      const safrn::StudyConfig & study, const nlohmann::json & json);

  static std::unique_ptr<JoinStatement>
  createJoinStatementIfExists(const nlohmann::json & topJson);

  static std::unique_ptr<JoinStatement> createJoinStatementIfExists(
      const safrn::StudyConfig & study, const nlohmann::json & topJson);
};

} // namespace safrn

#endif
