/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* platform-specific includes */

/* c/c++ standard includes */

/* third-party library includes */

/* project-specific includes */

/* same module include */
#include "EvBufferWrapper.h"
#include <Utils.h>

/* logging configuration */
#include <ff/logging.h>

safrn::EvBufferWrapper::EvBufferWrapper(evbuffer * other) :
    buffer(other) {
}

size_t safrn::EvBufferWrapper::length() const {
  return evbuffer_get_length(this->buffer);
}

void safrn::EvBufferWrapper::erase() {
  const size_t remainingBytes = this->length();
  log_assert(
      evbuffer_drain(this->buffer, remainingBytes) ==
          (int)evbuffer_status::success,
      "Failed to empty buffer.");
}

void safrn::EvBufferWrapper::remove(void * buf, size_t const len) {
  log_assert(
      evbuffer_remove(this->buffer, buf, len) == (int)len,
      "Failed to read buffer.");
}

void safrn::EvBufferWrapper::add(void const * buf, size_t const len) {
  log_assert(
      evbuffer_add(this->buffer, buf, len) ==
          (int)evbuffer_status::success,
      "Failed to write buffer.");
}

evbuffer * safrn::EvBufferWrapper::getEvBufferDirectAccess() {
  return this->buffer;
}

safrn::OwnedEvBufferWrapper::OwnedEvBufferWrapper() :
    EvBufferWrapper(evbuffer_new()) {
}

safrn::OwnedEvBufferWrapper::OwnedEvBufferWrapper(evbuffer * other) :
    OwnedEvBufferWrapper() {
  evbuffer_add_buffer(this->buffer, other);
}

safrn::OwnedEvBufferWrapper::OwnedEvBufferWrapper(
    evbuffer * other, const size_t dataLength) :
    OwnedEvBufferWrapper() {
  log_assert(
      evbuffer_remove_buffer(other, this->buffer, dataLength) ==
          (int)evbuffer_status::success,
      "Failed to read buffer during construction.");
}

safrn::OwnedEvBufferWrapper::~OwnedEvBufferWrapper() {
  evbuffer_free(this->buffer);
}
