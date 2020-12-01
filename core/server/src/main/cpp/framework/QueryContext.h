/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* C and POSIX Headers */

/* C++ Headers */
#include <cstdint>
#include <map>
#include <memory>

/* 3rd Party Headers */

/* SAFRN Headers */
#include <JSON/Config/StudyConfig.h>
#include <JSON/Query/Query.h>
#include <Util/Utils.h>
#include <network/EventHandler.h>

#ifndef SAFRN_QUERY_CONTEXT_H_
#define SAFRN_QUERY_CONTEXT_H_

namespace safrn {

/**
   * A given query is identified by a StudyID (assigned by dashboard) and
   * a query id (assigned sequentially by the analyst).
   */
struct QueryId {
  dbuid_t studyId;
  dbuid_t analyst; // analyst who started this query
  uint64_t queryId; // sequentially assigned by given analyst

  bool operator<(QueryId const & other) const;
  bool operator>(QueryId const & other) const;
  bool operator<=(QueryId const & other) const;
  bool operator>=(QueryId const & other) const;

  bool operator==(QueryId const & other) const;
  bool operator!=(QueryId const & other) const;
};

struct NewConnectionHandler;

struct QueryContext {

  QueryContext(
      QueryId const & qid,
      StudyConfig const * study,
      dbuid_t const & orgId);
  /**
     * Connections to all peers. It is used to send messages to peers,
     * and for freeing the bufferevents with RAII.
     */
  std::map<dbuid_t, std::unique_ptr<EventHandler>> peers;

  /**
     * Query object received from analyst at the beginning of query.
     */
  Query query;

  /**
     * Reference to this Query's study config.
     */
  StudyConfig const * study;

  /**
     * The QueryID of this query.
     */
  QueryId queryId;

  /**
   * The organization ID of the server who proxied the analyst's query.
   */
  dbuid_t analystProxy;

  /**
     * A copy of this server's organization ID.
     */
  dbuid_t orgId;

  /**
   * NewConnectionHandler reference, used during completion of a query
   * for cleanup.
   */
  NewConnectionHandler * connHandler;
};

} // namespace safrn

#endif //SAFRN_QUERY_CONTEXT_H_
