/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 *
 */

#ifndef SAFRN_UTILS_GENERATE_SAMPLE_DATA_MAIN_H
#define SAFRN_UTILS_GENERATE_SAMPLE_DATA_MAIN_H

/* C and POSIX Headers */

/* C++ Headers */
#include <cstdint>
#include <set>
#include <string>
#include <tuple>
#include <vector>

/* 3rd Party Headers */

/* SAFRN Headers */
#include "Util/data_structures.h" // For Sampling Parameters
#include "Util/string_utils.h"

namespace {

extern const std::size_t maxIdSize;
extern const int kNumCurrencyDecimalDigits;
extern const std::string kFileCommentChar;
extern std::string kConfigFile;
extern std::string kComma;
extern std::string kSingleQuote;
extern bool kPrintOrgId;
extern bool kPrintUserId;

/**
 *
 *  A function to convert an integer in [0-99] into a string of length
 *  exactly two.
 *
 *  \param int between 0 and 99.
 *  \return string representation of input.
 */
std::string AsString(const int input);

// ============================== Distribution Helper ================================
/**
 *  An enum type to represent the types of statistical distributions
 *  that are supported.
 */
enum DistributionType {
  UNKNOWN,
  NORMAL,
  UNIFORM,
  DISCRETE_UNIFORM,
  POISSON
};

/**
 *  An enum type to represent the output types supported by this data
 *  generation process.
 */
enum SafrnDatatype {
  DOUBLE,
};

/**
 *  A struct type to represent the properties of the statistical
 *  distirbution used to generate the sample data.
 */
struct SafrnDistribution {
  DistributionType type_{DistributionType::UNKNOWN};
  double mean_{0.0};
  double var_{0.0};
  double max_{0.0};

  /**
   *  The default constructor, which produces an empty instance.
   *
   */
  SafrnDistribution() = default;

  /**
   *
   *  A method to produce a formatted string of the SafrnDistribution.
   *  \return a formatted string of the distribution.
   */
  std::string ToString() const {
    return "type: " + string_utils::Itoa(static_cast<int>(type_)) +
        ", mean: " + string_utils::Itoa(mean_) +
        ", var: " + string_utils::Itoa(var_) +
        ", max: " + string_utils::Itoa(max_);
  }
};

/**
 *  A struct type to represent the properties of a single column of
 *  the sample data generated.
 */
struct ColumnInfo {
  SafrnDatatype datatype_{SafrnDatatype::DOUBLE};
  SafrnDistribution distribution_{};
  std::string column_name_{};
};

/**
 *  A struct type to represent properties of the sample data generated
 *  related to the study, such as number of participants,
 *  observations, parameters.
 */
struct VerticalInfo {
  std::size_t num_parties_{};
  std::size_t num_cols_{};
  std::size_t num_rows_{};
  std::string key_name_{};
  std::vector<std::size_t> num_rows_per_party_{};
  std::vector<ColumnInfo> column_info_{};
};

// ============================== Distributions ================================

/**
 *
 *  A function to produce sampling-related parameters for data
 *  generated according to a normal distribution.
 *
 *  \param mean the mean of the distribution.
 *  \param std_dev the standard deviation of the distribution.
 *  \return math_utils::SamplingParameters for a normal distribution.
 */
math_utils::SamplingParameters GetNormalDistSamplingParameters(
    const double & mean, const double & std_dev);

/**
 *
 *  A function to produce sampling-related parameters for data
 *  generated according to a normal distribution with mean 0.0 and
 *  standard deviation 1.0.
 *
 *  \return math_utils::SamplingParameters for a normal distribution
 *  with mean 0.0 and standard deviation 1.0.
 */
math_utils::SamplingParameters
GetStandardNormalDistSamplingParameters();

/**
 *
 *  A function to produce sampling-related parameters for data
 *  generated according to a uniform distribution from a range [left,
 *  right] of two real numbers.
 *
 *  \param left the lower bound of the range, expressed as a double.
 *  \param right the upper bound of the range, expressed as a double.
 *  \return math_utils::SamplingParameters for a uniform distribution.
 */
math_utils::SamplingParameters GetUniformDistSamplingParameters(
    const double & left, const double & right);

/**
 *
 *  A function to produce sampling-related parameters for data
 *  generated according to a discrete uniform distribution from a
 *  range [left, right] of two integers.
 *
 *  \param left the lower bound of the range, expressed as an integer.
 *  \param right the upper bound of the range, expressed as an integer.
 *  \return math_utils::SamplingParameters for a discrete uniform
 *  distribution.
 */
math_utils::SamplingParameters
GetDiscreteIntegerUniformDistSamplingParameters(
    const int & left, const int & right);

/**
 *
 *  A function to produce sampling-related parameters for data
 *  generated according to a discrete uniform distribution from a
 *  range [left, right] of two integers with a specified step size.
 *
 *  \param left the lower bound of the range, expressed as an integer.
 *  \param right the upper bound of the range, expressed as an integer.
 *  \param step_size the step size used to determine the possible values
 *   between left and right, expressed as an integer.
 *  \return math_utils::SamplingParameters for a discrete uniform distribution.
 */
math_utils::SamplingParameters
GetDiscreteDecimalUniformDistSamplingParameters(
    const double & left,
    const double & right,
    const double & step_size);

/**
 *
 *  A function to produce sampling-related parameters for data
 *  generated according to a Poission distribution.
 *
 *  \param lambda the mean of the distribution.
 *  \return math_utils::SamplingParameters for a Poisson distribution.
 */
math_utils::SamplingParameters
GetPoissonSamplingParameters(const int & lambda);

/**
 *
 *  A function to produce sampling-related parameters for data
 *  generated according to a generic distribution specified by the
 *  input parameters.
 *
 *  \param dist a instance of the class SafrnDistribution.
 *  \return math_utils::SamplingParameters for a generic distribution.
 */
math_utils::SamplingParameters
GetDistSamplingParameters(const SafrnDistribution & dist);

// ============================ END Distributions ==============================

} // namespace

/**
 *
 *  A function to parse the configuration file, retrieving various
 *  properties of the sample data to be generated. Note that the data
 *  is distributed among multiple parties.
 *
 *  \param output_dir a pointer to a string representing the location
 *  of the sample data to be generated.
 *  \param num_total_cols a pointer to an integer representing the
 *  total number of columns in the sample data after merging.
 *  \param num_total_common_rows a pointer to an integer representing
 *  the total number of observations after merging.
 *  \param num_vertical a pointer to an integer representing the number
 *  of vertical parties participating in the study.
 *  \param info_per_vertical a pointer to a vector of VerticalInfo. The
 *  length of this vector equals num_verticals, and each element stores
 *  the relevant information for each vertical to participate in the
 *  study.
 *  \return True iff file is parsed correctly.
 */
bool ParseConfigFile(
    std::string * output_dir,
    std::size_t * num_total_cols,
    std::size_t * num_total_common_rows,
    std::size_t * num_verticals,
    std::vector<VerticalInfo> * info_per_vertical);

/**
 *
 *  A function to generate, for each party participating in the study,
 *  the relevant information they need to particpate in the study.
 *
 *  \param output_dir a string representing the location of the sample
 *  data to be generated.
 *  \param header_line a string representing the header row of the csv
 *  to be generated.
 *  \param current_start_col_index a size_t to specifies the absolute
 *  column index (w.r.t. to entire db; i.e. all verticals) of the
 *  first column in this vertical.
 *  \param is_first_vertical a boolean to determine if this to the
 *  first vertical being processed (controls the set of ids in the
 *  intersection, and whether we write to this list or read from it).
 *  \param is_last_vertical a boolean to determine whether the data in
 *  the final column of this vertical should be generated directly (as
 *  per the specified sampling distribution) or as per the dependent
 *  variable of linear regression.
 *  \param v_index the index of the vertical to which this party belongs.
 *  \param p_index the index of this party.
 *  \param num_rows the number of observations of the data held by this party.
 *  \param col_info a vector of columInfo for the features of the data held by this party.
 *  \param coeffs a vector (of length the number of independent
 *  variables) of doubles. used to generate a dependent variable to
 *  measure the accuracy of the linear regression model.
 *  \param num_remaining_rows_total a size_t denoting how many rows
 *  still need to go into the intersection for this vertical (will
 *  control which row (ids) are marked as being in the intersection).
 *  \param num_remaining_rows_in_vert a size_t denoting how many rows
 *  still need to be generated for this vertical (will control which
 *  row (ids) are marked as being in the intersection
 *  \param used_ids A set of all user_ids already added to the sample
 *  data, used to control number of distinct vs. intersecting values
 *  in the join column.
 *  \param intersection_ids a vector of ids in the intersection across
 *  verticals.
 *  \param joined_table a vector of vector of doubles to hold the
 *  global "joined" database, reflecting the union of all data (all
 *  rows in all verticals).
 */
void GenerateDataForParty(
    const std::string & output_dir,
    const std::string & header_line,
    const std::size_t current_start_col_index,
    const bool is_first_vertical,
    const bool is_last_vertical,
    const std::size_t v_index,
    const std::size_t p_index,
    const std::size_t num_rows,
    const std::vector<ColumnInfo> & col_info,
    const std::vector<double> & coeffs,
    std::size_t * num_remaining_rows_total,
    std::size_t * num_remaining_rows_in_vert,
    std::set<std::size_t> * used_ids,
    std::vector<std::size_t> * intersection_ids,
    std::vector<std::vector<double>> * joined_table);

/**
 *
 *  A function to generate, for each vertical in the study, the
 *  relevant information needed to produce the sample data generated
 *  to each party within that vertical.
 *
 *  \param is_last_vertical A boolean to determine whether the data in
 *  the final column of this vertical should be generated directly (as
 *  per the specified sampling distribution) or as per the dependent
 *  variable of linear regression.
 *  \param output_dir a string representing the location of the sample
 *  data to be generated.
 *  \param header_line a string representing the header row of the csv
 *  to be generated.
 *  \param v_index the index of the vertical to which this party belongs.
 *  \param vert_info a struct containing metadata for this vertical
 *  \param current_start_col_index a size_t to specifies the absolute
 *  column index (w.r.t. to entire db; i.e. all verticals) of the
 *  first column in this vertical.
 *  \param coeffs a vector (of length the number of independent
 *  variables) of doubles. Used to generate a dependent variable to
 *  measure the accuracy of the linear regression model.
 *  \param used_ids A set of all user_ids already added to the sample
 *  data, used to control number of distinct vs. intersecting values
 *  in the join column.
 *  \param intersection_ids a vector of ids in the intersection across
 *  verticals.
 *  \param joined_table a vector of vector of doubles to hold the
 *  global "joined" database, reflecting the union of all data (all
 *  rows in all verticals).
 */
void GenerateDataForVertical(
    const bool is_last_vertical,
    const std::string & output_dir,
    const std::string & header_line,
    const std::size_t v_index,
    const VerticalInfo & vert_info,
    const std::size_t current_start_col_index,
    const std::vector<double> & coeffs,
    std::set<std::size_t> * used_ids,
    std::vector<std::size_t> * intersection_ids,
    std::vector<std::vector<double>> * joined_table);

#endif /* SAFRN_UTILS_GENERATE_SAMPLE_DATA_MAIN_H */
