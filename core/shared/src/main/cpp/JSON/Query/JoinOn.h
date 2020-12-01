/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

#ifndef INC_947BE16592A14BB8A7D3521B0BCB90DF
#define INC_947BE16592A14BB8A7D3521B0BCB90DF

/* platform-specific includes */

/* c/c++ standard includes */

/* third-party library includes */
#include <nlohmann/json.hpp>

/* project-specific includes */
#include <JSON/Config/StudyConfig.h>
#include <JSON/Query/ColumnSpec.h>
#include <JSON/Query/Polynomial.h>
#include <JSON/Query/QueryTypes.h>
#include <JSON/Query/RationalFraction.h>

namespace safrn {

// Specifies one-half of the join Key; namely, this describes the Column to
// join on (specified via a (Vertical Index, Column Index) pair, which is w.r.t.
// the Study config), and the formula to apply to that column value
// (we support only/any polynomial on the value-type for that Column).
class JoinOn {
public:
  explicit JoinOn(const nlohmann::json & json);
  JoinOn(const safrn::StudyConfig & study, const nlohmann::json & json);

  const ColumnSpec col;
  const Polynomial<RationalFraction> formula;
};

} // namespace safrn

#endif
