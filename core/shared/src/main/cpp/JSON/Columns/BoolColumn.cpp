/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* platform-specific includes */

/* c/c++ standard includes */
#include <memory>

/* third-party library includes */
#include <nlohmann/json.hpp>

/* project-specific includes */
#include <JSON/Columns/ColumnBase.h>

/* same module include */
#include <JSON/Columns/BoolColumn.h>

safrn::BoolColumn::BoolColumn(const nlohmann::json & json) :
    IntegerColumn(addBoolJsonFields(json)) {
}

std::unique_ptr<safrn::ColumnBase>
safrn::BoolColumn::CreateFromJSON(const nlohmann::json & JSON) {
  return std::unique_ptr<ColumnBase>(new BoolColumn(JSON));
}

nlohmann::json
safrn::BoolColumn::addBoolJsonFields(const nlohmann::json & json) {
  nlohmann::json result = json;
  result["signed"] = false;
  result["bits"] = 1;
  return result;
}
