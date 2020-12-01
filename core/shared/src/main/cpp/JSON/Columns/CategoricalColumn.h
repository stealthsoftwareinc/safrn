/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 *
 * DESCRIPTION: Class which parses and stores
 * fields associated with categorical columns
 * in the database.
 */

#ifndef INC_7BCF13D603204EA29E7D4D9B04F9D1E1
#define INC_7BCF13D603204EA29E7D4D9B04F9D1E1

/* c/c++ standard includes */
#include <memory>
#include <string>

/* third-party library includes */
#include <nlohmann/json.hpp>

/* project-specific includes */
#include <JSON/Columns/ColumnBase.h>

namespace safrn {

class CategoricalColumn : public ColumnBase {
public:
  explicit CategoricalColumn(const nlohmann::json & json);

  static std::unique_ptr<ColumnBase>
  CreateFromJSON(const nlohmann::json & JSON);

  const std::string categoricalName;
};

} // namespace safrn

#endif
