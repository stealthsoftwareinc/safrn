/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* C and POSIX Headers */

/* C++ Headers */
#include <cstdint>
#include <deque>
#include <memory>
#include <vector>

/* 3rd Party Headers */

/* SAFRN Headers */
#include <EventWrapper/Message.h>

#ifndef SAFRN_QUERY_HANDLER_H_
#define SAFRN_QUERY_HANDLER_H_

namespace safrn {

struct QueryHandler {
  QueryContext context;

  QueryHandler(
      QueryId const & qid,
      StudyConfig const * study,
      dbuid_t const & orgId);

  /**
   * First function invoked for the query after setup.
   * Perform query setup tasks and begin sending messages.
   */
  void init();

  /**
   * Function called by framework upon receipt of messages.
   *
   * Note that due to the asynchronous network, this may be invoked before
   * ``init()``.
   */
  void update(
      IncomingMessage & msg,
      fronctocolid_t fronctocolId,
      role_t myRole,
      uint8_t controlBlock);

  /**
   * Check if this has completed processing the query.
   */
  bool isFinished();

  /**
   * Check if all complete messages have been received from peers.
   */
  bool isClosed();

private:
  FronctocolManager dealer;
  FronctocolManager recipient;
  FronctocolManager dataowner;

  FronctocolManager * chooseRole(role_t r);

  void handleSync(
      IncomingMessage & msg, role_t myRole, fronctocolid_t fronctocol);

  void handlePayload(
      IncomingMessage & msg, role_t myRole, fronctocolid_t fronctocol);

  void handleComplete(
      IncomingMessage & msg, role_t myRole, fronctocolid_t fronctocol);

  /* Mechanism for message caching before a fronctocol is created */
  bool initialized = false;
  std::deque<std::unique_ptr<IncomingMessageCache>> messageCache;

  void sendOutgoingMessages(std::vector<OutgoingMessageCache> & msgs);
};
} // namespace safrn

#endif //SAFRN_QUERY_HANDLER_H_
