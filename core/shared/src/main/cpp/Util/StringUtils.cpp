/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* platform-specific includes */

/* c/c++ standard includes */
#include <string>

/* third-party library includes */

/* project-specific includes */

/* same module include */
#include "StringUtils.h"

std::string ExtractTokenN(
    const std::string & target,
    const std::string & token,
    const size_t n) {
  return ExtractTokenNWithDefault(target, token, "", n);
}

std::string ExtractTokenNWithDefault(
    const std::string & target,
    const std::string & token,
    const std::string & defaultToken,
    const size_t n) {
  size_t findPos = 0;
  size_t nextStart = findPos;
  size_t startPos = findPos;

  for (size_t n_it = 0; n_it < (n + 1); n_it++) {
    /* Store starting position of token. */
    startPos = nextStart;

    findPos = target.find(token, startPos);

    if (findPos == std::string::npos) {
      /* If we are at the right token,
       * but no more tokens remain,
       * just return the end of the string. */
      if (n == n_it) {
        return target.substr(startPos);
      }
      /* If we have not found enough
       * tokens, just return the default
       * token value. */
      else {
        return defaultToken;
      }
    }

    /* Skip over length of delimiter. */
    nextStart = findPos + token.size();
  }

  /* Return a substring with findPos as the
   * ending position and startPos as the start. */
  return target.substr(startPos, findPos - startPos);
}
