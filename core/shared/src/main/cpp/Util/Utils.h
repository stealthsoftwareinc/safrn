/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

#ifndef SAFRN_UTILS_H_
#define SAFRN_UTILS_H_

/* C and POSIX Headers */

/* C++ Headers */
#include <array>
#include <exception>
#include <functional>
#include <map>
#include <memory>
#include <string>

/* project-specific includes */
#include <nlohmann/json.hpp>

/* SAFRN Headers */

namespace safrn {
using json = nlohmann::json;

/**
 * helper for determining if a json object contains a given key.
 */
bool json_contains(json const & j, std::string const & key);

/**
 * helper for getting strings from JSON in v2.x.x
 */
std::string json_string(json const & j);

/**
   * Helper for determining if a std::map contains a given key.
   */
template<class M>
bool map_contains(M const & map, typename M::key_type const & key) {
  return map.find(key) != map.end();
}

/**
   * The Dashboard will issue Unique IDs for certain things.
   */
const size_t DBUID_LENGTH = 16;
using dbuid_t = std::array<uint8_t, DBUID_LENGTH>;
extern dbuid_t const DBUID_INVALID;

/**
   * Helper function to read a DBUID from a string.
   */
void strToDbuid(std::string const & str, dbuid_t & ref);

/**
 * Helper function (mainly for debugging) that converts a DBUID into a string.
 */
std::string dbuidToStr(dbuid_t const & dbuid);

//
// Lets us easily declare a std::unique_ptr with a custom deleter.
//

template<class T>
using customUniquePtr_t = std::unique_ptr<T, std::function<void(T *)>>;

//
// Quotes a string for error display purposes. This means converting it
// into an ASCII only JSON string and replacing invalid UTF-8 sequences
// with U+FFFD.
//

inline std::string quoteForError(std::string const & s) {
#if defined(NLOHMAN_JSON_VERSION_MAJOR) && \
    NLOHMAN_JSON_VERSION_MAJOR < 3
  return json(s).dump(-1, ' ', true, json::error_handler_t::replace);
#else
  return std::string("\"") + s + "\"";
#endif
}

} // namespace safrn

#endif //SAFRN_UTILS_H_
