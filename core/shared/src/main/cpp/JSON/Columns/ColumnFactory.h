/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 *
 * DESCRIPTION: Class which determines which column
 * is associated with a json specification, and
 * creations it as needed.
 */

#ifndef INC_622D0F26B6A441F99F00EB7E08098C64
#define INC_622D0F26B6A441F99F00EB7E08098C64

/* c/c++ standard includes */

/* third-party library includes */
#include <nlohmann/json.hpp>

/* project-specific includes */
#include <JSON/Columns/ColumnBase.h>

namespace safrn {

class ColumnFactory {
public:
  static std::unique_ptr<ColumnBase>
  createColumn(nlohmann::json const & columnJson);
};

} // namespace safrn

#endif
