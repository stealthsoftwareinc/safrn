/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 *
 * DESCRIPTION: Specifies an arbitrary (single-variate)
 * polynomial over a variable of type 'T'.
 * This polynomial is defined as a vector
 * (i^th entry is the coefficient of the x^i term)
 * of the coefficients.
 */

#ifndef INC_7407B120F3DC490A9DC29CD2C2AA2E4A
#define INC_7407B120F3DC490A9DC29CD2C2AA2E4A

/* platform-specific includes */

/* c/c++ standard includes */
#include <exception>
#include <vector>

/* third-party library includes */
#include <nlohmann/json.hpp>

/* project-specific includes */

namespace safrn {

template<class T>
class Polynomial {
public:
  explicit Polynomial(const nlohmann::json & json) :
      coefficients(coefficientsFromJson(json)) {
  }

  bool IsIdentity() const {
    return (
        coefficients.size() == 2 && coefficients[0] == 0 &&
        coefficients[1] == 1);
  }

  const std::vector<T> coefficients;

  class NotEnoughCoefficients : std::exception {
    const char * what() const noexcept override {
      return "Not enough coefficients.";
    }
  };

  class LastElementZero : std::exception {
    const char * what() const noexcept override {
      return "Last element is zero.";
    }
  };

private:
  static std::vector<T>
  coefficientsFromJson(const nlohmann::json & json) {

    if (json.empty()) {
      throw NotEnoughCoefficients();
    }

    if (T(json.back()) == 0) {
      throw LastElementZero();
    }

    std::vector<T> coefficients;
    for (const auto & jsonCoefficient : json) {
      coefficients.emplace_back(static_cast<T>(jsonCoefficient));
    }

    return coefficients;
  }
};

} // namespace safrn

#endif
