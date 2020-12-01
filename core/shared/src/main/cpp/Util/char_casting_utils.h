/**
 * Copyright (C) 2016-2020 Stealth Software Technologies, Inc.
 *
 * Developed under IR&D
 *
 * Description:
 *   Utility functions for going back and forth between untyped bytes
 *   (i.e. [unsigned] characters) and typed objects.
 *
 * For any (numeric) type, we can imagine two ways of casting to a vector of bytes:
 *   1) As a byte-string (= Big Endian), so that viewing a byte as 8-bits,
 *      then reading the vector (of bytes) from left-to-right would give the
 *      binary string representation of the value. In other words, vector[0]
 *      will be the 8 most significant bits of value, and vector.back() will
 *      be the 8 least significant bits of the value.
 *   2) Consistent with how the system actually stores the value (as a
 *      contiguous block of bytes in memory)
 * (Actually, a third way would be to write the number as a string, so that
 * the vector of bytes is the character/string representation of the number;
 * this case is not covered in this file, use string_utils::Stoi for this.)
 * Note that (1) and (2) are equivalent iff the system stores values in
 * BIG-ENDIAN format. Doing (2) is trivial: just cast the value as a
 * char vector (see examples below); however, we provide functions for these for
 * clarity (so the caller can be assured of behavior):
 *   CharVectorToValue[Vector]: Does (2) for vector<unsigned char> -> value_t
 *   Value[Vector]ToCharVector: Does (2) for value_t -> vector<unsigned char>
 * If you desire interpretation (1), so you always interpret a vector of bytes
 * as the byte-string representation of a value (i.e. Big-Endian), use:
 *   ByteStringToValue[Vector]: Does (1) for vector<unsigned char> -> value_t
 *   Value[Vector]ToByteString: Does (1) for value_t -> vector<unsigned char>
 *
 * As mentioned above, the functions provided that do (2) are not necessary.
 * For example, suppose a function takes in a char array and its size (in bytes):
 *   Foo(char* input, size_t num_bytes_in_input),
 * and I have a slice "bar" that I want to give it. Then I can do:
 *   Foo((char*) &bar, sizeof(slice));
 * Or with a vector<slice> "bar":
 *   Foo((char*) bar.data(), sizeof(slice) * bar.size());
 */

#ifndef CHAR_CASTING_UTILS_H
#define CHAR_CASTING_UTILS_H

#include "constants.h" // For slice
#include "init_utils.h" // For GetEndianness
#include "string_utils.h"

#include <climits> // For CHAR_BIT
#include <cstring> // For memcpy
#include <vector>

/* Logging Config */
#include <ff/logging.h>

// =========== Usage (1) ByteString -> Value (see Discussion at top) ===========

// Interpret the characters between [from, to) (note the 'from' is inclusive,
// while 'to' is exclusive) as a numeric value of the appropriate type.
template<typename value_t>
inline value_t ByteStringToValue(
    const uint64_t & from,
    const uint64_t & to,
    const std::vector<unsigned char> & buffer) {
  if (buffer.size() <= from)
    log_fatal("Start index exceeds buffer size.");
  const size_t value_t_bytes = sizeof(value_t);
  if (to <= from || to != from + value_t_bytes) {
    log_fatal(
        "Cannot cast a buffer of size %s"
        " as the indicated numeric type of %s"
        " bytes (wrong number of bytes).",
        string_utils::Itoa((uint64_t)to - from).c_str(),
        string_utils::Itoa(value_t_bytes).c_str());
  }

  // Check if there are value_t_bytes bytes to use in buffer; otherwise, we
  // just use the bytes we have (and treat missing bytes as leading '0's).
  const size_t stop_byte =
      to <= buffer.size() ? value_t_bytes : buffer.size() - from;

  value_t to_return = 0;
  for (size_t i = 0; i < stop_byte; ++i) {
    // We need to cast buffer[] as 'value_t', as for numeric types that are
    // greater than 32 bits (e.g. int64_t), element buffer[i] will be cast
    // as an int (32 bits), and then when j >= 4, we'll shift past 32 bits,
    // and end up with zero.
    to_return +=
        ((value_t)buffer[i + from] << (stop_byte - 1 - i) * CHAR_BIT);
  }

  return to_return;
}
// Same as above, but only take in a start position (end position will be
// determined by sizeof(value_t)).
template<typename value_t>
inline value_t ByteStringToValue(
    const uint64_t & from, const std::vector<unsigned char> & buffer) {
  return ByteStringToValue<value_t>(
      from, from + sizeof(value_t), buffer);
}
// Same as above, but with from = 0.
template<typename value_t>
inline value_t
ByteStringToValue(const std::vector<unsigned char> & buffer) {
  return ByteStringToValue<value_t>(0, buffer);
}
// Same as above, for unsigned char array (instead of vector).
template<typename value_t>
inline value_t ByteStringToValue(
    const uint64_t & num_bytes, const unsigned char * buffer) {
  const size_t value_t_bytes = sizeof(value_t);
  if (num_bytes > value_t_bytes) {
    log_fatal(
        "Cannot cast a buffer of size %s"
        " as the indicated numeric type of %s"
        " bytes (wrong number of bytes).",
        string_utils::Itoa(num_bytes).c_str(),
        string_utils::Itoa(value_t_bytes).c_str());
  }

  value_t to_return = 0;
  for (size_t i = 0; i < num_bytes; ++i) {
    // We need to cast buffer[] as 'value_t', as for numeric types that are
    // greater than 32 bits (e.g. int64_t), element buffer[i] will be cast
    // as an int (32 bits), and then when j >= 4, we'll shift past 32 bits,
    // and end up with zero.
    to_return += ((value_t)buffer[i] << (num_bytes - 1 - i) * CHAR_BIT);
  }

  return to_return;
}

// Same as above, but interpret as a vector of values.
template<typename value_t>
inline std::vector<value_t> ByteStringToValueVector(
    const uint64_t & from,
    const uint64_t & to,
    const std::vector<unsigned char> & buffer) {
  const size_t value_t_bytes = sizeof(value_t);
  uint64_t num_elements = (to - from) / value_t_bytes;

  // Make sure buffer is an integer multiple of value_t_bytes.
  if (to <= from || to != from + num_elements * value_t_bytes) {
    log_fatal(
        "Cannot cast a buffer of size %s"
        " as a vector of the indicated numeric type (wrong number of "
        "bytes).",
        string_utils::Itoa((uint64_t)to - from).c_str());
  }

  std::vector<value_t> to_return(num_elements, value_t());
  for (uint64_t i = 0; i < num_elements; ++i) {
    value_t & current_value = to_return[i];
    current_value =
        0; // Should be default to value_t(), but just in case...
    for (size_t j = 0; j < value_t_bytes; ++j) {
      // We need to cast buffer[] as 'value_t', as for numeric types that are
      // greater than 32 bits (e.g. int64_t), element buffer[i] will be cast
      // as an int (32 bits), and then when j >= 4, we'll shift past 32 bits,
      // and end up with zero.
      current_value +=
          ((value_t)buffer[from + value_t_bytes * i + j]
           << (value_t_bytes - 1 - j) * CHAR_BIT);
    }
  }

  return to_return;
}
// Same as above, but only input 'from' (and read the remainder of buffer).
template<typename value_t>
inline std::vector<value_t> ByteStringToValueVector(
    const uint64_t & from, const std::vector<unsigned char> & buffer) {
  return ByteStringToValueVector<value_t>(from, buffer.size(), buffer);
}
// Same as above, but use the entire buffer.
template<typename value_t>
inline std::vector<value_t>
ByteStringToValueVector(const std::vector<unsigned char> & buffer) {
  return ByteStringToValueVector<value_t>(0, buffer.size(), buffer);
}
// ========= END Usage (1) ByteString -> Value (see Discussion at top) =========

// =========== Usage (1) Value -> ByteString (see Discussion at top) ===========
// Cast the numeric value as byte string.
// NOTE: Appends value to buffer (as opposed to clearing buffer first).
template<typename value_t>
inline bool ValueToByteString(
    const bool append,
    const value_t & input,
    std::vector<unsigned char> * buffer) {
  const size_t orig_size = buffer->size();
  const size_t value_t_bytes = sizeof(value_t);
  const size_t start_index = append ? orig_size : 0;
  if (append) {
    buffer->resize(orig_size + value_t_bytes);
  } else if (orig_size < value_t_bytes) {
    buffer->resize(value_t_bytes);
  }
  for (size_t i = 0; i < value_t_bytes; ++i) {
    (*buffer)[start_index + i] =
        (unsigned char)((input >> (value_t_bytes - 1 - i) * CHAR_BIT) & ~0);
  }

  return true;
}

// Same as above, but defaults to 'append = true'.
template<typename value_t>
inline bool ValueToByteString(
    const value_t & input, std::vector<unsigned char> * buffer) {
  return ValueToByteString<value_t>(true, input, buffer);
}

// Same as above with output an unsigned char* instead of a vector.
template<typename value_t>
inline bool
ValueToByteString(const value_t & input, unsigned char * buffer) {
  const size_t value_t_bytes = sizeof(value_t);
  for (size_t i = 0; i < value_t_bytes; ++i) {
    *(buffer + i) =
        (unsigned char)((input >> (value_t_bytes - 1 - i) * CHAR_BIT) & ~0);
  }
  return true;
}

// Cast the vector of numeric values as a vector<unsigned char>
template<typename value_t>
inline bool ValueVectorToByteString(
    const std::vector<value_t> & input,
    std::vector<unsigned char> * buffer) {
  const size_t value_t_bytes = sizeof(value_t);
  const size_t orig_buffer_size = buffer->size();
  const size_t num_new_bytes = input.size() * value_t_bytes;
  buffer->resize(orig_buffer_size + num_new_bytes);
  for (size_t i = 0; i < input.size(); ++i) {
    const value_t & value = input[i];
    for (size_t j = 0; j < value_t_bytes; ++j) {
      (*buffer)[orig_buffer_size + i * value_t_bytes + j] =
          (unsigned char)((value >> (value_t_bytes - 1 - j) * CHAR_BIT) & ~0);
    }
  }

  return true;
}
// ========= END Usage (1) Value -> ByteString (see Discussion at top) =========

// ============= Usage (2) Bytes -> Value (see Discussion at top) ==============
// Interpret the characters between [from, to) (note the 'from' is inclusive,
// while 'to' is exclusive) as a numeric value of the appropriate type.
template<typename value_t>
inline value_t CharVectorToValue(
    const uint64_t & from,
    const uint64_t & to,
    const std::vector<unsigned char> & buffer) {
  if (buffer.size() <= from)
    log_fatal("Start index exceeds buffer size.");
  const size_t value_t_bytes = sizeof(value_t);
  if (to <= from || to != from + value_t_bytes) {
    log_fatal(
        "Cannot cast a buffer of size %s"
        " as the indicated numeric type of %s"
        " bytes (wrong number of bytes).",
        string_utils::Itoa((uint64_t)to - from).c_str(),
        string_utils::Itoa(value_t_bytes).c_str());
  }

  // Check if there are value_t_bytes bytes to use in buffer, in which case
  // can just cast the input vector as the appropriate type.
  if (to <= buffer.size()) {
    value_t ret = 0;
    memcpy(
        (void *)&ret,
        (void const *)(buffer.data() + from),
        sizeof(value_t));
    return ret;
  }
  // Not enough bytes in input vector: treat missing bytes as leading '0's.
  const size_t stop_byte = buffer.size() - from;
  value_t to_return = 0;
  for (size_t i = 0; i < stop_byte; ++i) {
    // Cast buffer[] as 'value_t' so that we don't overflow when bit-shifting below.
    if (GetEndianness() == SYSTEM_ENDIAN::BIG) {
      to_return +=
          ((value_t)buffer[i + from] << (stop_byte - 1 - i) * CHAR_BIT);
    } else {
      to_return += ((value_t)buffer[i + from] << (CHAR_BIT * i));
    }
  }

  return to_return;
}
// Same as above, but only take in a start position (end position will be
// determined by sizeof(value_t)).
template<typename value_t>
inline value_t CharVectorToValue(
    const uint64_t & from, const std::vector<unsigned char> & buffer) {
  return CharVectorToValue<value_t>(
      from, from + sizeof(value_t), buffer);
}
// Same as above, but with from = 0.
template<typename value_t>
inline value_t
CharVectorToValue(const std::vector<unsigned char> & buffer) {
  return CharVectorToValue<value_t>(0, buffer);
}
// Same as above, for unsigned char array (instead of vector).
template<typename value_t>
inline value_t CharVectorToValue(
    const uint64_t & num_bytes, const unsigned char * buffer) {
  const size_t value_t_bytes = sizeof(value_t);
  if (num_bytes > value_t_bytes) {
    log_fatal(
        "Cannot cast a buffer of size %s"
        " as the indicated numeric type of %s"
        " bytes (wrong number of bytes).",
        string_utils::Itoa(num_bytes),
        string_utils::Itoa(value_t_bytes).c_str());
  }

  // Check if there are value_t_bytes bytes to use in buffer, in which case
  // can just cast the input vector as the appropriate type.
  if (num_bytes == value_t_bytes)
    return *((value_t *)buffer);
  // Not enough bytes in input vector: treat missing bytes as leading '0's.
  value_t to_return = 0;
  for (size_t i = 0; i < num_bytes; ++i) {
    // Cast buffer[] as 'value_t' so that we don't overflow when bit-shifting below.
    if (GetEndianness() == SYSTEM_ENDIAN::BIG) {
      to_return +=
          ((value_t)buffer[i] << (num_bytes - 1 - i) * CHAR_BIT);
    } else {
      to_return += ((value_t)buffer[i] << (CHAR_BIT * i));
    }
  }

  return to_return;
}

// Same as above, but interpret as a vector of values.
template<typename value_t>
inline std::vector<value_t> CharVectorToValueVector(
    const uint64_t & from,
    const uint64_t & to,
    const std::vector<unsigned char> & buffer) {
  const size_t value_t_bytes = sizeof(value_t);
  uint64_t num_elements = (to - from) / value_t_bytes;

  // Make sure buffer is an integer multiple of value_t_bytes.
  if (to <= from || to != from + num_elements * value_t_bytes) {
    log_fatal(
        "Cannot cast a buffer of size %s"
        " as a vector of the indicated numeric type (wrong number of "
        "bytes).",
        string_utils::Itoa((uint64_t)to - from).c_str());
  }

  std::vector<value_t> to_return(num_elements, value_t());
  for (uint64_t i = 0; i < num_elements; ++i) {
    value_t tmp = 0;
    memcpy(
        (void *)&tmp,
        (void const *)(buffer.data() + from),
        sizeof(value_t));
    to_return[i] = tmp + i;
  }

  return to_return;
}
// Same as above, but only input 'from' (and read the remainder of buffer).
template<typename value_t>
inline std::vector<value_t> CharVectorToValueVector(
    const uint64_t & from, const std::vector<unsigned char> & buffer) {
  return CharVectorToValueVector<value_t>(from, buffer.size(), buffer);
}
// Same as above, but use the entire buffer.
template<typename value_t>
inline std::vector<value_t>
CharVectorToValueVector(const std::vector<unsigned char> & buffer) {
  return CharVectorToValueVector<value_t>(0, buffer.size(), buffer);
}
// =========== END Usage (2) Bytes -> Value (see Discussion at top) ============

// ============= Usage (2) Value -> Bytes (see Discussion at top) ==============
// Cast the numeric value as a vector<unsigned char>.
// NOTE: Appends value to buffer (as opposed to clearing buffer first).
template<typename value_t>
inline bool ValueToCharVector(
    const bool append,
    const value_t & input,
    std::vector<unsigned char> * buffer) {
  const size_t orig_size = buffer->size();
  const size_t value_t_bytes = sizeof(value_t);
  const size_t start_index = append ? orig_size : 0;
  if (append) {
    buffer->resize(orig_size + value_t_bytes);
  } else if (orig_size < value_t_bytes) {
    buffer->resize(value_t_bytes);
  }
  memcpy(
      (void *)(buffer->data() + start_index),
      (void const *)&input,
      sizeof(value_t));

  return true;
}

// Same as above, but defaults to 'append = true'.
template<typename value_t>
inline bool ValueToCharVector(
    const value_t & input, std::vector<unsigned char> * buffer) {
  return ValueToCharVector<value_t>(true, input, buffer);
}

// Same as above with output an unsigned char* instead of a vector.  Caller should already have
// allocated sizeof(value_t) bytes to buffer
template<typename value_t>
inline bool
ValueToCharVector(const value_t & input, unsigned char * buffer) {
  const size_t value_t_bytes = sizeof(value_t);
  value_t * inserter = (value_t *)buffer;
  *inserter = input;
  return true;
}

// Cast the vector of numeric values as a vector<unsigned char>
template<typename value_t>
inline bool ValueVectorToCharVector(
    const std::vector<value_t> & input,
    std::vector<unsigned char> * buffer) {
  const size_t value_t_bytes = sizeof(value_t);
  const size_t orig_buffer_size = buffer->size();
  const size_t num_new_bytes = input.size() * value_t_bytes;
  buffer->resize(orig_buffer_size + num_new_bytes);
  for (size_t i = 0; i < input.size(); ++i) {
    const value_t & value_i = input[i];
    memcpy(
        (void *)(buffer->data() + (orig_buffer_size + i * value_t_bytes)),
        (void const *)&value_i,
        sizeof(value_t));
  }

  return true;
}
// =========== END Usage (2) Value -> Bytes (see Discussion at top) ============

// ============= The remaining functions all do Usage (2)  =====================
// ======= They match the functionality above, but for specified value_t =======

// Unsigned Char <-> Bool
// WARNING: If you're using a vector<bool>, you're probably doing something wrong.
// C++ handles these in a funky way. Consider using std::bitset instead, or
// by having the user cast the vector<bool> to a vector<unsigned char> in a
// way that is consistent with their needs.
extern bool CharVectorToBool(const std::vector<unsigned char> & buffer);
extern std::vector<bool>
CharVectorToBoolVector(const std::vector<unsigned char> & buffer);
extern bool
BoolToCharVector(const bool input, std::vector<unsigned char> * buffer);
extern bool BoolVectorToCharVector(
    const std::vector<bool> & input,
    std::vector<unsigned char> * buffer);

// Unsigned Char <-> int32_t
extern int32_t
CharVectorToInt32(const std::vector<unsigned char> & buffer);
extern std::vector<int32_t>
CharVectorToInt32Vector(const std::vector<unsigned char> & buffer);
extern bool Int32ToCharVector(
    const int32_t input, std::vector<unsigned char> * buffer);
extern bool Int32VectorToCharVector(
    const std::vector<int32_t> & input,
    std::vector<unsigned char> * buffer);

// Unsigned Char <-> uint32_t
extern uint32_t
CharVectorToUint32(const std::vector<unsigned char> & buffer);
extern std::vector<uint32_t>
CharVectorToUint32Vector(const std::vector<unsigned char> & buffer);
extern bool Uint32ToCharVector(
    const uint32_t input, std::vector<unsigned char> * buffer);
extern bool Uint32VectorToCharVector(
    const std::vector<uint32_t> & input,
    std::vector<unsigned char> * buffer);

// Unsigned Char <-> int64_t
extern int64_t
CharVectorToInt64(const std::vector<unsigned char> & buffer);
extern std::vector<int64_t>
CharVectorToInt64Vector(const std::vector<unsigned char> & buffer);
extern bool Int64ToCharVector(
    const int64_t & input, std::vector<unsigned char> * buffer);
extern bool Int64VectorToCharVector(
    const std::vector<int64_t> & input,
    std::vector<unsigned char> * buffer);

// Unsigned Char <-> uint64_t
extern uint64_t
CharVectorToUint64(const std::vector<unsigned char> & buffer);
extern std::vector<uint64_t>
CharVectorToUint64Vector(const std::vector<unsigned char> & buffer);
extern bool Uint64ToCharVector(
    const uint64_t & input, std::vector<unsigned char> * buffer);
extern bool Uint64VectorToCharVector(
    const std::vector<uint64_t> & input,
    std::vector<unsigned char> * buffer);

// Unsigned Char <-> slice
extern math_utils::slice
CharVectorToSlice(const std::vector<unsigned char> & buffer);
extern std::vector<math_utils::slice>
CharVectorToSliceVector(const std::vector<unsigned char> & buffer);
extern bool SliceToCharVector(
    const math_utils::slice & input,
    std::vector<unsigned char> * buffer);
extern bool SliceVectorToCharVector(
    const std::vector<math_utils::slice> & input,
    std::vector<unsigned char> * buffer);
// Unsigned Char <-> SlicePair
extern std::pair<math_utils::slice, math_utils::slice>
CharVectorToSlicePair(
    const uint64_t & from,
    const uint64_t & to,
    const std::vector<unsigned char> & buffer);
extern std::pair<math_utils::slice, math_utils::slice>
CharVectorToSlicePair(
    const uint64_t & from, const std::vector<unsigned char> & buffer);
extern std::pair<math_utils::slice, math_utils::slice>
CharVectorToSlicePair(const std::vector<unsigned char> & buffer);
extern std::vector<std::pair<math_utils::slice, math_utils::slice>>
CharVectorToSlicePairVector(
    const uint64_t & from,
    const uint64_t & to,
    const std::vector<unsigned char> & buffer);
extern std::vector<std::pair<math_utils::slice, math_utils::slice>>
CharVectorToSlicePairVector(
    const uint64_t & from, const std::vector<unsigned char> & buffer);
extern std::vector<std::pair<math_utils::slice, math_utils::slice>>
CharVectorToSlicePairVector(const std::vector<unsigned char> & buffer);
extern bool SlicePairToCharVector(
    const std::pair<math_utils::slice, math_utils::slice> & input,
    std::vector<unsigned char> * buffer);
extern bool SlicePairVectorToCharVector(
    const std::vector<std::pair<math_utils::slice, math_utils::slice>> &
        input,
    std::vector<unsigned char> * buffer);
// Unsigned Char <-> pair<SlicePair, SlicePair>
extern std::pair<
    std::pair<math_utils::slice, math_utils::slice>,
    std::pair<math_utils::slice, math_utils::slice>>
CharVectorToPairSlicePair(
    const uint64_t & from,
    const uint64_t & to,
    const std::vector<unsigned char> & buffer);
extern std::pair<
    std::pair<math_utils::slice, math_utils::slice>,
    std::pair<math_utils::slice, math_utils::slice>>
CharVectorToPairSlicePair(
    const uint64_t & from, const std::vector<unsigned char> & buffer);
extern std::pair<
    std::pair<math_utils::slice, math_utils::slice>,
    std::pair<math_utils::slice, math_utils::slice>>
CharVectorToPairSlicePair(const std::vector<unsigned char> & buffer);
extern std::vector<std::pair<
    std::pair<math_utils::slice, math_utils::slice>,
    std::pair<math_utils::slice, math_utils::slice>>>
CharVectorToPairSlicePairVector(
    const uint64_t & from,
    const uint64_t & to,
    const std::vector<unsigned char> & buffer);
extern std::vector<std::pair<
    std::pair<math_utils::slice, math_utils::slice>,
    std::pair<math_utils::slice, math_utils::slice>>>
CharVectorToPairSlicePairVector(
    const uint64_t & from, const std::vector<unsigned char> & buffer);
extern std::vector<std::pair<
    std::pair<math_utils::slice, math_utils::slice>,
    std::pair<math_utils::slice, math_utils::slice>>>
CharVectorToPairSlicePairVector(
    const std::vector<unsigned char> & buffer);
extern bool PairSlicePairToCharVector(
    const std::pair<
        std::pair<math_utils::slice, math_utils::slice>,
        std::pair<math_utils::slice, math_utils::slice>> & input,
    std::vector<unsigned char> * buffer);
extern bool PairSlicePairVectorToCharVector(
    const std::vector<std::pair<
        std::pair<math_utils::slice, math_utils::slice>,
        std::pair<math_utils::slice, math_utils::slice>>> & input,
    std::vector<unsigned char> * buffer);

// Unsigned Char <-> double
extern double
CharVectorToDouble(const std::vector<unsigned char> & buffer);
extern std::vector<double>
CharVectorToDoubleVector(const std::vector<unsigned char> & buffer);
extern bool DoubleToCharVector(
    const double & input, std::vector<unsigned char> * buffer);
extern bool DoubleVectorToCharVector(
    const std::vector<double> & input,
    std::vector<unsigned char> * buffer);

// ===================================== Char ==================================
// NOTE: The remainder of this file is identical to the above, with everything in
// terms of 'char' instead of 'unsigned char' (ugh, why does C++ even allow both?!)
// Note that one change that has to be made is to cast each std::vector<char> buffer
// as a vector of unsigned chars (it is assumed that any code path that
// creates such a buffer did so using unsigned chars, so we must do it this way).

// Interpret the characters between [from, to) (note the 'from' is inclusive,
// while 'to' is exclusive) as a numeric value of the appropriate type.
template<typename value_t>
inline value_t CharVectorToValue(
    const uint64_t & from,
    const uint64_t & to,
    const std::vector<char> & buffer) {
  value_t ret = 0;
  memcpy(
      (void *)&ret,
      (void const *)(buffer.data() + from),
      sizeof(value_t));
  return ret;
}
// Same as above, but only take in a start position (end position will be
// determined by sizeof(value_t)).
template<typename value_t>
inline value_t CharVectorToValue(
    const uint64_t & from, const std::vector<char> & buffer) {
  return CharVectorToValue<value_t>(
      from, from + sizeof(value_t), buffer);
}
// Same as above, but with from = 0.
template<typename value_t>
inline value_t CharVectorToValue(const std::vector<char> & buffer) {
  return CharVectorToValue<value_t>(0, buffer);
}

// Same as above, but interpret as a vector of values.
template<typename value_t>
inline std::vector<value_t> CharVectorToValueVector(
    const uint64_t & from,
    const uint64_t & to,
    const std::vector<char> & buffer) {
  const size_t value_t_bytes = sizeof(value_t);
  uint64_t num_elements = (to - from) / value_t_bytes;

  // Make sure buffer is an integer multiple of value_t_bytes.
  if (to <= from || (to - from) != num_elements * value_t_bytes) {
    log_fatal(
        "Cannot cast a buffer of size %s"
        " as a vector of the indicated numeric type (wrong number of "
        "bytes).",
        string_utils::Itoa((uint64_t)to - from).c_str());
  }

  std::vector<value_t> to_return(num_elements, value_t());
  for (uint64_t i = 0; i < num_elements; ++i) {
    value_t tmp = 0;
    memcpy(
        (void *)&tmp,
        (void const *)((buffer.data() + from) + (sizeof(value_t) / sizeof(char)) * i),
        sizeof(value_t));
    to_return[i] = tmp;
  }

  return to_return;
}
// Same as above, but only input 'from' (and read the remainder of buffer).
template<typename value_t>
inline std::vector<value_t> CharVectorToValueVector(
    const uint64_t & from, const std::vector<char> & buffer) {
  return CharVectorToValueVector<value_t>(from, buffer.size(), buffer);
}
// Same as above, but use the entire buffer.
template<typename value_t>
inline std::vector<value_t>
CharVectorToValueVector(const std::vector<char> & buffer) {
  return CharVectorToValueVector<value_t>(0, buffer.size(), buffer);
}

// Cast the numeric value as a byte-string (vector<char>).
// NOTE: The resulting byte-string is always oriented big-endian, so that if
// we interpret each byte in the byte string as 8-bits, then the byte string
// is the binary string representation of the value.
template<typename value_t>
inline bool
ValueToCharVector(const value_t & input, std::vector<char> * buffer) {
  const size_t value_t_bytes = sizeof(value_t);
  const size_t orig_buffer_size = buffer->size();
  buffer->resize(orig_buffer_size + value_t_bytes);
  memcpy(
      (void *)(buffer->data() + orig_buffer_size),
      (void const *)&input,
      sizeof(value_t));

  return true;
}

// Cast the vector of numeric values as a vector<char>
template<typename value_t>
inline bool ValueVectorToCharVector(
    const std::vector<value_t> & input, std::vector<char> * buffer) {
  const size_t value_t_bytes = sizeof(value_t);
  const size_t orig_buffer_size = buffer->size();
  const size_t num_new_bytes = input.size() * value_t_bytes;
  buffer->resize(orig_buffer_size + num_new_bytes);
  for (size_t i = 0; i < input.size(); ++i) {
    const value_t & value_i = input[i];
    memcpy(
        (void *)(buffer->data() + (orig_buffer_size + i * value_t_bytes)),
        (void const *)&value_i,
        sizeof(value_t));
  }

  return true;
}

// Char <-> Bool
// WARNING: If you're using a vector<bool>, you're probably doing something wrong.
// C++ handles these in a funky way. Consider using std::bitset instead, or
// by having the user cast the vector<bool> to a vector<char> in a
// way that is consistent with their needs.
extern bool CharVectorToBool(const std::vector<char> & buffer);
extern std::vector<bool>
CharVectorToBoolVector(const std::vector<char> & buffer);
extern bool
BoolToCharVector(const bool input, std::vector<char> * buffer);
extern bool BoolVectorToCharVector(
    const std::vector<bool> & input, std::vector<char> * buffer);

// Char <-> int32_t
extern int32_t CharVectorToInt32(const std::vector<char> & buffer);
extern std::vector<int32_t>
CharVectorToInt32Vector(const std::vector<char> & buffer);
extern bool
Int32ToCharVector(const int32_t input, std::vector<char> * buffer);
extern bool Int32VectorToCharVector(
    const std::vector<int32_t> & input, std::vector<char> * buffer);

// Char <-> uint32_t
extern uint32_t CharVectorToUint32(const std::vector<char> & buffer);
extern std::vector<uint32_t>
CharVectorToUint32Vector(const std::vector<char> & buffer);
extern bool
Uint32ToCharVector(const uint32_t input, std::vector<char> * buffer);
extern bool Uint32VectorToCharVector(
    const std::vector<uint32_t> & input, std::vector<char> * buffer);

// Char <-> int64_t
extern int64_t CharVectorToInt64(const std::vector<char> & buffer);
extern std::vector<int64_t>
CharVectorToInt64Vector(const std::vector<char> & buffer);
extern bool
Int64ToCharVector(const int64_t & input, std::vector<char> * buffer);
extern bool Int64VectorToCharVector(
    const std::vector<int64_t> & input, std::vector<char> * buffer);

// Char <-> uint64_t
extern uint64_t CharVectorToUint64(const std::vector<char> & buffer);
extern std::vector<uint64_t>
CharVectorToUint64Vector(const std::vector<char> & buffer);
extern bool
Uint64ToCharVector(const uint64_t & input, std::vector<char> * buffer);
extern bool Uint64VectorToCharVector(
    const std::vector<uint64_t> & input, std::vector<char> * buffer);

// Char <-> math_utils::slice
extern math_utils::slice
CharVectorToSlice(const std::vector<char> & buffer);
extern std::vector<math_utils::slice>
CharVectorToSliceVector(const std::vector<char> & buffer);
extern bool SliceToCharVector(
    const math_utils::slice & input, std::vector<char> * buffer);
extern bool SliceVectorToCharVector(
    const std::vector<math_utils::slice> & input,
    std::vector<char> * buffer);
// Char <-> SlicePair
extern std::pair<math_utils::slice, math_utils::slice>
CharVectorToSlicePair(
    const uint64_t & from,
    const uint64_t & to,
    const std::vector<char> & buffer);
extern std::pair<math_utils::slice, math_utils::slice>
CharVectorToSlicePair(
    const uint64_t & from, const std::vector<char> & buffer);
extern std::pair<math_utils::slice, math_utils::slice>
CharVectorToSlicePair(const std::vector<char> & buffer);
extern std::vector<std::pair<math_utils::slice, math_utils::slice>>
CharVectorToSlicePairVector(
    const uint64_t & from,
    const uint64_t & to,
    const std::vector<char> & buffer);
extern std::vector<std::pair<math_utils::slice, math_utils::slice>>
CharVectorToSlicePairVector(
    const uint64_t & from, const std::vector<char> & buffer);
extern std::vector<std::pair<math_utils::slice, math_utils::slice>>
CharVectorToSlicePairVector(const std::vector<char> & buffer);
extern bool SlicePairToCharVector(
    const std::pair<math_utils::slice, math_utils::slice> & input,
    std::vector<char> * buffer);
extern bool SlicePairVectorToCharVector(
    const std::vector<std::pair<math_utils::slice, math_utils::slice>> &
        input,
    std::vector<char> * buffer);
// Char <-> pair<SlicePair, SlicePair>
extern std::pair<
    std::pair<math_utils::slice, math_utils::slice>,
    std::pair<math_utils::slice, math_utils::slice>>
CharVectorToPairSlicePair(
    const uint64_t & from,
    const uint64_t & to,
    const std::vector<char> & buffer);
extern std::pair<
    std::pair<math_utils::slice, math_utils::slice>,
    std::pair<math_utils::slice, math_utils::slice>>
CharVectorToPairSlicePair(
    const uint64_t & from, const std::vector<char> & buffer);
extern std::pair<
    std::pair<math_utils::slice, math_utils::slice>,
    std::pair<math_utils::slice, math_utils::slice>>
CharVectorToPairSlicePair(const std::vector<char> & buffer);
extern std::vector<std::pair<
    std::pair<math_utils::slice, math_utils::slice>,
    std::pair<math_utils::slice, math_utils::slice>>>
CharVectorToPairSlicePairVector(
    const uint64_t & from,
    const uint64_t & to,
    const std::vector<char> & buffer);
extern std::vector<std::pair<
    std::pair<math_utils::slice, math_utils::slice>,
    std::pair<math_utils::slice, math_utils::slice>>>
CharVectorToPairSlicePairVector(
    const uint64_t & from, const std::vector<char> & buffer);
extern std::vector<std::pair<
    std::pair<math_utils::slice, math_utils::slice>,
    std::pair<math_utils::slice, math_utils::slice>>>
CharVectorToPairSlicePairVector(const std::vector<char> & buffer);
extern bool PairSlicePairToCharVector(
    const std::pair<
        std::pair<math_utils::slice, math_utils::slice>,
        std::pair<math_utils::slice, math_utils::slice>> & input,
    std::vector<char> * buffer);
extern bool PairSlicePairVectorToCharVector(
    const std::vector<std::pair<
        std::pair<math_utils::slice, math_utils::slice>,
        std::pair<math_utils::slice, math_utils::slice>>> & input,
    std::vector<char> * buffer);

// Char <-> double
extern double CharVectorToDouble(const std::vector<char> & buffer);
extern std::vector<double>
CharVectorToDoubleVector(const std::vector<char> & buffer);
extern bool
DoubleToCharVector(const double & input, std::vector<char> * buffer);
extern bool DoubleVectorToCharVector(
    const std::vector<double> & input, std::vector<char> * buffer);

#endif

#define LOG_UNCLUDE
#include <ff/logging.h>
