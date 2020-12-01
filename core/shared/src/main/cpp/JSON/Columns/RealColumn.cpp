/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* c/c++ standard includes */

/* third-party library includes */

/* project-specific includes */

/* same module include */
#include <JSON/Columns/RealColumn.h>

safrn::RealColumn::RealColumn(const nlohmann::json & json) :
    ColumnBase(json),
    precision(json["precision"]),
    scale(json["scale"]) {
}

std::unique_ptr<safrn::ColumnBase>
safrn::RealColumn::CreateFromJSON(const nlohmann::json & JSON) {
  return std::unique_ptr<ColumnBase>(new RealColumn(JSON));
}
