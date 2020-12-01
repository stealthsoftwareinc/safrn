/**
 * Copyright (C) 2020 Stealth Software Technologies, Inc.
 *
 * Developed under IR&D
 */

#include <map>
#include <random>
#include <string>
#include <vector>

#include "constants.h"
#include "data_structures.h"
#include "simulate_data_utils.h"
#include "string_utils.h"

using namespace math_utils;
using namespace string_utils;
using namespace std;

static mt19937 pnrg_;
static bool is_pnrg_initialized_ = false;

// This method only used for unit testing.
void SimulationUtils::ResetPnrgSeed(const unsigned int seed) {
  is_pnrg_initialized_ = true;
  pnrg_.seed(seed);
}

// Simulate uniform distribution on (0,1)
double
SimulationUtils::SampleValueFromUniformOldPRG(long x, long y, long z) {
  double r;
  static long gix, giy, giz;
  if (!is_pnrg_initialized_) {
    is_pnrg_initialized_ = true;
    gix = x;
    giy = y;
    giz = z;
  }

  gix = 171 * (gix % 177) - 2 * (gix / 177);
  giy = 172 * (giy % 176) - 35 * (giy / 176);
  giz = 170 * (giz % 178) - 63 * (giz / 178);

  if (gix < 0)
    gix = gix + 30269;
  if (giy < 0)
    giy = giy + 30307;
  if (giz < 0)
    giz = giz + 30323;

  r = fmod(
      (double)(gix) / 30269. + (double)(giy) / 30307. +
          (double)(giz) / 30323.,
      1.0);

  return r;
}

double SimulationUtils::SampleValueFromUniform(
    const double & range_start,
    const double & range_end,
    const unsigned int seed) {
  // We use 'seed' here (instead of e.g. the C++ built-in "random_device" class)
  // so that results are reproducible.
  if (!is_pnrg_initialized_) {
    is_pnrg_initialized_ = true;
    pnrg_.seed(seed);
  }
  uniform_real_distribution<double> sampler(range_start, range_end);
  return sampler(pnrg_);
}

double SimulationUtils::SampleValueFromBernoulli(
    const double & probability_of_success, const unsigned int seed) {
  // We use 'seed' here (instead of e.g. the C++ built-in "random_device" class)
  // so that results are reproducible.
  if (!is_pnrg_initialized_) {
    is_pnrg_initialized_ = true;
    pnrg_.seed(seed);
  }
  bernoulli_distribution sampler(probability_of_success);
  return sampler(pnrg_) ? 1.0 : 0.0;
}

double SimulationUtils::SampleValueFromPoisson(
    const int lambda, const unsigned int seed) {
  // We use 'seed' here (instead of e.g. the C++ built-in "random_device" class)
  // so that results are reproducible.
  if (!is_pnrg_initialized_) {
    is_pnrg_initialized_ = true;
    pnrg_.seed(seed);
  }
  poisson_distribution<long> sampler(static_cast<long>(lambda));
  return sampler(pnrg_);
}

double SimulationUtils::SampleValueFromNormal(
    const double & mean,
    const double & std_dev,
    const unsigned int seed) {
  // We use 'seed' here (instead of e.g. the C++ built-in "random_device" class)
  // so that results are reproducible.
  if (!is_pnrg_initialized_) {
    is_pnrg_initialized_ = true;
    pnrg_.seed(seed);
  }
  normal_distribution<double> sampler(mean, std_dev);
  return sampler(pnrg_);
}

double SimulationUtils::SampleValueFromChiSquared(
    const double & degrees_of_freedom, const unsigned int seed) {
  // We use 'seed' here (instead of e.g. the C++ built-in "random_device" class)
  // so that results are reproducible.
  if (!is_pnrg_initialized_) {
    is_pnrg_initialized_ = true;
    pnrg_.seed(seed);
  }
  chi_squared_distribution<double> sampler(degrees_of_freedom);
  return sampler(pnrg_);
}

bool SimulationUtils::SampleValue(
    const unsigned int seed,
    const SamplingParameters & params,
    double * output) {
  // Sanity check input.
  if (output == nullptr)
    return false;

  // Sample.
  switch (params.type_) {
    case Distribution::BERNOULLI: {
      *output = params.constant_ *
          SampleValueFromBernoulli(params.mean_, seed);
      break;
    }
    case Distribution::CONSTANT: {
      *output = params.constant_;
      break;
    }
    case Distribution::LOG_UNIFORM: {
      const double unif_valu = SampleValueFromUniform(
          params.range_start_, params.range_end_, seed);
      if (unif_valu <= 0.0)
        return false;
      *output = params.constant_ * log(unif_valu);
      break;
    }
    case Distribution::NORMAL: {
      *output = params.constant_ *
          SampleValueFromNormal(params.mean_, params.std_dev_, seed);
      break;
    }
    case Distribution::POISSON: {
      *output = params.constant_ *
          SampleValueFromPoisson(static_cast<int>(params.mean_), seed);
      break;
    }
    case Distribution::UNIFORM: {
      *output = params.constant_ *
          SampleValueFromUniform(
                    params.range_start_, params.range_end_, seed);
      break;
    }
    case Distribution::DISCRETE_UNIFORM: {
      // For discrete uniform, we define this as drawing uniformly in [a, b]
      // with 'step size' d, where [a,b] = range_start_/end_, and d = constant_.
      // Thus, there are ([b - a] / d) possible values to be chosen from.
      // Note that user should always use (a, b, d) values s.t.:
      //   - b > a
      //   - d | (b - a): 'd' should divide (b - a).
      // However, these conditions are not explicitly checked.
      const double unif_value = SampleValueFromUniform(
          // We add 'params.constant_' to range_end_ ('b') so that the
          // (continuous) uniform distribution chooses from (a, b+d).
          params.range_start_,
          params.range_end_ + params.constant_,
          seed);
      const int dist_from_left =
          (unif_value - params.range_start_) / params.constant_;
      *output = params.range_start_ + dist_from_left * params.constant_;
      break;
    }
    default:
      // Unrecognized/Unsupported distribution type.
      // TODO(PHB): Handle all distribution types in MathUtils/data_structures.h
      cout << "\nERROR Sampling for unsupported Distribution type: "
           << static_cast<int>(params.type_);
      return false;
  }
  return true;
}

bool SimulationUtils::SampleValues(
    const unsigned int seed,
    const int subject_index,
    const vector<vector<SamplingParameters>> & sampling_params,
    map<string, double> * var_names_and_values,
    string * error_msg) {
  for (const vector<SamplingParameters> & params_itr :
       sampling_params) {
    if (params_itr.empty()) {
      if (error_msg != nullptr) {
        *error_msg += "Sampling Parameter " + Itoa(subject_index) +
            " is empty.\n";
      }
      return false;
    }
    const string & var_name = params_itr[0].variable_name_;
    int matching_index = -1;
    for (int j = 0; j < params_itr.size(); ++j) {
      const SamplingParameters & params = params_itr[j];
      if ((params.first_sample_ == -1 && params.last_sample_ == -1) ||
          (params.first_sample_ == -1 &&
           params.last_sample_ >= subject_index + 1) ||
          (params.first_sample_ <= subject_index + 1 &&
           params.last_sample_ == -1) ||
          (params.first_sample_ <= subject_index + 1 &&
           params.last_sample_ >= subject_index + 1)) {
        matching_index = j;
        break;
      }
    }
    if (matching_index == -1) {
      if (error_msg != nullptr) {
        *error_msg +=
            "Unable to find a Sample range that contains Sample "
            "index " +
            Itoa(subject_index) + " for variable '" + var_name + "'.\n";
      }
      return false;
    }
    const SamplingParameters & params = params_itr[matching_index];

    double current_value;
    if (!SampleValue(seed, params, &current_value)) {
      if (error_msg != nullptr) {
        *error_msg +=
            "Unable to SampleValue for variable '" + var_name + "'.\n";
      }
      return false;
    }
    if (!var_names_and_values
             ->insert(make_pair(var_name, current_value))
             .second) {
      if (error_msg != nullptr) {
        *error_msg += "ERROR: Already sampled a value for variable '" +
            var_name + "'.\n";
      }
      return false;
    }
  }

  return true;
}
