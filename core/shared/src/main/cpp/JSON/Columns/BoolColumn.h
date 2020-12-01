/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 *
 * DESCRIPTION: Class which parses and stores
 * fields associated with boolean columns
 * in the database.
 */

#ifndef INC_698569A071F34380B7589DB99901C43E
#define INC_698569A071F34380B7589DB99901C43E

/* platform-specific includes */

/* c/c++ standard includes */

/* third-party library includes */
#include <nlohmann/json.hpp>

/* project-specific includes */
#include <JSON/Columns/IntegerColumn.h>

namespace safrn {

class BoolColumn : public IntegerColumn {
public:
  explicit BoolColumn(const nlohmann::json & json);

  static std::unique_ptr<ColumnBase>
  CreateFromJSON(const nlohmann::json & JSON);

private:
  static nlohmann::json addBoolJsonFields(const nlohmann::json & json);
};

} // namespace safrn

#endif
