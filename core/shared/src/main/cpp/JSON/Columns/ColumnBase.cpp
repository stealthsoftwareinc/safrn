/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* c/c++ standard includes */
#include <stdexcept>
#include <string>

/* third-party library includes */

/* project-specific includes */
#include <Util/Utils.h>

/* same module include */
#include <JSON/Columns/ColumnBase.h>

namespace {
safrn::ColumnDatatype
StringToColumnType(const std::string & as_string) {
  if (as_string == "bool")
    return safrn::ColumnDatatype::BOOL;
  if (as_string == "integer")
    return safrn::ColumnDatatype::INTEGER;
  if (as_string == "real")
    return safrn::ColumnDatatype::REAL;
  if (as_string == "categorical")
    return safrn::ColumnDatatype::CATEGORICAL;
  throw std::runtime_error("Unrecognized column datatype.");
}
} // namespace

safrn::ColumnBase::ColumnBase(const nlohmann::json & json) :
    type(StringToColumnType(json["type"])),
    name(json_string(json["name"])),
    index(json["columnIndex"]) {
}
