/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* c/c++ standard includes */
#include <functional>
#include <map>
#include <memory>

/* third-party library includes */

/* project-specific includes */
#include <JSON/Columns/CategoricalColumn.h>
#include <JSON/Columns/IntegerColumn.h>
#include <JSON/Columns/RealColumn.h>

/* same module include */
#include <JSON/Columns/BoolColumn.h>
#include <JSON/Columns/CategoricalColumn.h>
#include <JSON/Columns/ColumnFactory.h>
#include <JSON/Columns/IntegerColumn.h>
#include <JSON/Columns/RealColumn.h>

std::unique_ptr<safrn::ColumnBase>
safrn::ColumnFactory::createColumn(nlohmann::json const & columnJson) {
  static const std::map<
      std::string,
      std::function<std::unique_ptr<ColumnBase>(
          const nlohmann::json &)>>
      factoryMap = {{"integer", IntegerColumn::CreateFromJSON},
                    {"real", RealColumn::CreateFromJSON},
                    {"categorical", CategoricalColumn::CreateFromJSON},
                    {"bool", BoolColumn::CreateFromJSON}};

  std::function<std::unique_ptr<ColumnBase>(const nlohmann::json &)>
      function = factoryMap.at(columnJson["type"]);
  return function(columnJson);
}
