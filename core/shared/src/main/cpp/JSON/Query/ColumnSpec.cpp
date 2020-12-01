/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* platform-specific includes */

/* c/c++ standard includes */
#include <stdexcept>
#include <string>

/* third-party library includes */

/* project-specific includes */

/* same module include */
#include <JSON/Config/StudyConfig.h>
#include <JSON/Query/ColumnSpec.h>

safrn::ColumnSpec::ColumnSpec(const nlohmann::json & json) :
    vertical((safrn::VerticalIndex_t)json["vertical"]),
    column((safrn::ColumnIndex_t)json["columnIndex"]) {
}

safrn::ColumnSpec::ColumnSpec(
    const safrn::StudyConfig & study, const nlohmann::json & json) :
    vertical((safrn::VerticalIndex_t)json["vertical"]),
    column(ParseColumnIndexFromJson(study, json)) {
}

safrn::ColumnIndex_t safrn::ParseColumnIndexFromJson(
    const safrn::StudyConfig & study, const nlohmann::json & json) {
  const safrn::VerticalIndex_t vertIndex =
      (json_contains(json, "vertical") ?
           (safrn::VerticalIndex_t)json["vertical"] :
           std::numeric_limits<safrn::VerticalIndex_t>::max());
  const safrn::ColumnIndex_t colIndex =
      (json_contains(json, "columnIndex") ?
           (safrn::ColumnIndex_t)json["columnIndex"] :
           std::numeric_limits<safrn::ColumnIndex_t>::max());
  const std::string colName =
      (json_contains(json, "columnName") ?
           json_string(json["columnName"]) :
           "");
  if (colName.empty()) {
    if (colIndex == std::numeric_limits<safrn::ColumnIndex_t>::max()) {
      throw std::runtime_error("Unable to determine column index.");
    }
    return colIndex;
  }
  if (vertIndex != std::numeric_limits<safrn::VerticalIndex_t>::max()) {
    if (vertIndex >= study.lexicon.size()) {
      throw std::runtime_error("Invalid vertical index.");
    }
    const safrn::Vertical & vert_i = study.lexicon[vertIndex];
    if (colIndex != std::numeric_limits<safrn::ColumnIndex_t>::max()) {
      if (colIndex >= vert_i.columns.size()) {
        throw std::runtime_error("Invalid column index.");
      }
      if (vert_i.columns[colIndex]->name != colName) {
        throw std::runtime_error(
            "Incompatible column index and column name.");
      }
      return colIndex;
    } else {
      for (size_t i = 0; i < vert_i.columns.size(); ++i) {
        if (vert_i.columns[i]->name == colName)
          return vert_i.columns[i]->index;
      }
      throw std::runtime_error(
          "Unable to parse column index from column name.");
    }
  } else {
    for (const safrn::Vertical & vert_i : study.lexicon) {
      if (colIndex !=
          std::numeric_limits<safrn::ColumnIndex_t>::max()) {
        if (colIndex >= vert_i.columns.size()) {
          throw std::runtime_error("Invalid column index.");
        }
        if (vert_i.columns[colIndex]->name != colName) {
          throw std::runtime_error(
              "Incompatible column index and column name.");
        }
        return colIndex;
      } else {
        for (size_t i = 0; i < vert_i.columns.size(); ++i) {
          if (vert_i.columns[i]->name == colName)
            return vert_i.columns[i]->index;
        }
      }
    }
  }

  // Error if code reaches here.
  throw std::runtime_error("Unable to parse column index.");
  return colIndex;
}
