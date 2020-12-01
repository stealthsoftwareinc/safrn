/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* C and POSIX Headers */

/* C++ Headers */
#include <cstdint>
#include <map>

/* SAFRN Headers */
#include <JSON/Config/StudyConfig.h>
#include <Util/Utils.h>

#include <framework/QueryContext.h>
#include <network/EventHandler.h>

namespace safrn {

bool QueryId::operator<(QueryId const & other) const {
  return this->studyId < other.studyId &&
      this->analyst < other.analyst && this->queryId < other.queryId;
}

bool QueryId::operator>(QueryId const & other) const {
  return this->studyId > other.studyId &&
      this->analyst > other.analyst && this->queryId > other.queryId;
}

bool QueryId::operator<=(QueryId const & other) const {
  return !(*this > other);
}

bool QueryId::operator>=(QueryId const & other) const {
  return !(*this < other);
}

bool QueryId::operator==(QueryId const & other) const {
  return this->studyId == other.studyId &&
      this->analyst == other.analyst && this->queryId == other.queryId;
}

bool QueryId::operator!=(QueryId const & other) const {
  return !(*this == other);
}

QueryContext::QueryContext(
    QueryId const & qid,
    StudyConfig const * study,
    dbuid_t const & orgId) :
    study(study), queryId(qid), orgId(orgId) {
}

} // namespace safrn
