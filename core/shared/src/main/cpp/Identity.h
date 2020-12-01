/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

#ifndef SAFRN_IDENTITY_H_
#define SAFRN_IDENTITY_H_

/* C and POSIX Headers */

/* C++ Headers */
#include <cstdint>

/* 3rd Party Headers */
#include <ff/Message.h>

/* SAFRN Headers */
#include <Util/Utils.h>

namespace safrn {

/**
 * flag set to define the Analyst, Dataowner, Dealer, and Recipient roles.
 */
using role_t = uint8_t;
constexpr role_t ROLE_ANALYST = 0x01;
constexpr role_t ROLE_DATAOWNER = 0x02;
constexpr role_t ROLE_DEALER = 0x04;
constexpr role_t ROLE_RECIPIENT = 0x08;
constexpr role_t ROLE_INVALID = 0xFF;

role_t strToRole(std::string const & str);

struct Identity {
  dbuid_t orgId = DBUID_INVALID;
  role_t role = ROLE_INVALID;
  size_t vertical = SIZE_MAX;

  Identity() = default;
  Identity(Identity const &) = default;
  Identity(Identity &&) = default;
  Identity(dbuid_t const & o, role_t r, size_t v);
  Identity(char const * const o, role_t r, size_t v = SIZE_MAX);

  Identity & operator=(Identity &&) = default;
  Identity & operator=(Identity const &) = default;

  bool operator==(Identity const & other) const;
  bool operator!=(Identity const & other) const;
  bool operator<(Identity const & other) const;
  bool operator<=(Identity const & other) const;
  bool operator>(Identity const & other) const;
  bool operator>=(Identity const & other) const;
};

} // namespace safrn

#include <Identity.t.h>

#endif // SAFRN_IDENTITY_H_
