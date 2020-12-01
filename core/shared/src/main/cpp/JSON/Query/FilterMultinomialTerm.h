/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 *
 * DESCRIPTION: Class which represents any multinomial (multiplicative) term.
 * The most common cases are:
 *   - Constant: Set 'coefficient' to desired constant,
 *               and values to be empty.
 *   - Col A (monomial): Set 'coefficient' to zero,
 *                       and values has length one.
 */

#ifndef EA1C4ECB782446D9A9A2368A511359F3
#define EA1C4ECB782446D9A9A2368A511359F3

/* platform-specific includes */

/* c/c++ standard includes */
#include <utility>
#include <vector>

/* third-party library includes */
#include <nlohmann/json.hpp>

/* project-specific includes */
#include <JSON/Config/StudyConfig.h>
#include <JSON/Query/ColumnSpec.h>
#include <JSON/Query/QueryTypes.h>
#include <JSON/Query/RationalFraction.h>

namespace safrn {

class FilterMultinomialTerm {
public:
  explicit FilterMultinomialTerm(const nlohmann::json & json);
  FilterMultinomialTerm(
      const safrn::StudyConfig & study, const nlohmann::json & json);

  const ComparisonValue_t coefficient;

  class ColumnExponent {
  public:
    explicit ColumnExponent(const nlohmann::json & json);
    ColumnExponent(
        const safrn::StudyConfig & study, const nlohmann::json & json);

    const ColumnSpec col;
    const Exponent_t exp;

  private:
    static Exponent_t createExponent(const nlohmann::json & json);
  };

  // The following datastructure represents a single term in a multivariate
  // poynomial, e.g. x^2 * y * z^3.
  // The number of entries is the number of variables, and each entry
  // specifies the relevant column (which is a 'variable', e.g. 'x'
  // in the expression above) and the power.
  const std::vector<ColumnExponent> values;

private:
  static std::vector<ColumnExponent>
  createValues(const nlohmann::json & json);

  static std::vector<ColumnExponent> createValues(
      const safrn::StudyConfig & study, const nlohmann::json & json);
};

} // namespace safrn

#endif
