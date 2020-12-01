/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* c/c++ standard includes */
#include <algorithm>
#include <limits>
#include <map>

/* third-party library includes */

/* project-specific includes */
#include <JSON/Columns/ColumnBase.h>

/* same module include */
#include <JSON/Columns/IntegerColumn.h>

safrn::IntegerColumn::IntegerColumn(const nlohmann::json & json) :
    ColumnBase(json), isSigned(json["signed"]), bits(json["bits"]) {
}

std::unique_ptr<safrn::ColumnBase>
safrn::IntegerColumn::CreateFromJSON(const nlohmann::json & JSON) {
  return std::unique_ptr<ColumnBase>(new IntegerColumn(JSON));
}
