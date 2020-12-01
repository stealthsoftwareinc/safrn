/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 *
 * DESCRIPTION: This module describes intrinsic types
 * used by the SAFRN query code.
 */

#ifndef INC_626C08DCB6804DD287E3AA031F7A82EC
#define INC_626C08DCB6804DD287E3AA031F7A82EC

/* platform-specific includes */

/* c/c++ standard includes */
#include <memory>
#include <set>
#include <tuple>
#include <vector>

/* third-party library includes */
#include <nlohmann/json.hpp>

/* project-specific includes */

namespace safrn {

// The VerticalId is w.r.t. the virtual schema defined in study.config.
// TODO: Update the type of this to match whatever type we're using for this.
typedef size_t VerticalIndex_t;

// Ditto, for column (w.r.t. the column's index within the relevant lexicon).
typedef size_t ColumnIndex_t;

// This type is used for comparison of output of a column calculation
// with a literal constant value represented in the query.
typedef float ComparisonValue_t;

typedef int32_t Exponent_t;

} // namespace safrn

#endif
