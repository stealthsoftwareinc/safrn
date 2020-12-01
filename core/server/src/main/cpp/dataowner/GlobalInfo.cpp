/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

#include <JSON/Query/LinearRegressionFunction.h>
#include <JSON/Query/MomentFunction.h>
#include <JSON/Query/SafrnFunction.h>
#include <dataowner/GlobalInfo.h>

#include <ff/logging.h>

namespace safrn {
namespace dataowner {

static size_t kNumDataowners = 100;

GlobalInfo generateGlobals(Query const & q, StudyConfig const & cfg) {
  size_t max_list_size = cfg.maxListSize;

  // Determine the (up to) two verticals part of the computation.
  const JoinStatement & join = *q.joinStatement;
  if (join.joinOns.empty()) {
    return GlobalInfo(max_list_size, kNumDataowners);
  }
  // TODO: This assumes a single Join statement, and exactly two verticals.
  // So this needs to be generalized to support fewer (one) and more verticals.
  const std::pair<JoinOn, JoinOn> & join_columns = join.joinOns[0];
  const ColumnSpec & first_col = join_columns.first.col;
  const ColumnSpec & second_col = join_columns.second.col;
  const VerticalIndex_t first_vert = first_col.vertical;
  const VerticalIndex_t second_vert = second_col.vertical;

  size_t num_dataowners = 0;
  for (auto it = cfg.peers.begin(); it != cfg.peers.end(); ++it) {
    const Peer & other = it->second;
    if (other.isDataowner() &&
        (other.dataowner.verticalIdx == first_vert ||
         other.dataowner.verticalIdx == second_vert)) {
      num_dataowners++;
    }
  }

  // Parse bits of precision, bytes per table cell, and max num table rows
  // from Study Config, if present; otherwise use defaults.
  size_t bits_of_precision =
      GlobalInfo::BITS_OF_PRECISION_DEFAULT_VALUE;
  size_t bytes_per_table_cell =
      GlobalInfo::BYTES_IN_LOOKUP_TABLE_CELLS_DEFAULT_VALUE;
  size_t max_table_rows =
      GlobalInfo::MAX_F_T_TABLE_NUM_ROWS_DEFAULT_VALUE;

  // for (const auto & f : cfg.allowedQueries) {
  //   if (f->type == FunctionType::LIN_REGRESSION) {
  //     const LinearRegressionFunction & func =
  //         *((LinearRegressionFunction *)f.get());
  //     bits_of_precision = func.bits_of_precision;
  //     bytes_per_table_cell = func.num_bytes_in_f_t_table_cells;
  //     max_table_rows = func.max_f_t_table_rows;
  //   }
  // }

  if (q.function->type == FunctionType::LIN_REGRESSION) {
    const LinearRegressionFunction & func =
        *((LinearRegressionFunction *)q.function.get());
    bits_of_precision = func.bits_of_precision;
    bytes_per_table_cell = func.num_bytes_in_f_t_table_cells;
    max_table_rows = func.max_f_t_table_rows;
  } else if (q.function->type == FunctionType::MOMENT) {
    const MomentFunction & func = *((MomentFunction *)q.function.get());
    bits_of_precision = func.bits_of_precision;
  }

  return GlobalInfo(
      max_list_size,
      num_dataowners,
      max_list_size * num_dataowners,
      bits_of_precision,
      bytes_per_table_cell,
      max_table_rows);
}

} // namespace dataowner
} // namespace safrn
