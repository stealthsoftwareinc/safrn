/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

#ifndef SAFRN_EVBUFFER_WRAPPER_H_
#define SAFRN_EVBUFFER_WRAPPER_H_

/* C and POSIX Headers */

/* C++ Headers */
#include <cassert>
#include <climits>
#include <cstdint>
#include <limits>
#include <vector>

/* 3rd Party Headers */
#include <event2/buffer.h>
#include <event2/bufferevent.h>

/* SAFRN Headers */
#include <Util/Utils.h>

namespace safrn {

class EvBufferWrapper {
public:
  /**
   * Create an EvBufferWrapper with a non-owned pointer to other.
   */
  explicit EvBufferWrapper(evbuffer * other);
  virtual ~EvBufferWrapper() = default;

  /* deleted functions */
  explicit EvBufferWrapper(EvBufferWrapper & other) = delete;
  explicit EvBufferWrapper(EvBufferWrapper && other) = delete;
  EvBufferWrapper & operator=(EvBufferWrapper & other) = delete;
  EvBufferWrapper && operator=(EvBufferWrapper && other) = delete;

  /**
   * Return the reported length available of the evbuffer.
   */
  virtual size_t length() const;

  /**
   * remove len many bytes from this buffer, and emplace them into buf.
   */
  virtual void remove(void * buf, size_t const len);
  /**
   * Copy len many bytes from buf into this buffer.
   */
  virtual void add(void const * buf, size_t const len);

  /**
   * Write various types of values into this buffer.
   */
  template<typename value_T>
  void write(value_T const & value);

  /**
   * Read various types of values from this buffer into the given reference.
   */
  template<typename value_T>
  void read(value_T & result);

  /**
   * remove all data in this buffer without any action.
   */
  void erase();

  /**
   * grants direct access to the underlying buffer. Use this with care.
   */
  evbuffer * getEvBufferDirectAccess();

protected:
  struct evbuffer * buffer;
  enum class evbuffer_status { success = 0, fail = -1 };
};

/**
 * EvBufferWrapper which takes ownership of its evbuffer pointer, and
 * responsibility for freeing it.
 */
class OwnedEvBufferWrapper : public EvBufferWrapper {
public:
  /**
   * Create a buffer with an owned evbuffer, which is empty initialized.
   */
  OwnedEvBufferWrapper();

  /**
   * Create a buffer with an owned evbuffer, which copies all data out of
   * the other buffer.
   */
  explicit OwnedEvBufferWrapper(evbuffer * other);

  /**
   * Create a buffer with an owned evbuffer, which copies dataLength many
   * bytes out of the other evbuffer.
   */
  explicit OwnedEvBufferWrapper(
      evbuffer * other, const size_t dataLength);
  ~OwnedEvBufferWrapper();
};

static_assert(
    std::numeric_limits<uint8_t>::digits == CHAR_BIT,
    "Bits per byte constant incorrect.");

template<typename value_T>
void EvBufferWrapper::write(const value_T & value) {
  static_assert(std::is_integral<value_T>::value, "Unsupported type");
  const size_t integerSize = sizeof(value_T);
  const size_t topByteIndex = integerSize - 1;
  std::array<uint8_t, sizeof(value_T)> result;

  for (size_t it = 0; it < integerSize; it++) {
    const size_t shiftPosition =
        (topByteIndex - it) * std::numeric_limits<uint8_t>::digits;
    result[it] = (uint8_t)(value >> shiftPosition);
  }

  this->add(result.data(), integerSize);
}

template<>
inline void EvBufferWrapper::write<std::vector<uint8_t>>(
    const std::vector<uint8_t> & value) {
  this->add(value.data(), value.size());
}

/* The length of the string will be sent in prefix as a 32-bit unsigned int. */
template<>
inline void
EvBufferWrapper::write<std::string>(std::string const & value) {
  this->write<uint32_t>((uint32_t)value.size());
  this->add(value.data(), value.size());
}

template<>
inline void
EvBufferWrapper::write<safrn::dbuid_t>(const dbuid_t & value) {
  this->add(value.data(), DBUID_LENGTH);
}

template<typename value_T>
void EvBufferWrapper::read(value_T & result) {
  static_assert(std::is_integral<value_T>::value, "Unsupported type");
  const size_t integerSize = sizeof(value_T);
  const size_t topByteIndex = integerSize - 1;
  std::array<uint8_t, sizeof(value_T)> bytes;
  this->remove(bytes.data(), integerSize);

  result = 0;

  for (size_t it = 0; it < integerSize; it++) {
    const size_t shiftPosition =
        (topByteIndex - it) * std::numeric_limits<uint8_t>::digits;
    result = result | (value_T)((value_T)bytes[it] << shiftPosition);
  }
}

template<>
inline void EvBufferWrapper::read<std::vector<uint8_t>>(
    std::vector<uint8_t> & result) {
  this->remove(result.data(), result.size());
}

template<>
inline void EvBufferWrapper::read<safrn::dbuid_t>(dbuid_t & result) {
  this->remove(result.data(), DBUID_LENGTH);
}

template<>
inline void EvBufferWrapper::read<std::string>(std::string & result) {
  uint32_t len32;
  this->read<uint32_t>(len32);
  result.resize((size_t)len32);

  this->remove(&result[0], (size_t)len32);
}
} // namespace safrn

#endif //SAFRN_EVBUFFER_WRAPPER_H_
