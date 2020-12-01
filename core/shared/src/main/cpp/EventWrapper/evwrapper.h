/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

#ifndef SAFRN_EVENT_WRAPPER_H_
#define SAFRN_EVENT_WRAPPER_H_

/* C and POSIX Headers */
//PHB#include <netinet/in.h>
//PHB#include <sys/socket.h>

/* C++ Headers */
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

/* 3rd Party Headers */
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/bufferevent_ssl.h>
#include <event2/dns.h>
#include <event2/event.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <openssl/err.h>
#include <openssl/ssl.h>

/* SAFRN Headers */
#include <EventWrapper/EvBufferWrapper.h>
#include <Util/Utils.h>

/* logging config */
#include <ff/logging.h>

namespace safrn {

/**
   * helper functions for deleting event2 objects from a smart ptr.
   */
class evw_deleter {
public:
  void operator()(evutil_addrinfo * ptr);
  void operator()(bufferevent * ptr);
  void operator()(evbuffer * ptr);
  void operator()(evconnlistener * ptr);
  void operator()(evutil_socket_t * ptr) const noexcept;
};

/**
   * Specialization of std::unique_ptr for event2 objects.
   */
template<typename T>
using evw_unique_ptr = std::unique_ptr<T, evw_deleter>;

/**
   * Single event_base object for whole program.
   */
extern event_base * evtbase;

/**
   * Single evdns_base object for whole program.
   */
extern evdns_base * dnsbase;

/**
   * initial setup for event wrappers.
   */
void evw_startup();

/**
   * teardown actions for event wrappers.
   */
void evw_teardown();

//
// Returns a string describing the current error of OpenSSL.
//

std::string getOpenSSLErrorString();

//
// Single SSL_CTX object for the whole program.
//

extern SSL_CTX * Global_SSL_Context;

//
// Initializes OpenSSL and creates Global_SSL_Context.
//
// ca_file and ca_path should be paths to a file and a directory,
// respectively, as described by the documentation for OpenSSL's
// SSL_CTX_load_verify_locations() function.
//
// crt_file and key_file should be paths to files that store this peer's
// certificate and private key, respectively, both of which should be in
// PEM format.
//
// It is acceptable to pass null pointers for any of the arguments. If
// ca_file and ca_path are both null pointers, then peer authentication
// will be disabled. If at least one of crt_file and key_file is a null
// pointer, then this peer will fail to be authenticated by other peers.
//
// It is acceptable to call this function more than once. Any calls
// after the first call will have no effect.
//
// It is acceptable to call this function from multiple threads. In this
// case, each call sychronizes with the next call.
//
// If this function throws an exception, it is acceptable to retry the
// call.
//

void initializeOpenSSL(
    std::string const & ca_file,
    std::string const & ca_path,
    std::string const & crt_file,
    std::string const & key_file);

//
// Starts a server socket bufferevent.
//
// This should be called inside an evconnlistener_cb callback.
//
// If addr_str is not a null pointer, it will be set to a "host:port"
// text representation of the connecting peer's address.
//
// If cbarg is not a null pointer and this function succeeds,
// cbarg->release() will be called to conceptually transfer the
// ownership of the underlying object to the readcb, writecb, and
// eventcb callbacks of the newly created bufferevent.
//
// If this function throws an exception, fd will be closed and
// cbarg->release() will not be called.
//

template<class UniquePtr>
bufferevent * startServerSocket(
    evutil_socket_t fd,
    sockaddr * const addr,
    int const socklen,
    std::string * const addr_str,
    bufferevent_data_cb const readcb,
    bufferevent_data_cb const writecb,
    bufferevent_event_cb const eventcb,
    UniquePtr * const cbarg) {
  log_assert(Global_SSL_Context != nullptr);
  log_assert(addr != nullptr);
  log_assert(socklen > 0);
  static_cast<void>(socklen); // socklen is only used in assertions.

  evw_unique_ptr<evutil_socket_t> sock(&fd);

  if (addr->sa_family != AF_INET && addr->sa_family != AF_INET6) {
    throw std::runtime_error(
        "Unsupported addr->sa_family: " +
        std::to_string(addr->sa_family));
  }
  bool const ipv4 = addr->sa_family == AF_INET;
  log_assert(
      static_cast<unsigned int>(socklen) ==
      (ipv4 ? sizeof(sockaddr_in) : sizeof(sockaddr_in6)));

  std::string str(ipv4 ? INET_ADDRSTRLEN : INET6_ADDRSTRLEN, '\0');
  if (evutil_inet_ntop(
          addr->sa_family,
          ipv4 ?
              static_cast<void const *>(
                  &reinterpret_cast<sockaddr_in *>(addr)->sin_addr) :
              static_cast<void const *>(
                  &reinterpret_cast<sockaddr_in6 *>(addr)->sin6_addr),
          &str[0],
          str.size()) == nullptr) {
    throw std::runtime_error("evutil_inet_ntop() failed");
  }
  str.resize(str.find('\0'));
  str += ':';
  str += std::to_string(
      ipv4 ? ntohs(reinterpret_cast<sockaddr_in *>(addr)->sin_port) :
             ntohs(reinterpret_cast<sockaddr_in6 *>(addr)->sin6_port));
  log_debug("Received connection from %s", str.c_str());
  if (addr_str != nullptr) {
    *addr_str = std::move(str);
  }

  customUniquePtr_t<SSL> ssl(
      SSL_new(Global_SSL_Context), [](SSL * const p) { SSL_free(p); });
  if (ssl.get() == nullptr) {
    throw std::runtime_error(
        "SSL_new() failed: " + getOpenSSLErrorString());
  }
  SSL_set_accept_state(ssl.get());

  evw_unique_ptr<bufferevent> bev(bufferevent_openssl_socket_new(
      evtbase,
      *sock,
      ssl.get(),
      BUFFEREVENT_SSL_ACCEPTING,
      BEV_OPT_CLOSE_ON_FREE));
  if (bev.get() == nullptr) {
    throw std::runtime_error("bufferevent_openssl_socket_new() failed");
  }
  // At this point, bufferevent_openssl_socket_new() has succeeded and
  // sock and ssl are now owned by bev, which will destroy them when
  // bufferevent_free(bev) is eventually called. As such, we should
  // release them without triggering their deleters.
  sock.release();
  ssl.release();

  {
    int const s = bufferevent_enable(bev.get(), EV_READ | EV_WRITE);
    if (s != 0) {
      throw std::runtime_error(
          "bufferevent_enable() failed and returned " +
          std::to_string(s));
    }
  }

  // At this point, we're about to set up the callbacks for bev. This is
  // the last step of setting up bev and it cannot fail, so this is also
  // where we want to transfer the ownership of bev to itself. In other
  // words, the callbacks of bev will now be responsible for destroying
  // bev. We'll also transfer the ownership of cbarg, if it's given, to
  // bev. Lastly, we'll return an observer pointer to bev so the caller
  // can do something with it if they want.
  bufferevent * const p = bev.release();
  bufferevent_setcb(
      p,
      readcb,
      writecb,
      eventcb,
      cbarg == nullptr ? nullptr : cbarg->release());
  return p;
}

inline bufferevent * startServerSocket(
    evutil_socket_t fd,
    sockaddr * const addr,
    int const socklen,
    std::string * const addr_str,
    bufferevent_data_cb const readcb,
    bufferevent_data_cb const writecb,
    bufferevent_event_cb const eventcb,
    std::nullptr_t) {
  return startServerSocket(
      fd,
      addr,
      socklen,
      addr_str,
      readcb,
      writecb,
      eventcb,
      static_cast<std::unique_ptr<int> *>(nullptr));
}

//
// Starts a client socket bufferevent.
//
// If cbarg is not a null pointer and this function succeeds,
// cbarg->release() will be called to conceptually transfer the
// ownership of the underlying object to the readcb, writecb, and
// eventcb callbacks of the newly created bufferevent.
//
// If this function throws an exception, cbarg->release() will not be
// called.
//

template<class UniquePtr>
bufferevent * startClientSocket(
    std::string const & host,
    int const port,
    bufferevent_data_cb const readcb,
    bufferevent_data_cb const writecb,
    bufferevent_event_cb const eventcb,
    UniquePtr * const cbarg) {

  log_assert(Global_SSL_Context != nullptr);
  log_assert(port > 0);
  log_assert(port < 65536);

  customUniquePtr_t<SSL> ssl(
      SSL_new(Global_SSL_Context), [](SSL * const p) { SSL_free(p); });
  if (ssl.get() == nullptr) {
    throw std::runtime_error(
        "SSL_new() failed: " + getOpenSSLErrorString());
  }
  SSL_set_connect_state(ssl.get());

  evw_unique_ptr<bufferevent> bev(bufferevent_openssl_socket_new(
      evtbase,
      -1,
      ssl.get(),
      BUFFEREVENT_SSL_CONNECTING,
      BEV_OPT_CLOSE_ON_FREE));
  if (bev.get() == nullptr) {
    throw std::runtime_error("bufferevent_openssl_socket_new() failed");
  }
  // At this point, bufferevent_openssl_socket_new() has succeeded and
  // ssl is now owned by bev, which will destroy it when
  // bufferevent_free(bev) is eventually called. As such, we should
  // release it without triggering its deleter.
  ssl.release();

  {
    int const s = bufferevent_enable(bev.get(), EV_READ | EV_WRITE);
    if (s != 0) {
      throw std::runtime_error(
          "bufferevent_enable() failed and returned " +
          std::to_string(s));
    }
  }

  bufferevent_setcb(
      bev.get(),
      readcb,
      writecb,
      eventcb,
      cbarg == nullptr ? nullptr : cbarg->get());

  {
    int const s = bufferevent_socket_connect_hostname(
        bev.get(), dnsbase, AF_UNSPEC, host.c_str(), port);
    if (s != 0) {
      throw std::runtime_error(
          "bufferevent_socket_connect_hostname() failed");
    }
  }

  // At this point, we're done setting up bev, so this is also where we
  // want to transfer the ownership of bev to itself. In other words,
  // the callbacks of bev will now be responsible for destroying bev.
  // We'll also transfer the ownership of cbarg, if it's given, to bev.
  // Lastly, we'll return an observer pointer to bev so the caller can
  // do something with it if they want.
  if (cbarg != nullptr) {
    cbarg->release();
  }
  return bev.release();
}

inline bufferevent * startClientSocket(
    std::string const & host,
    int const port,
    bufferevent_data_cb const readcb,
    bufferevent_data_cb const writecb,
    bufferevent_event_cb const eventcb,
    std::nullptr_t) {
  return startClientSocket(
      host,
      port,
      readcb,
      writecb,
      eventcb,
      static_cast<std::unique_ptr<int> *>(nullptr));
}

//
// Returns a string describing the current error of a bufferevent,
// including the current error state of OpenSSL.
//

std::string getErrorString(bufferevent & bev);

//
// Looks up a hash algorithm by name. The returned reference is always
// valid and does not need to be released in any sense.
//

EVP_MD const & getHashAlgorithmByName(std::string const & name);

//
// Gets the fingerprint of this peer's certificate.
//

std::vector<unsigned char>
getMyCertificateFingerprint(EVP_MD const & algorithm);

//
// Gets the fingerprint of a connected peer's certificate.
//

std::vector<unsigned char> getPeerCertificateFingerprint(
    bufferevent & bev, EVP_MD const & algorithm);

} // namespace safrn

// Since this file is a header file, we need to use LOG_UNCLUDE to
// prevent this file's logging settings from interfering with later
// inclusions of <Util/Logging.h>
#define LOG_UNCLUDE
#include <ff/logging.h>

#endif //SAFRN_EVENT_WRAPPER_H_
