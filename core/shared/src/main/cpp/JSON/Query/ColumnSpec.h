/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 *
 * DESCRIPTION: Class used to precisely describe
 * a column selection within multiple verticals.
 */

#ifndef INC_379139EA8F8E4DC2AFA83B53761B31C8
#define INC_379139EA8F8E4DC2AFA83B53761B31C8

/* platform-specific includes */

/* c/c++ standard includes */
#include <string>

/* third-party library includes */
#include <nlohmann/json.hpp>

/* project-specific includes */
#include <JSON/Query/QueryTypes.h>

namespace safrn {

// Forward declare StudyConfig.
class StudyConfig;

class ColumnSpec {
public:
  explicit ColumnSpec(const nlohmann::json & json);
  ColumnSpec(
      const safrn::StudyConfig & study, const nlohmann::json & json);

  const VerticalIndex_t vertical;
  const ColumnIndex_t column;
};

ColumnIndex_t ParseColumnIndexFromJson(
    const StudyConfig & study, const nlohmann::json & json);

} // namespace safrn

#endif
