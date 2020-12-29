/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* C and POSIX Headers */

/* C++ Headers */
#include <sstream>
#include <string>

/* 3rd Party Headers */

/* project-specific includes */
#include <sst/rep/from_hex.h>
#include <sst/rep/to_hex.h>

/* same module include */
#include <Util/Utils.h>

namespace safrn {

bool json_contains(json const & j, std::string const & key) {
  return j.find(key) != j.end();
}

std::string json_string(json const & j) {
  std::string ret = j;
  return ret;
}

dbuid_t const DBUID_INVALID = {
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF};

void strToDbuid(std::string const & str, dbuid_t & ref) {
  sst::rep::from_hex_exact(
      str.begin(), str.end(), ref.begin(), ref.end());
}

std::string dbuidToStr(dbuid_t const & dbuid) {
  return sst::rep::to_hex(dbuid);
}

} // namespace safrn
