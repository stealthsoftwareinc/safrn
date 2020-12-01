/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* c/c++ standard includes */

/* third-party library includes */

/* project-specific includes */
#include <Util/Utils.h>

/* same module include */
#include <JSON/Columns/CategoricalColumn.h>

safrn::CategoricalColumn::CategoricalColumn(
    const nlohmann::json & json) :
    ColumnBase(json),
    categoricalName(json_string(json["categorical"])) {
}

std::unique_ptr<safrn::ColumnBase>
safrn::CategoricalColumn::CreateFromJSON(const nlohmann::json & JSON) {
  return std::unique_ptr<ColumnBase>(new CategoricalColumn(JSON));
}
