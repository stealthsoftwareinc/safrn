/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* C and POSIX Headers */

/* C++ Headers */
#include <climits>
#include <cstdint>

/* 3rd Party Headers */

/* SAFRN Headers */
#include <EventWrapper/Message.h>
#include <EventWrapper/evwrapper.h>

#ifndef SAFRN_EVENT_HANDLER_H_
#define SAFRN_EVENT_HANDLER_H_

namespace safrn {
struct QueryHandler;

struct EventHandler {
  /**
     * The organization ID of the "sender", the peer who sends messages on
     * this connection.
     */
  dbuid_t senderOrg;

  /**
     * The query that this connection should route connections to.
     */
  QueryHandler * handler;

  /**
     * The bufferevent that this handler sends to and receives from.
     */
  evw_unique_ptr<bufferevent> bufevt;

  bool hasCachedHeader = false;
  struct {
    uint8_t controlBlock;
    role_t senderRole;
    fronctocolid_t id;
    role_t receiverRole;
    uint32_t length;
  } cachedHeader;
};

/**
 * Libevent callback for when the bufferevent is ready to read.
 */
void readEventHandler(bufferevent * bev, void * ptr);

/**
 * Libevent callback for when the bufferevent is ready to write.
 *
 * This is used to close the query after all the shutdown logic is complete.
 */
void writeEventHandler(bufferevent * bev, void * ptr);

/**
 * Libevent callback for when there is an error with the bufferevent.
 */
void eventHandlerError(bufferevent * bev, short events, void * ptr);
} // namespace safrn

#endif //SAFRN_EVENT_HANDLER_H_
