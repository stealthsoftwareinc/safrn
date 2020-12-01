/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* C and POSIX Headers */
#include <arpa/inet.h>
#include <string.h>

/* C++ Headers */
#include <cstdint>
#include <exception>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>

/* 3rd Party Headers */

/* SAFRN Headers */
#include <EventWrapper/evwrapper.h>
#include <JSON/Config/ServerConfig.h>
#include <JSON/Config/StudyConfig.h>

#include <network/EventHandler.h>
#include <network/NewConnectionHandler.h>

#include <framework/QueryContext.h>
#include <framework/QueryHandler.h>

/* Logging configuration */
#include <ff/logging.h>

namespace safrn {

/**
 * Check if the query has all the necessary connections and begin processing
 */
static void queryReady(QueryHandler & handler) {
  /* Return if we're missing any recipients */
  for (std::pair<dbuid_t, Peer> pair : handler.context.study->peers) {
    Peer const & peer = pair.second;
    if (peer.organizationId == handler.context.orgId) {
      continue; /* skip checking self */
    }

    if (peer.isRecipient() &&
        !map_contains(handler.context.peers, peer.organizationId)) {
      return;
    }

    if (peer.isDealer() &&
        !map_contains(handler.context.peers, peer.organizationId)) {
      return;
    }

    if (peer.isDataowner()) {
      for (VerticalColumns const & vc :
           handler.context.query.verticals) {
        if (peer.dataowner.verticalIdx == vc.verticalIdx &&
            !map_contains(handler.context.peers, peer.organizationId)) {
          return;
        }
      }
    }
  }
  /* Otherwise, set the quey to begin processing. */

  handler.init();
}

/**
 * Change the callbacks on a bufferevent to EventHandler callbacks instead of
 * readConnectionHeaderHandler.
 */
static void changeToEventHandlerCallback(
    bufferevent * bev, dbuid_t sender, QueryHandler * handler) {
  EventHandler * eh = new EventHandler();
  eh->senderOrg = sender;
  eh->handler = handler;
  eh->bufevt = evw_unique_ptr<bufferevent>(bev);

  if (map_contains(handler->context.peers, sender)) {
    log_error(
        "Duplicate connections for %s (%s)",
        handler->context.study->peers.find(sender)
            ->second.organizationName.c_str(),
        dbuidToStr(sender).c_str());
    bufferevent_free(bev);
    delete eh;
    return;
  }

  handler->context.peers[sender] = std::unique_ptr<EventHandler>(eh);
  bufferevent_setcb(
      bev, readEventHandler, nullptr, eventHandlerError, eh);

  log_info(
      "Connection to %s (%s) established successfully",
      handler->context.study->peers.at(sender).organizationName.c_str(),
      dbuidToStr(sender).c_str());

  queryReady(*handler);

  size_t lenAvail = evbuffer_get_length(bufferevent_get_input(bev));
  if (lenAvail > 0) {
    readEventHandler(bev, (void *)eh);
  }
}

void newConnectionHandler(
    evconnlistener *,
    evutil_socket_t const fd,
    sockaddr * const addr,
    int const socklen,
    void * const ctxt) {
  try {
    log_assert(addr != nullptr);
    log_assert(ctxt != nullptr);
    std::unique_ptr<NewConnectionHeader> nch(new NewConnectionHeader());
    nch->handler = static_cast<NewConnectionHandler *>(ctxt);
    startServerSocket(
        fd,
        addr,
        socklen,
        &nch->ipAndPort,
        readConnectionHeaderHandler,
        nullptr,
        readConnectionHeaderError,
        &nch);
  } catch (std::exception const & e) {
    try {
      log_error("%s", e.what());
    } catch (...) {
    }
  } catch (...) {
    try {
      log_error("unknown error");
    } catch (...) {
    }
  }
}

void newConnectionError(evconnlistener * listener, void * ptr) {
  (void)listener;
  (void)ptr; /* unused parameters, required by libevent */
  int err = EVUTIL_SOCKET_ERROR();
  log_error(
      "New connection error (%d): %s",
      err,
      evutil_socket_error_to_string(err));
}

void readConnectionHeaderHandler(bufferevent * buf, void * ctxt) {
  NewConnectionHeader * header = (NewConnectionHeader *)ctxt;

  EvBufferWrapper inBuf(bufferevent_get_input(buf));

  /** See the SAFRN Wiki framework-protocol document for protocol details. */

  /* Step 1. read header constant */
  {
    if (header->hdr == CONNHDR_CONST_UNINIT &&
        inBuf.length() > sizeof(uint8_t)) {
      inBuf.read<uint8_t>(header->hdr);

      if (header->hdr != CONNHDR_CONST_DELIVERY &&
          header->hdr != CONNHDR_CONST_DISTRIBUTION &&
          header->hdr != CONNHDR_CONST_GREETING) {
        log_error(
            "Unexpected connection header constant %hhu from %s",
            header->hdr,
            header->ipAndPort.c_str());
        bufferevent_free(buf);
        delete header;
        return;
      }
    } else if (header->hdr == CONNHDR_CONST_UNINIT) {
      log_debug(
          "waiting for header constant from %s",
          header->ipAndPort.c_str());
      return;
    }
  }

  /* Step 2. ensure that the remainder of the header is available */
  {
    if (header->hdr == CONNHDR_CONST_DELIVERY &&
        sizeof(dbuid_t) * 2 + sizeof(uint32_t) + sizeof(uint64_t) >
            inBuf.length()) {
      log_debug(
          "waiting for remainder of delivery header from %s (%zu "
          "bytes)",
          header->ipAndPort.c_str(),
          inBuf.length());
      return;
    } else if (
        header->hdr == CONNHDR_CONST_DISTRIBUTION &&
        sizeof(dbuid_t) * 3 + sizeof(uint32_t) + sizeof(uint64_t) >
            inBuf.length()) {
      log_debug(
          "waiting for remainder of distribution header from %s (%zu "
          "bytes)",
          header->ipAndPort.c_str(),
          inBuf.length());
      return;
    } else if (
        header->hdr == CONNHDR_CONST_GREETING &&
        sizeof(dbuid_t) * 3 + sizeof(uint64_t) > inBuf.length()) {
      log_debug(
          "waiting for remainder of greeting header from %s (%zu "
          "bytes)",
          header->ipAndPort.c_str(),
          inBuf.length());
      return;
    }
  }

  /* Step 3: Parse Message Header */
  QueryId queryId;
  dbuid_t sender;
  size_t queryLen = 0;

  {
    inBuf.read(queryId.studyId);
    inBuf.read(queryId.analyst);
    inBuf.read(queryId.queryId);

    if (header->hdr == CONNHDR_CONST_DISTRIBUTION ||
        header->hdr == CONNHDR_CONST_GREETING) {
      inBuf.read(sender);
    } else if (header->hdr == CONNHDR_CONST_DELIVERY) {
      sender = queryId.analyst;
    } else {
      log_fatal("Unexpected conection header 0x%02hhx", header->hdr);
    }

    if (header->hdr == CONNHDR_CONST_DELIVERY ||
        header->hdr == CONNHDR_CONST_DISTRIBUTION) {
      uint32_t queryLen32;
      inBuf.read(queryLen32);
      queryLen = (size_t)queryLen32;
    }
  }

  /* Step 4: Other verifications */
  {
    if (!map_contains(header->handler->studies, queryId.studyId)) {
      log_error(
          "New connection from illegal study: %s",
          dbuidToStr(queryId.studyId).c_str());
      bufferevent_free(buf);
      delete header;
      return;
    }

    if (!header->handler->studies[queryId.studyId]
             ->peers.at(queryId.analyst)
             .isAnalyst()) {
      log_error(
          "New connection with illegal analyst: %s",
          dbuidToStr(queryId.analyst).c_str());
      bufferevent_free(buf);
      delete header;
      return;
    }
  }

  /* Step 5: Verify identity using TLS Certificates */
  {
    if (!map_contains(
            header->handler->studies[queryId.studyId]->peers, sender)) {
      log_error(
          "New connection from unknown peer: %s",
          dbuidToStr(sender).c_str());
      bufferevent_free(buf);
      delete header;
      return;
    }

    Peer const & peer =
        header->handler->studies[queryId.studyId]->peers[sender];
    if (peer.certificateFingerprint !=
        getPeerCertificateFingerprint(
            *buf, *peer.certificateFingerprintAlgorithm)) {
      log_error(
          "Peer claimed to be someone they aren't: %s",
          dbuidToStr(sender).c_str());
      bufferevent_free(buf);
      delete header;
      return;
    }
  }

  /* Step 6: Assure that a QueryHandler exists for this query. */
  QueryHandler * qh = nullptr;
  {
    if (!map_contains(header->handler->queries, queryId)) {
      header->handler->queries[queryId] =
          std::unique_ptr<QueryHandler>(new QueryHandler(
              queryId,
              header->handler->studies[queryId.studyId].get(),
              header->handler->config.organizationId));

      qh = header->handler->queries[queryId].get();
      qh->context.connHandler = header->handler;
    } else {
      qh = header->handler->queries[queryId].get();
    }

    if (header->hdr == CONNHDR_CONST_DELIVERY) {
      qh->context.analystProxy = header->handler->config.organizationId;
    } else if (header->hdr == CONNHDR_CONST_DISTRIBUTION) {
      qh->context.analystProxy = sender;
    }

    log_info(
        "New connection from %s (%s), study %s, analyst %s, query "
        "%lu",
        qh->context.study->peers.find(sender)
            ->second.organizationName.c_str(),
        dbuidToStr(sender).c_str(),
        dbuidToStr(qh->context.study->studyId).c_str(),
        dbuidToStr(queryId.analyst).c_str(),
        queryId.queryId);
    log_debug(
        "Party: %s, Message header type: 0x%02hhx, Query JSON length: "
        "%zu",
        dbuidToStr(header->handler->config.organizationId).c_str(),
        header->hdr,
        queryLen);
  }

  /* Step 7a: if the header has a query, set up to read the query */
  /* In the case that this connection is from the analyst, parse the
     * query json object, possibly assigning a new callback to await the
     * remainder of the query object. */
  {
    if (header->hdr == CONNHDR_CONST_DELIVERY ||
        header->hdr == CONNHDR_CONST_DISTRIBUTION) {
      ReadQueryJsonHandler * rqjh = new ReadQueryJsonHandler();
      rqjh->handler = qh;
      rqjh->length = queryLen;
      rqjh->sendQueries = header->hdr == CONNHDR_CONST_DELIVERY;
      rqjh->newConnHandler = header->handler;
      rqjh->sender = sender;

      delete header;
      if (rqjh->length <= inBuf.length()) {
        readQueryJsonHandler(buf, (void *)rqjh);
        return;
      } else {
        bufferevent_setcb(
            buf,
            readQueryJsonHandler,
            nullptr,
            readQueryJsonError,
            rqjh);
        return;
      }
    }
  }

  /* Step 7b: Change the callbacks to EventHandler callbacks, because this one
     * only handles study startup. */
  delete header;
  changeToEventHandlerCallback(buf, sender, qh);
}

void readConnectionHeaderError(
    bufferevent * bev, short events, void * ptr) {
  NewConnectionHeader * header = (NewConnectionHeader *)ptr;

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
        "Read connection header connection successful for %s",
        header->ipAndPort.c_str());
  } else if (events & BEV_EVENT_EOF) {
    log_error(
        "%s during read connection header from %s: end of file",
        phase,
        header->ipAndPort.c_str());
  } else if (events & BEV_EVENT_ERROR) {
    log_error(
        "%s during read connection header from %s: %s",
        phase,
        header->ipAndPort.c_str(),
        getErrorString(*bev).c_str());
  } else if (events & BEV_EVENT_TIMEOUT) {
    log_error(
        "%s during read connection header from %s: timeout",
        phase,
        header->ipAndPort.c_str());
  } else {
    log_error(
        "%s during read connection header from %s: unknown error",
        phase,
        header->ipAndPort.c_str());
  }
}

void readQueryJsonHandler(bufferevent * buf, void * ctxt) {
  ReadQueryJsonHandler * rqjh = (ReadQueryJsonHandler *)ctxt;
  EvBufferWrapper inBuf(bufferevent_get_input(buf));

  /* Step 1. wait until the entire query has arrived. */
  if (rqjh->length <= inBuf.length()) {

    /* Step 2. read the query */
    std::string inStr;
    inStr.resize(rqjh->length);
    inBuf.remove(&inStr[0], rqjh->length);

    bool validJson = false;
    try {
      log_debug("query len %zu query: %s", rqjh->length, inStr.c_str());
      readQueryJsonFromString(inStr, rqjh->handler->context.query);
      validJson = true;
    } catch (...) {
      log_warn("failed to parse Query JSON");
    }

    /* Step 3. test if query is legal */
    if (!validJson ||
        !verifyAllowableQuery(
            rqjh->handler->context.query,
            *rqjh->handler->context.study)) {
      /* TODO: figure out how to shutdown the query. */
      log_warn(
          "Invalid Query JSON: Study=%s, Analyst=%s, Query=%lu",
          dbuidToStr(rqjh->handler->context.queryId.studyId).c_str(),
          dbuidToStr(rqjh->handler->context.queryId.analyst).c_str(),
          rqjh->handler->context.queryId.queryId);
      rqjh->newConnHandler->queries.erase(
          rqjh->handler->context.queryId);
      delete rqjh;
      return;
    }

    log_info(
        "Read query Study %s, Analyst %s, queryId %lu",
        dbuidToStr(rqjh->handler->context.queryId.studyId).c_str(),
        dbuidToStr(rqjh->handler->context.queryId.analyst).c_str(),
        rqjh->handler->context.queryId.queryId);

    /* Step 4. send bufferevent connections to peers, perhaps with the query */
    if (rqjh->sendQueries) {
      sendQueryToPeers(*rqjh->handler, inStr);
    } else {
      sendConnectionsToPeers(*rqjh->handler);
    }

    /* Step 5. configure the EventHandler callbacks, or close analyst
     * connection.
     */
    if (!rqjh->sendQueries) {
      changeToEventHandlerCallback(buf, rqjh->sender, rqjh->handler);
    } else {
      bufferevent_free(buf);
    }

    delete rqjh;
  }
}

void readQueryJsonError(bufferevent * bev, short events, void * ptr) {
  ReadQueryJsonHandler * rqjh = (ReadQueryJsonHandler *)ptr;

  (void)bev; /* unused parameter, required by libevent. */

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

  std::string sender;
  if (rqjh->handler->context.analystProxy ==
      rqjh->handler->context.orgId) {
    // sender is analyst, because I am analyst proxy
    sender = dbuidToStr(rqjh->handler->context.queryId.analyst);
  } else {
    // sender is analyst proxy, because I am not analyst proxy
    sender = dbuidToStr(rqjh->handler->context.analystProxy);
  }

  if (events & BEV_EVENT_CONNECTED) {
    log_debug(
        "Read query JSON connection successful for %s", sender.c_str());
  } else if (events & BEV_EVENT_EOF) {
    log_error(
        "%s during read query JSON from %s: end of file",
        phase,
        sender.c_str());
  } else if (events & BEV_EVENT_ERROR) {
    log_error(
        "%s during read query JSON from %s: %s",
        phase,
        sender.c_str(),
        getErrorString(*bev).c_str());
  } else if (events & BEV_EVENT_TIMEOUT) {
    log_error(
        "%s during read query JSON from %s: timeout",
        phase,
        sender.c_str());
  } else {
    log_error(
        "%s during read query JSON from %s: unknown error",
        phase,
        sender.c_str());
  }
}

static bufferevent *
init_connection(QueryHandler & handler, Peer const & peer) {
  try {
    QueryContext & ctxt = handler.context;
    if (map_contains(ctxt.peers, peer.organizationId)) {
      throw std::runtime_error(
          "Unexpected Error, sending duplicate connection to " +
          peer.organizationName + " (" +
          dbuidToStr(peer.organizationId) + ")");
    }
    std::unique_ptr<SendConnectionInfo> sci(new SendConnectionInfo());
    sci->handler = &handler;
    sci->sender = peer.organizationId;
    return startClientSocket(
        peer.domainOrIp,
        peer.port,
        nullptr,
        nullptr,
        sendConnectionError,
        &sci);
  } catch (std::exception const & e) {
    try {
      log_error("%s", e.what());
    } catch (...) {
    }
    throw e;
  } catch (...) {
    try {
      log_error("unknown error");
    } catch (...) {
    }
    throw;
  }
}

static void
sendConnectionToPeer(QueryHandler & handler, Peer const & peer) {
  bufferevent * bevout = init_connection(handler, peer);
  EvBufferWrapper connHeader(bufferevent_get_output(bevout));

  connHeader.write<uint8_t>(CONNHDR_CONST_GREETING);
  connHeader.write(handler.context.study->studyId);
  connHeader.write(handler.context.queryId.analyst);
  connHeader.write(handler.context.queryId.queryId);
  connHeader.write(handler.context.orgId);

  log_debug(
      "Sending greeting connection to %s",
      dbuidToStr(peer.organizationId).c_str());
}

void sendConnectionsToPeers(QueryHandler & handler) {
  QueryContext & ctxt = handler.context;
  for (std::pair<dbuid_t, Peer> p : ctxt.study->peers) {
    dbuid_t peerId = p.first;
    Peer const & peer = p.second;

    if (peerId > ctxt.orgId) {
      if (peer.isRecipient()) {
        sendConnectionToPeer(handler, peer);
      }

      if (peer.isDataowner()) {
        for (VerticalColumns & vc : ctxt.query.verticals) {
          if (vc.verticalIdx == peer.dataowner.verticalIdx) {
            sendConnectionToPeer(handler, peer);
          }
        }
      }
    }
  }
}

static void sendQueryToPeer(
    QueryHandler & handler,
    Peer const & peer,
    std::string const & queryStr) {

  bufferevent * bevout = init_connection(handler, peer);
  EvBufferWrapper queryHeader(bufferevent_get_output(bevout));

  queryHeader.write<uint8_t>(CONNHDR_CONST_DISTRIBUTION);
  queryHeader.write(handler.context.study->studyId);
  queryHeader.write(handler.context.queryId.analyst);
  queryHeader.write(handler.context.queryId.queryId);
  queryHeader.write(handler.context.orgId);
  queryHeader.write(queryStr);

  log_info(
      "Sending query connection to %s (%s)",
      peer.organizationName.c_str(),
      dbuidToStr(peer.organizationId).c_str());
}

void sendQueryToPeers(
    QueryHandler & handler, std::string const & queryStr) {
  QueryContext & ctxt = handler.context;
  for (std::pair<dbuid_t, Peer> p : ctxt.study->peers) {
    dbuid_t peerId = p.first;
    Peer const & peer = p.second;

    if (peerId != ctxt.orgId) {
      if (peer.isRecipient()) {
        sendQueryToPeer(handler, peer, queryStr);
      }

      if (peer.isDataowner()) {
        for (VerticalColumns & vc : ctxt.query.verticals) {
          if (vc.verticalIdx == peer.dataowner.verticalIdx) {
            sendQueryToPeer(handler, peer, queryStr);
          }
        }
      }
    }
  }
}

void sendConnectionError(bufferevent * buf, short events, void * ptr) {
  SendConnectionInfo * sci = (SendConnectionInfo *)ptr;

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

  std::string sender = dbuidToStr(sci->sender);

  if (events & BEV_EVENT_CONNECTED) {
    log_debug("Send Connection success for %s", sender.c_str());
  } else if (events & BEV_EVENT_EOF) {
    log_error(
        "%s during Send Connection from %s: end of file",
        phase,
        sender.c_str());
  } else if (events & BEV_EVENT_ERROR) {
    log_error(
        "%s during Send Connection from %s: %s",
        phase,
        sender.c_str(),
        getErrorString(*buf).c_str());
  } else if (events & BEV_EVENT_TIMEOUT) {
    log_error(
        "%s during Send Connection from %s: timeout",
        phase,
        sender.c_str());
  } else {
    log_error(
        "%s during Send Connection from %s: unknown error",
        phase,
        sender.c_str());
  }

  log_assert(
      map_contains(sci->handler->context.study->peers, sci->sender));
  Peer const & peer =
      sci->handler->context.study->peers.at(sci->sender);
  if (peer.certificateFingerprint !=
      getPeerCertificateFingerprint(
          *buf, *peer.certificateFingerprintAlgorithm)) {
    log_error(
        "Peer claimed to be someone they aren't: %s",
        dbuidToStr(sci->sender).c_str());
    bufferevent_free(buf);
    delete sci;
    return;
  }

  changeToEventHandlerCallback(buf, sci->sender, sci->handler);
  delete sci;
}
} // namespace safrn
