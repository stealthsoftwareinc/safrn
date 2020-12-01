/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 *
 * DESCRIPTION: Generic string utilities for
 * manipulating strings in ways the C++11
 * standard template library doesn't directly
 * support with a single function call.
 */

#ifndef INC_46D4B0ED64BF464AB9FC3289A35A47B7
#define INC_46D4B0ED64BF464AB9FC3289A35A47B7

/* platform-specific includes */

/* c/c++ standard includes */
#include <string>

/* third-party library includes */

/* project-specific includes */

/* Extract token number N from target
 * using an empty default token value
 * (which is returned if the token doesn't
 * exist). */
extern std::string ExtractTokenN(
    const std::string & target,
    const std::string & token,
    const size_t n);

/* Extract token number N from target
 * using an arbitrary default token value
 * (which is return if the token doesn't
 * exist). */
extern std::string ExtractTokenNWithDefault(
    const std::string & target,
    const std::string & token,
    const std::string & defaultToken,
    const size_t n);

#endif
