/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* C and POSIX Headers */

/* C++ Headers */
#include <algorithm>
#include <climits>
#include <cstdint>
#include <string>

/* 3rd Party Headers */

/* SAFRN Headers */
#include <EventWrapper/Message.h>
#include <EventWrapper/evwrapper.h>

#include <network/EventHandler.h>
#include <network/NewConnectionHandler.h>

#include <framework/QueryContext.h>
#include <framework/QueryHandler.h>

/* Logging Configuration */
#include <ff/logging.h>

namespace safrn {

static_assert(CHAR_BIT == 8, "Wrong CHAR_BIT");

static const size_t HEADER_LEN =
    sizeof(uint64_t) + 3 * sizeof(uint8_t) + sizeof(uint32_t);

/* Returns whether or not the header has been completely read. */
bool readEntireHeader(evbuffer * evbuf, EventHandler * eh) {
  EvBufferWrapper inBuf(evbuf);

  size_t lenAvail = inBuf.length();
  if (!eh->hasCachedHeader) {
    log_debug(
        "readEntireHeader: %s of %s bytes received",
        std::to_string(std::min(lenAvail, HEADER_LEN)).c_str(),
        std::to_string(HEADER_LEN).c_str());
    if (lenAvail >= HEADER_LEN) {
      inBuf.read<uint8_t>(eh->cachedHeader.controlBlock);
      inBuf.read<uint8_t>(eh->cachedHeader.senderRole);
      inBuf.read<uint64_t>(eh->cachedHeader.id);
      inBuf.read<uint8_t>(eh->cachedHeader.receiverRole);
      inBuf.read<uint32_t>(eh->cachedHeader.length);
      log_debug(
          "readEntireHeader: got %s",
          ("eh->cachedHeader.controlBlock = " +
           std::to_string(eh->cachedHeader.controlBlock) + ", " +
           "eh->cachedHeader.senderRole = " +
           std::to_string(eh->cachedHeader.senderRole) + ", " +
           "eh->cachedHeader.id = " +
           std::to_string(eh->cachedHeader.id) + ", " +
           "eh->cachedHeader.receiverRole = " +
           std::to_string(eh->cachedHeader.receiverRole) + ", " +
           "eh->cachedHeader.length = " +
           std::to_string(eh->cachedHeader.length))
              .c_str());
      eh->hasCachedHeader = true;
      return true;
    } else {
      log_debug(
          "EventHandler (%s) waiting for header.",
          dbuidToStr(eh->senderOrg).c_str());
      return false;
    }
  } else {
    return true;
  }
}

static inline void finishEvent(EventHandler * evthandler) {
  TestManager * tmgr = TestManager::getInstance();
  if (tmgr != nullptr && tmgr->runningTestsCount == 0) {
    event_base_loopbreak(evtbase);
  } else {
    evthandler->handler->context.connHandler->queries.erase(
        evthandler->handler->context.queryId);
  }
}

static inline void condFinishEvent(EventHandler * evthandler) {
  if (evthandler->handler->isClosed()) {
    log_info(
        "Query closed for query (study=%s, analyst=%s, id=%lu)",
        dbuidToStr(evthandler->handler->context.queryId.studyId)
            .c_str(),
        dbuidToStr(evthandler->handler->context.queryId.analyst)
            .c_str(),
        evthandler->handler->context.queryId.queryId);
    finishEvent(evthandler);
  }
}

void readEventHandler(bufferevent * bev, void * ptr) {
  EventHandler * evthandler = (EventHandler *)ptr;
  evbuffer * inBuf = bufferevent_get_input(bev);

  /* Read the entire message header before continueing */
  if (!readEntireHeader(inBuf, evthandler)) {
    return;
  }

  /* Ensure the entire message is available before continuing */
  size_t lenAvail = evbuffer_get_length(inBuf);
  size_t const lenWant = (size_t)evthandler->cachedHeader.length;
  log_trace(
      "readEventHandler: %s of %s bytes received",
      std::to_string(std::min(lenAvail, lenWant)).c_str(),
      std::to_string(lenWant).c_str());
  if (lenAvail < lenWant) {
    return;
  }

  /* Set up the IncomingMessage object and pass the header data and
     * the message to the QueryHandler.
     */
  if_debug if (TestManager::getInstance() != nullptr) {
    if (evthandler->cachedHeader.controlBlock == CTRLBLK_PAYLOAD) {
      log_info(
          "Processing payload message for %s from %s (fronctocol %lu)",
          dbuidToStr(evthandler->handler->context.orgId).c_str(),
          dbuidToStr(evthandler->senderOrg).c_str(),
          evthandler->cachedHeader.id);
    } else if (evthandler->cachedHeader.controlBlock == CTRLBLK_SYNC) {
      log_debug(
          "Processing sync message for %s from %s (fronctocol %lu)",
          dbuidToStr(evthandler->handler->context.orgId).c_str(),
          dbuidToStr(evthandler->senderOrg).c_str(),
          evthandler->cachedHeader.id);
    } else if (
        evthandler->cachedHeader.controlBlock == CTRLBLK_COMPLETE) {
      log_debug(
          "Processing complete message for %s from %s (fronctocol %lu)",
          dbuidToStr(evthandler->handler->context.orgId).c_str(),
          dbuidToStr(evthandler->senderOrg).c_str(),
          evthandler->cachedHeader.id);
    } else {
      log_fatal(
          "Processing unexpected message for %s from %s (fronctocol "
          "%lu)",
          dbuidToStr(evthandler->handler->context.orgId).c_str(),
          dbuidToStr(evthandler->senderOrg).c_str(),
          evthandler->cachedHeader.id);
    }
  }
  IncomingMessage msg(
      evthandler->senderOrg,
      evthandler->cachedHeader.senderRole,
      inBuf,
      evthandler->cachedHeader.length);
  evthandler->handler->update(
      msg,
      evthandler->cachedHeader.id,
      evthandler->cachedHeader.receiverRole,
      evthandler->cachedHeader.controlBlock);

  if (msg.length() > 0) {
    log_warn(
        "Incoming Message from %s incompletely consumed by fronctocol "
        "%lu",
        dbuidToStr(msg.peer).c_str(),
        evthandler->cachedHeader.id);
    msg.erase();
  }

  condFinishEvent(evthandler);

  /* Handle remaining received data by recursing */
  evthandler->hasCachedHeader = false;
  if (evbuffer_get_length(inBuf) > 0) {
    readEventHandler(bev, ptr);
  }
}

void writeEventHandler(bufferevent * bev, void * ptr) {
  (void)bev;
  EventHandler * evthandler = (EventHandler *)ptr;
  condFinishEvent(evthandler);
}

void eventHandlerError(bufferevent * bev, short events, void * ptr) {
  (void)bev; /* unused parameter, required by libevent. */

  EventHandler * evthandler = (EventHandler *)ptr;

  /* Per Libevent's documentation, read/write are exclusive of eachother,
   * as are the connect/eof/error/timeout.
   * http://www.wangafu.net/~nickm/libevent-2.0/doxygen/html/bufferevent_8h.html#a3b0191b460511796e5281474cd704ec4
   */
  char const * phase;
  if (events & BEV_EVENT_READING) {
    phase = "Reading event";
  } else if (events & BEV_EVENT_WRITING) {
    phase = "Writing event";
  } else {
    phase = "Connect event";
  }

  if (events & BEV_EVENT_CONNECTED) {
    log_debug(
        "EventHandler Connect success for %s",
        dbuidToStr(evthandler->senderOrg).c_str());
  } else if (events & BEV_EVENT_EOF) {
    if (evthandler->handler->isFinished()) {
      log_info(
          "Closing finished query after close event from peer %s",
          dbuidToStr(evthandler->senderOrg).c_str());
      finishEvent(evthandler);
    } else {
      log_error(
          "%s during EventHandler from %s: end of file",
          phase,
          dbuidToStr(evthandler->senderOrg).c_str());
      log_debug(
          "finished = %s, bytes available = %zu",
          evthandler->handler->isFinished() ? "true" : "false",
          evbuffer_get_length(bufferevent_get_output(bev)));
    }
  } else if (events & BEV_EVENT_ERROR) {
    log_error(
        "%s during EventHandler from %s: %s",
        phase,
        dbuidToStr(evthandler->senderOrg).c_str(),
        evutil_socket_error_to_string(EVUTIL_SOCKET_ERROR()));
  } else if (events & BEV_EVENT_TIMEOUT) {
    log_error(
        "%s during EventHandler from %s: timeout",
        phase,
        dbuidToStr(evthandler->senderOrg).c_str());
  } else {
    log_error(
        "%s during EventHandler from %s: unknown error",
        phase,
        dbuidToStr(evthandler->senderOrg).c_str());
  }
}
} // namespace safrn
