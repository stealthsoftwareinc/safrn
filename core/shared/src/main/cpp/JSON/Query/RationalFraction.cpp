/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* platform-specific includes */

/* c/c++ standard includes */

/* third-party library includes */

/* project-specific includes */
#include <Util/StringUtils.h>

/* same module include */
#include <JSON/Query/RationalFraction.h>

safrn::RationalFraction::RationalFraction(const nlohmann::json & json) :
    safrn::GCDReducedFraction(
        std::stoll(ExtractTokenNWithDefault(
            json, FRACTION_DELIMITER, DEFAULT_FRACTION_TOKEN, 0)),
        std::stoll(ExtractTokenNWithDefault(
            json, FRACTION_DELIMITER, DEFAULT_FRACTION_TOKEN, 1))) {
}

bool safrn::RationalFraction::operator==(
    const intmax_t & literal) const {
  return (denominator == 1) && (numerator == literal);
}

bool safrn::RationalFraction::operator==(
    const safrn::RationalFraction & literal) const {
  return (this->numerator == literal.numerator) and
      (this->denominator == literal.denominator);
}
