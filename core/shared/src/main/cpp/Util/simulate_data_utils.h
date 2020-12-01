/**
 * Copyright (C) 2020 Stealth Software Technologies, Inc.
 *
 * Developed under IR&D
 */

#include <fstream>
#include <map>
#include <set>
#include <string>
#include <tuple>
#include <vector>

#include "data_structures.h"

#ifndef SIMULATE_DATA_UTILS_H
#define SIMULATE_DATA_UTILS_H

class SimulationUtils {
  // For Unit Tests.
  friend void ResetPnrgSeedForTest(const unsigned int);
  friend double SampleValueFromUniformTest(
      const double &, const double &, const unsigned int);
  friend double SampleValueFromNormalTest(
      const double &, const double &, const unsigned int);
  friend double
  SampleValueFromBernoulliTest(const double &, const unsigned int);
  friend double
  SampleValueFromPoissonTest(const int, const unsigned int);
  friend bool SimulateIndependentValuesTest(
      const math_utils::MathOperation &,
      const std::string &,
      const int,
      const double &,
      const unsigned int,
      const std::map<std::string, math_utils::SamplingParameters> &,
      const std::vector<math_utils::VariableTerm> &,
      std::map<std::string, double> *,
      double *,
      std::vector<std::string> *,
      std::string *);

public:
  // Samples a single value (as specified by the parameters in params, using 'seed' to
  // seed the PRG iff this is the first value sampled upon invocation of this program,
  // i.e. iff the static bool 'is_pnrg_initialized_' is false), and stores output in 'output'.
  // Returns true upon success, false otherwise.
  static bool SampleValue(
      const unsigned int seed,
      const math_utils::SamplingParameters & params,
      double * output);
  // Same as above, with default value '1' for 'seed'.
  static bool SampleValue(
      const math_utils::SamplingParameters & params, double * output) {
    return SampleValue(1, params, output);
  }
  // Similar to above, but does this simultaneously for many different variables.
  static bool SampleValues(
      const unsigned int seed,
      const int subject_index,
      const std::vector<std::vector<math_utils::SamplingParameters>> &
          sampling_params,
      std::map<std::string, double> * var_names_and_values,
      std::string * error_msg);

  // WARNING: This method should only be used for testing purposes.
  // For non-testing purposes, no need to directly set the seed (it will
  // be done via a call to SampleValues, SimulateValues, etc.).
  static void ResetPnrgSeed(const unsigned int seed);

private:
  static double SampleValueFromUniformOldPRG(long x, long y, long z);
  static double SampleValueFromUniform(
      const double & range_start,
      const double & range_end,
      const unsigned int seed);
  static double SampleValueFromNormal(
      const double & mean,
      const double & std_dev,
      const unsigned int seed);
  static double SampleValueFromBernoulli(
      const double & probability_of_success, const unsigned int seed);
  static double
  SampleValueFromPoisson(const int lambda, const unsigned int seed);
  static double SampleValueFromChiSquared(
      const double & degrees_of_freedom, const unsigned int seed);
};

#endif
