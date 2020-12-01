/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 *
 * DESCRIPTION: Class that describes the moment
 * function needed for a probability distribution
 * for a given set of column data.
 */

#ifndef INC_8A46C650BDAD43AB92E349FDDD5F6726
#define INC_8A46C650BDAD43AB92E349FDDD5F6726

/* platform-specific includes */

/* c/c++ standard includes */
#include <memory>

/* third-party library includes */
#include <nlohmann/json.hpp>

/* project-specific includes */
#include <JSON/Config/StudyConfig.h>
#include <JSON/Query/ColumnSpec.h>
#include <JSON/Query/MomentType.h>
#include <JSON/Query/QueryTypes.h>
#include <JSON/Query/SafrnFunction.h>

namespace safrn {

class MomentFunction : public SafrnFunction {
public:
  explicit MomentFunction(const nlohmann::json & json);
  MomentFunction(
      const safrn::StudyConfig & study, const nlohmann::json & json);

  const ColumnSpec col;
  const MomentType momentType;
  bool revealCount;
  size_t bits_of_precision;
};

} // namespace safrn

#endif
