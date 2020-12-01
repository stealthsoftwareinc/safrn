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
#include <EventWrapper/evwrapper.h>
#include <JSON/Config/ServerConfig.h>
#include <JSON/Config/StudyConfig.h>

#include <network/EventHandler.h>

#include <framework/QueryHandler.h>

#ifndef SAFRN_NEW_CONNECTION_HANDLER_H_
#define SAFRN_NEW_CONNECTION_HANDLER_H_

namespace safrn {
/**
   * This class awaits connections and handles them by creating a new
   * query, or adding the connection to an existing query.
   */
struct NewConnectionHandler {
  /**
     * The event2 connection listener.
     * This is just here to delete it eventually.
     */
  evw_unique_ptr<evconnlistener> connListener;

  ServerConfig config;

  /**
     * A map of all the Studies in which this server is authorized to
     * participate.
     */
  std::map<dbuid_t, std::unique_ptr<StudyConfig>> studies;

  /**
     * A map of all the queries being processed.
     */
  std::map<QueryId, std::unique_ptr<QueryHandler>> queries;

  explicit NewConnectionHandler() = default;
};

/**
   * libevent callback function for new connections.
   */
void newConnectionHandler(
    evconnlistener * listener,
    evutil_socket_t fd,
    sockaddr * addr,
    int socklen,
    void * ctxt);

/**
 * Libevent callback for a connection error.
 */
void newConnectionError(evconnlistener * listener, void * ptr);

/** Constants describing the connection header */
const uint8_t CONNHDR_CONST_DELIVERY = 0x00;
const uint8_t CONNHDR_CONST_DISTRIBUTION = 0x01;
const uint8_t CONNHDR_CONST_GREETING = 0x02;
const uint8_t CONNHDR_CONST_UNINIT = 0x03;

struct NewConnectionHeader {
  NewConnectionHandler * handler;

  uint8_t hdr = CONNHDR_CONST_UNINIT;

  std::string ipAndPort;
};

/**
   * libevent callback function for the setup data received from a connection.
   */
void readConnectionHeaderHandler(bufferevent * buf, void * ctxt);

/**
 * Libevent callback for error while reading header.
 */
void readConnectionHeaderError(
    bufferevent * bev, short events, void * ptr);

/**
   * specialized callback argument for reading the QueryJson object off the
   * wire.
   */
struct ReadQueryJsonHandler {
  QueryHandler * handler;

  /**
   * expected length of the query.
   */
  size_t length;

  /**
   * Whether to distribute the queries or not. (whether this server is the
   * analyst's proxy or not)
   */
  bool sendQueries;

  /**
   * Reference to the NewConnectionHandler which invoked this handler,
   * used for cleanup in the case of a bad Query JSON.
   */
  NewConnectionHandler * newConnHandler;

  /**
   * The organization which sent the query to me (proxy server).
   */
  dbuid_t sender;
};

/**
   * libevent callback function for reading the query from the connection.
   */
void readQueryJsonHandler(bufferevent * buf, void * ctxt);

/**
 * libevent callback for handling errors while attempting to read the query.
 */
void readQueryJsonError(bufferevent * buf, short events, void * ptr);

/**
   * Helper function for sending new connections to other servers.
   */
void sendConnectionsToPeers(QueryHandler & handler);

/**
   * Helper function for distributing new queries to other servers.
   */
void sendQueryToPeers(
    QueryHandler & handler, std::string const & queryStr);

/**
 * Helper class to hold information while sending a connection to a peer.
 */
struct SendConnectionInfo {
  QueryHandler * handler;
  dbuid_t sender;
};

/**
 * Helper function for error handling when sending connections to peers.
 */
void sendConnectionError(bufferevent * buf, short events, void * ptr);
} // namespace safrn

#endif //SAFRN_NEW_CONNECTION_HANDLER_H_
