/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* C and POSIX Headers */

/* C++ Headers */
#include <array>
#include <string>

/* 3rd Party Headers */

/* Fortissimo Headers */
#include <ff/Message.h>
#include <ff/Util.h>

/* SAFRN Headers */
#include <Identity.h>
#include <Util/Utils.h>

namespace safrn {

role_t strToRole(std::string const & str) {
  if (str == "analyst") {
    return ROLE_ANALYST;
  }
  if (str == "dataowner") {
    return ROLE_DATAOWNER;
  }
  if (str == "dealer") {
    return ROLE_DEALER;
  }
  if (str == "recipient") {
    return ROLE_RECIPIENT;
  }

  return ROLE_INVALID;
}

std::string roleToStr(role_t const role) {
  if (role == ROLE_ANALYST) {
    return "analyst";
  }
  if (role == ROLE_DATAOWNER) {
    return "dataowner";
  }
  if (role == ROLE_DEALER) {
    return "dealer";
  }
  if (role == ROLE_RECIPIENT) {
    return "recipient";
  }

  return "invalid";
}

Identity::Identity(dbuid_t const & o, role_t r, size_t v) :
    orgId(o), role(r), vertical(v) {
}

Identity::Identity(char const * const o, role_t r, size_t v) :
    role(r), vertical(v) {
  std::string str(o);
  strToDbuid(str, this->orgId);
}

bool Identity::operator==(Identity const & other) const {
  if (this->role == ROLE_DATAOWNER) {
    return this->orgId == other.orgId && this->role == other.role &&
        this->vertical == other.vertical;
  } else {
    return this->orgId == other.orgId && this->role == other.role;
  }
}

bool Identity::operator!=(Identity const & other) const {
  return !(*this == other);
}

bool Identity::operator<(Identity const & other) const {
  if (this->orgId != other.orgId) {
    return this->orgId < other.orgId;
  }
  if (this->role != other.role) {
    return this->role < other.role;
  }

  if (this->role == ROLE_DATAOWNER) {
    return this->vertical < other.vertical;
  } else {
    return false;
  }
}

bool Identity::operator<=(Identity const & other) const {
  return *this == other || *this < other;
}

bool Identity::operator>(Identity const & other) const {
  return !(*this <= other);
}

bool Identity::operator>=(Identity const & other) const {
  return !(*this < other);
}

} // namespace safrn

namespace ff {

template<>
::std::string
identity_to_string<safrn::Identity>(safrn::Identity const & id) {
  std::string ret = "{ ";
  ret += safrn::dbuidToStr(id.orgId);
  ret += ", ";
  ret += safrn::roleToStr(id.role);
  if (id.role == safrn::ROLE_DATAOWNER) {
    ret += ", ";
    ret += std::to_string(id.vertical);
  }
  ret += " }";
  return ret;
}

} // namespace ff
