/**
 * Copyright (C) 2016-2020 Stealth Software Technologies, Inc.
 *
 * Developed under IR&D
 */

#include "char_casting_utils.h"

#include "constants.h" // For slice
#include "string_utils.h"

#include <climits> // For CHAR_BIT
#include <vector>

/* Logging Configuration */
#include <ff/logging.h>

using namespace math_utils;
using namespace string_utils;
using namespace std;

// Char <-> Bool
bool CharVectorToBool(const vector<unsigned char> & buffer) {
  if (buffer.size() != 1) {
    log_fatal(
        "Cannot cast a buffer of size %s as a Bool.",
        Itoa((uint64_t)buffer.size()).c_str());
  }
  return (unsigned int)buffer[0] != 0;
}
vector<bool>
CharVectorToBoolVector(const vector<unsigned char> & buffer) {
  vector<bool> to_return;

  // Interpret each byte as 8 packed bits.
  for (uint64_t i = 0; i < buffer.size(); ++i) {
    const unsigned int current_byte = (unsigned int)buffer[i];
    for (int j = 0; j < CHAR_BIT; ++j) {
      to_return.push_back(current_byte & (1 << (CHAR_BIT - 1 - j)));
    }
  }
  return to_return;
}
bool BoolToCharVector(
    const bool input, vector<unsigned char> * buffer) {
  if (buffer == nullptr)
    log_fatal("Null input.");
  buffer->push_back(input ? 1 : 0);
  return true;
}
bool BoolVectorToCharVector(
    const vector<bool> & input, vector<unsigned char> * buffer) {
  if (buffer == nullptr)
    log_fatal("Null input.");
  // Pack bits together into bytes. Since input.size() may not be a factor of 8
  // (number of bits per byte), we'll need to pad the char vector with either
  // leading or trailing 0's; we choose to do leading 0's, as this won't affect
  // the value of the bit string that input represents; however, note that
  // information is lost here, as we when going from
  //   BoolVector -> CharVector -> BoolVector
  // the final bool vector will size divisible by 8, while the original may not,
  // so we can only recover the original up to ambiguity in the number of leading
  // zeros.
  const int num_partial_bits = input.size() % CHAR_BIT;
  const int num_full_bytes = input.size() / CHAR_BIT;
  if (num_partial_bits > 0) {
    unsigned int first_byte = 0;

    for (int i = 0; i < num_partial_bits; ++i) {
      if (input[i]) {
        first_byte |= (1 << (num_partial_bits - 1 - i));
      }
    }
    buffer->push_back((unsigned char)first_byte);
  }

  // Now fill in the remaining bytes.
  for (size_t i = 0; i < num_full_bytes; ++i) {
    unsigned int current_byte = 0;
    for (int j = 0; j < CHAR_BIT; ++j) {
      if (input[num_partial_bits + CHAR_BIT * i + j]) {
        current_byte |= (1 << (CHAR_BIT - 1 - j));
      }
    }
    buffer->push_back((unsigned char)current_byte);
  }
  return true;
}

// Char <-> int32_t
int32_t CharVectorToInt32(const vector<unsigned char> & buffer) {
  return CharVectorToValue<int32_t>(buffer);
}
vector<int32_t>
CharVectorToInt32Vector(const vector<unsigned char> & buffer) {
  return CharVectorToValueVector<int32_t>(buffer);
}
bool Int32ToCharVector(
    const int32_t input, vector<unsigned char> * buffer) {
  return ValueToCharVector<int32_t>(input, buffer);
}
bool Int32VectorToCharVector(
    const vector<int32_t> & input, vector<unsigned char> * buffer) {
  return ValueVectorToCharVector<int32_t>(input, buffer);
}

// Char <-> uint32_t
uint32_t CharVectorToUint32(const vector<unsigned char> & buffer) {
  return CharVectorToValue<uint32_t>(buffer);
}
vector<uint32_t>
CharVectorToUint32Vector(const vector<unsigned char> & buffer) {
  return CharVectorToValueVector<uint32_t>(buffer);
}
bool Uint32ToCharVector(
    const uint32_t input, vector<unsigned char> * buffer) {
  return ValueToCharVector<uint32_t>(input, buffer);
}
bool Uint32VectorToCharVector(
    const vector<uint32_t> & input, vector<unsigned char> * buffer) {
  return ValueVectorToCharVector<uint32_t>(input, buffer);
}

// Char <-> int64_t
int64_t CharVectorToInt64(const vector<unsigned char> & buffer) {
  return CharVectorToValue<int64_t>(buffer);
}
vector<int64_t>
CharVectorToInt64Vector(const vector<unsigned char> & buffer) {
  return CharVectorToValueVector<int64_t>(buffer);
}
bool Int64ToCharVector(
    const int64_t & input, vector<unsigned char> * buffer) {
  return ValueToCharVector<int64_t>(input, buffer);
}
bool Int64VectorToCharVector(
    const vector<int64_t> & input, vector<unsigned char> * buffer) {
  return ValueVectorToCharVector<int64_t>(input, buffer);
}

// Char <-> uint64_t
uint64_t CharVectorToUint64(const vector<unsigned char> & buffer) {
  return CharVectorToValue<uint64_t>(buffer);
}
vector<uint64_t>
CharVectorToUint64Vector(const vector<unsigned char> & buffer) {
  return CharVectorToValueVector<uint64_t>(buffer);
}
bool Uint64ToCharVector(
    const int64_t & input, vector<unsigned char> * buffer) {
  return ValueToCharVector<int64_t>(input, buffer);
}
bool Uint64VectorToCharVector(
    const vector<int64_t> & input, vector<unsigned char> * buffer) {
  return ValueVectorToCharVector<int64_t>(input, buffer);
}

// Char <-> slice
slice CharVectorToSlice(const vector<unsigned char> & buffer) {
  return CharVectorToValue<slice>(buffer);
}
vector<slice>
CharVectorToSliceVector(const vector<unsigned char> & buffer) {
  return CharVectorToValueVector<slice>(buffer);
}
bool SliceToCharVector(
    const slice & input, vector<unsigned char> * buffer) {
  return ValueToCharVector<slice>(input, buffer);
}
bool SliceVectorToCharVector(
    const vector<slice> & input, vector<unsigned char> * buffer) {
  return ValueVectorToCharVector<slice>(input, buffer);
}

// Char <-> SlicePair
pair<slice, slice> CharVectorToSlicePair(
    const uint64_t & from,
    const uint64_t & to,
    const vector<unsigned char> & buffer) {
  if (buffer.size() <= from)
    log_fatal("Start index exceeds buffer size.");
  if (to <= from || to != from + sizeof(slice) * 2) {
    log_fatal(
        "Cannot cast a buffer of size %s as two slices (wrong number "
        "of bytes).",
        Itoa((uint64_t)to - from).c_str());
  }
  return make_pair(
      CharVectorToValue<slice>(from, sizeof(slice), buffer),
      CharVectorToValue<slice>(from + sizeof(slice), to, buffer));
}
pair<slice, slice> CharVectorToSlicePair(
    const uint64_t & from, const vector<unsigned char> & buffer) {
  return CharVectorToSlicePair(from, buffer.size(), buffer);
}
pair<slice, slice>
CharVectorToSlicePair(const vector<unsigned char> & buffer) {
  return CharVectorToSlicePair(0, buffer.size(), buffer);
}
vector<pair<slice, slice>> CharVectorToSlicePairVector(
    const uint64_t & from,
    const uint64_t & to,
    const vector<unsigned char> & buffer) {
  const size_t slice_size = sizeof(slice);
  uint64_t num_elements = (to - from) / (2 * slice_size);

  // Make sure buffer is an integer multiple of sizeof(SlicePair).
  if (to <= from || to != from + num_elements * 2 * slice_size) {
    log_fatal(
        "Cannot cast a buffer of size %s as a vector of SlicePairs "
        "(wrong number of bytes).",
        Itoa((uint64_t)to - from).c_str());
  }

  vector<pair<slice, slice>> to_return;
  for (uint64_t i = 0; i < num_elements; ++i) {
    to_return.push_back(make_pair(
        CharVectorToValue<slice>(
            from + i * 2 * slice_size,
            from + (i * 2 + 1) * slice_size,
            buffer),
        CharVectorToValue<slice>(
            from + (i * 2 + 1) * slice_size,
            from + (i + 1) * 2 * slice_size,
            buffer)));
  }
  return to_return;
}
vector<pair<slice, slice>> CharVectorToSlicePairVector(
    const uint64_t & from, const vector<unsigned char> & buffer) {
  return CharVectorToSlicePairVector(from, buffer.size(), buffer);
}
vector<pair<slice, slice>>
CharVectorToSlicePairVector(const vector<unsigned char> & buffer) {
  return CharVectorToSlicePairVector(0, buffer.size(), buffer);
}
bool SlicePairToCharVector(
    const pair<slice, slice> & input, vector<unsigned char> * buffer) {
  return (
      SliceToCharVector(input.first, buffer) &&
      SliceToCharVector(input.second, buffer));
}
bool SlicePairVectorToCharVector(
    const vector<pair<slice, slice>> & input,
    vector<unsigned char> * buffer) {
  for (const pair<slice, slice> & slice_pair : input) {
    if (!SlicePairToCharVector(slice_pair, buffer))
      return false;
  }
  return true;
}

// Char <-> pair<SlicePair, SlicePair>
pair<pair<slice, slice>, pair<slice, slice>> CharVectorToPairSlicePair(
    const uint64_t & from,
    const uint64_t & to,
    const vector<unsigned char> & buffer) {
  const size_t slice_size = sizeof(slice);
  if (buffer.size() <= from)
    log_fatal("Start index exceeds buffer size.");
  if (to <= from || to != from + slice_size * 4) {
    log_fatal(
        "Cannot cast a buffer of size %s as four slices (wrong number "
        "of bytes).",
        Itoa((uint64_t)to - from).c_str());
  }
  return make_pair(
      make_pair(
          CharVectorToValue<slice>(from, from + slice_size, buffer),
          CharVectorToValue<slice>(
              from + slice_size, from + 2 * slice_size, buffer)),
      make_pair(
          CharVectorToValue<slice>(
              from + 2 * slice_size, from + 3 * slice_size, buffer),
          CharVectorToValue<slice>(from + 3 * slice_size, to, buffer)));
}
pair<pair<slice, slice>, pair<slice, slice>> CharVectorToPairSlicePair(
    const uint64_t & from, const vector<unsigned char> & buffer) {
  return CharVectorToPairSlicePair(from, buffer.size(), buffer);
}
pair<pair<slice, slice>, pair<slice, slice>>
CharVectorToPairSlicePair(const vector<unsigned char> & buffer) {
  return CharVectorToPairSlicePair(0, buffer.size(), buffer);
}
vector<pair<pair<slice, slice>, pair<slice, slice>>>
CharVectorToPairSlicePairVector(
    const uint64_t & from,
    const uint64_t & to,
    const vector<unsigned char> & buffer) {
  const size_t slice_size = sizeof(slice);
  uint64_t num_elements = (to - from) / (4 * slice_size);

  // Make sure buffer is an integer multiple of sizeof(PairSlicePair).
  if (to <= from || to != from + num_elements * 4 * slice_size) {
    log_fatal(
        "Cannot cast a buffer of size %s as a vector of PairSlicePairs "
        "(wrong number of bytes).",
        Itoa((uint64_t)to - from).c_str());
  }

  vector<PairSlicePair> to_return;
  for (uint64_t i = 0; i < num_elements; ++i) {
    to_return.push_back(make_pair(
        make_pair(
            CharVectorToValue<slice>(
                from + i * 4 * slice_size,
                from + (i * 4 + 1) * slice_size,
                buffer),
            CharVectorToValue<slice>(
                from + (i * 4 + 1) * slice_size,
                from + (i * 4 + 2) * slice_size,
                buffer)),
        make_pair(
            CharVectorToValue<slice>(
                from + (i * 4 + 2) * slice_size,
                from + (i * 4 + 3) * slice_size,
                buffer),
            CharVectorToValue<slice>(
                from + (i * 4 + 3) * slice_size,
                from + (i * 4 + 4) * slice_size,
                buffer))));
  }
  return to_return;
}
vector<pair<pair<slice, slice>, pair<slice, slice>>>
CharVectorToPairSlicePairVector(
    const uint64_t & from, const vector<unsigned char> & buffer) {
  return CharVectorToPairSlicePairVector(from, buffer.size(), buffer);
}
vector<pair<pair<slice, slice>, pair<slice, slice>>>
CharVectorToPairSlicePairVector(const vector<unsigned char> & buffer) {
  return CharVectorToPairSlicePairVector(0, buffer.size(), buffer);
}
bool PairSlicePairToCharVector(
    const pair<pair<slice, slice>, pair<slice, slice>> & input,
    vector<unsigned char> * buffer) {
  return (
      SlicePairToCharVector(input.first, buffer) &&
      SlicePairToCharVector(input.second, buffer));
}
bool PairSlicePairVectorToCharVector(
    const vector<pair<pair<slice, slice>, pair<slice, slice>>> & input,
    vector<unsigned char> * buffer) {
  for (const PairSlicePair & pair_slice_pair : input) {
    if (!PairSlicePairToCharVector(pair_slice_pair, buffer))
      return false;
  }
  return true;
}

// Char <-> double
double CharVectorToDouble(const vector<unsigned char> & buffer) {
  if (buffer.size() != sizeof(double)) {
    log_fatal(
        "Cannot cast a buffer of size %s as a double (wrong number of "
        "bytes).",
        Itoa((uint64_t)buffer.size()).c_str());
  }

  double to_return;
  for (size_t i = 0; i < sizeof(double); ++i) {
    *((unsigned char *)(&to_return) + sizeof(double) - i - 1) =
        buffer[i];
  }

  return to_return;
}
vector<double>
CharVectorToDoubleVector(const vector<unsigned char> & buffer) {
  uint64_t num_elements = buffer.size() / sizeof(double);

  // Make sure buffer is an integer multiple of sizeof(double).
  if (buffer.size() != num_elements * sizeof(double)) {
    log_fatal(
        "Cannot cast a buffer of size %s as a vector of doubles (wrong "
        "number of bytes).",
        Itoa((uint64_t)buffer.size()).c_str());
  }

  vector<double> to_return;
  for (uint64_t i = 0; i < num_elements; ++i) {
    to_return.push_back(double());
    double & current_value = to_return.back();
    for (size_t j = 0; j < sizeof(double); ++j) {
      *((unsigned char *)(&current_value) + sizeof(double) - j - 1) =
          buffer[sizeof(double) * i + j];
    }
  }

  return to_return;
}
bool DoubleToCharVector(
    const double & input, vector<unsigned char> * buffer) {
  if (buffer == nullptr)
    log_fatal("Null input.");
  char * wrong_endian = (char *)&input;

  for (size_t i = 0; i < sizeof(double); ++i) {
    buffer->push_back(wrong_endian[sizeof(double) - i - 1]);
  }

  return true;
}
bool DoubleVectorToCharVector(
    const vector<double> & input, vector<unsigned char> * buffer) {
  if (buffer == nullptr)
    log_fatal("Null input.");
  for (const double & value : input) {
    char * wrong_endian = (char *)&value;
    for (size_t i = 0; i < sizeof(double); ++i) {
      buffer->push_back(wrong_endian[sizeof(double) - i - 1]);
    }
  }

  return true;
}

// ===================================== Char ==================================
// NOTE: The remainder of this file is identical to the above, with everything in
// terms of 'char' instead of 'unsigned char' (ugh, why does C++ even allow both?!)

// Char <-> Bool
bool CharVectorToBool(const vector<char> & buffer) {
  if (buffer.size() != 1) {
    log_fatal(
        "Cannot cast a buffer of size %s as a Bool.",
        Itoa((uint64_t)buffer.size()).c_str());
  }
  return (unsigned int)buffer[0] != 0;
}
vector<bool> CharVectorToBoolVector(const vector<char> & buffer) {
  vector<bool> to_return;

  // Interpret each byte as 8 packed bits.
  for (uint64_t i = 0; i < buffer.size(); ++i) {
    const unsigned int current_byte = (unsigned int)buffer[i];
    for (int j = 0; j < CHAR_BIT; ++j) {
      to_return.push_back(current_byte & (1 << (CHAR_BIT - 1 - j)));
    }
  }
  return to_return;
}
bool BoolToCharVector(const bool input, vector<char> * buffer) {
  if (buffer == nullptr)
    log_fatal("Null input.");
  buffer->push_back(input ? 1 : 0);
  return true;
}
bool BoolVectorToCharVector(
    const vector<bool> & input, vector<char> * buffer) {
  if (buffer == nullptr)
    log_fatal("Null input.");
  // Pack bits together into bytes. Since input.size() may not be a factor of 8
  // (number of bits per byte), we'll need to pad the char vector with either
  // leading or trailing 0's; we choose to do leading 0's, as this won't affect
  // the value of the bit string that input represents; however, note that
  // information is lost here, as we when going from
  //   BoolVector -> CharVector -> BoolVector
  // the final bool vector will size divisible by 8, while the original may not,
  // so we can only recover the original up to ambiguity in the number of leading
  // zeros.
  const int num_partial_bits = input.size() % CHAR_BIT;
  const int num_full_bytes = input.size() / CHAR_BIT;
  if (num_partial_bits > 0) {
    unsigned int first_byte = 0;

    for (int i = 0; i < num_partial_bits; ++i) {
      if (input[i]) {
        first_byte |= (1 << (num_partial_bits - 1 - i));
      }
    }
    buffer->push_back((char)first_byte);
  }

  // Now fill in the remaining bytes.
  for (size_t i = 0; i < num_full_bytes; ++i) {
    unsigned int current_byte = 0;
    for (int j = 0; j < CHAR_BIT; ++j) {
      if (input[num_partial_bits + CHAR_BIT * i + j]) {
        current_byte |= (1 << (CHAR_BIT - 1 - j));
      }
    }
    buffer->push_back((char)current_byte);
  }
  return true;
}

// Char <-> int32_t
int32_t CharVectorToInt32(const vector<char> & buffer) {
  return CharVectorToValue<int32_t>(buffer);
}
vector<int32_t> CharVectorToInt32Vector(const vector<char> & buffer) {
  return CharVectorToValueVector<int32_t>(buffer);
}
bool Int32ToCharVector(const int32_t input, vector<char> * buffer) {
  return ValueToCharVector<int32_t>(input, buffer);
}
bool Int32VectorToCharVector(
    const vector<int32_t> & input, vector<char> * buffer) {
  return ValueVectorToCharVector<int32_t>(input, buffer);
}

// Char <-> uint32_t
uint32_t CharVectorToUint32(const vector<char> & buffer) {
  return CharVectorToValue<uint32_t>(buffer);
}
vector<uint32_t> CharVectorToUint32Vector(const vector<char> & buffer) {
  return CharVectorToValueVector<uint32_t>(buffer);
}
bool Uint32ToCharVector(const uint32_t input, vector<char> * buffer) {
  return ValueToCharVector<uint32_t>(input, buffer);
}
bool Uint32VectorToCharVector(
    const vector<uint32_t> & input, vector<char> * buffer) {
  return ValueVectorToCharVector<uint32_t>(input, buffer);
}

// Char <-> int64_t
int64_t CharVectorToInt64(const vector<char> & buffer) {
  return CharVectorToValue<int64_t>(buffer);
}
vector<int64_t> CharVectorToInt64Vector(const vector<char> & buffer) {
  return CharVectorToValueVector<int64_t>(buffer);
}
bool Int64ToCharVector(const int64_t & input, vector<char> * buffer) {
  return ValueToCharVector<int64_t>(input, buffer);
}
bool Int64VectorToCharVector(
    const vector<int64_t> & input, vector<char> * buffer) {
  return ValueVectorToCharVector<int64_t>(input, buffer);
}

// Char <-> uint64_t
uint64_t CharVectorToUint64(const vector<char> & buffer) {
  return CharVectorToValue<uint64_t>(buffer);
}
vector<uint64_t> CharVectorToUint64Vector(const vector<char> & buffer) {
  return CharVectorToValueVector<uint64_t>(buffer);
}
bool Uint64ToCharVector(const int64_t & input, vector<char> * buffer) {
  return ValueToCharVector<int64_t>(input, buffer);
}
bool Uint64VectorToCharVector(
    const vector<int64_t> & input, vector<char> * buffer) {
  return ValueVectorToCharVector<int64_t>(input, buffer);
}

// Char <-> slice
slice CharVectorToSlice(const vector<char> & buffer) {
  return CharVectorToValue<slice>(buffer);
}
vector<slice> CharVectorToSliceVector(const vector<char> & buffer) {
  return CharVectorToValueVector<slice>(buffer);
}
bool SliceToCharVector(const slice & input, vector<char> * buffer) {
  return ValueToCharVector<slice>(input, buffer);
}
bool SliceVectorToCharVector(
    const vector<slice> & input, vector<char> * buffer) {
  return ValueVectorToCharVector<slice>(input, buffer);
}

// Char <-> SlicePair
pair<slice, slice> CharVectorToSlicePair(
    const uint64_t & from,
    const uint64_t & to,
    const vector<char> & buffer) {
  if (buffer.size() <= from)
    log_fatal("Start index exceeds buffer size.");
  if (to <= from || to != from + sizeof(slice) * 2) {
    log_fatal(
        "Cannot cast a buffer of size %s as two slices (wrong number "
        "of bytes).",
        Itoa((uint64_t)to - from).c_str());
  }
  return make_pair(
      CharVectorToValue<slice>(from, sizeof(slice), buffer),
      CharVectorToValue<slice>(from + sizeof(slice), to, buffer));
}
pair<slice, slice> CharVectorToSlicePair(
    const uint64_t & from, const vector<char> & buffer) {
  return CharVectorToSlicePair(from, buffer.size(), buffer);
}
pair<slice, slice> CharVectorToSlicePair(const vector<char> & buffer) {
  return CharVectorToSlicePair(0, buffer.size(), buffer);
}
vector<pair<slice, slice>> CharVectorToSlicePairVector(
    const uint64_t & from,
    const uint64_t & to,
    const vector<char> & buffer) {
  if (buffer.size() <= from)
    log_fatal("Start index bigger than buffer.");
  vector<pair<slice, slice>> to_return;
  if (buffer.size() == from)
    return to_return;
  const size_t slice_size = sizeof(slice);
  uint64_t num_elements = (to - from) / (2 * slice_size);

  // Make sure buffer is an integer multiple of sizeof(SlicePair).
  if (to <= from || to != from + num_elements * 2 * slice_size) {
    log_fatal(
        "Cannot cast a buffer of size %s as a vector of SlicePairs "
        "(wrong number of bytes).",
        Itoa((uint64_t)to - from).c_str());
  }

  for (uint64_t i = 0; i < num_elements; ++i) {
    to_return.push_back(make_pair(
        CharVectorToValue<slice>(
            from + i * 2 * slice_size,
            from + (i * 2 + 1) * slice_size,
            buffer),
        CharVectorToValue<slice>(
            from + (i * 2 + 1) * slice_size,
            from + (i + 1) * 2 * slice_size,
            buffer)));
  }
  return to_return;
}
vector<pair<slice, slice>> CharVectorToSlicePairVector(
    const uint64_t & from, const vector<char> & buffer) {
  return CharVectorToSlicePairVector(from, buffer.size(), buffer);
}
vector<pair<slice, slice>>
CharVectorToSlicePairVector(const vector<char> & buffer) {
  return CharVectorToSlicePairVector(0, buffer.size(), buffer);
}
bool SlicePairToCharVector(
    const pair<slice, slice> & input, vector<char> * buffer) {
  return (
      SliceToCharVector(input.first, buffer) &&
      SliceToCharVector(input.second, buffer));
}
bool SlicePairVectorToCharVector(
    const vector<pair<slice, slice>> & input, vector<char> * buffer) {
  for (const pair<slice, slice> & slice_pair : input) {
    if (!SlicePairToCharVector(slice_pair, buffer))
      return false;
  }
  return true;
}

// Char <-> pair<SlicePair, SlicePair>
pair<pair<slice, slice>, pair<slice, slice>> CharVectorToPairSlicePair(
    const uint64_t & from,
    const uint64_t & to,
    const vector<char> & buffer) {
  const size_t slice_size = sizeof(slice);
  if (buffer.size() <= from)
    log_fatal("Start index exceeds buffer size.");
  if (to <= from || to != from + slice_size * 4) {
    log_fatal(
        "Cannot cast a buffer of size %s as four slices (wrong number "
        "of bytes).",
        Itoa((uint64_t)to - from).c_str());
  }
  return make_pair(
      make_pair(
          CharVectorToValue<slice>(from, from + slice_size, buffer),
          CharVectorToValue<slice>(
              from + slice_size, from + 2 * slice_size, buffer)),
      make_pair(
          CharVectorToValue<slice>(
              from + 2 * slice_size, from + 3 * slice_size, buffer),
          CharVectorToValue<slice>(from + 3 * slice_size, to, buffer)));
}
pair<pair<slice, slice>, pair<slice, slice>> CharVectorToPairSlicePair(
    const uint64_t & from, const vector<char> & buffer) {
  return CharVectorToPairSlicePair(from, buffer.size(), buffer);
}
pair<pair<slice, slice>, pair<slice, slice>>
CharVectorToPairSlicePair(const vector<char> & buffer) {
  return CharVectorToPairSlicePair(0, buffer.size(), buffer);
}
vector<pair<pair<slice, slice>, pair<slice, slice>>>
CharVectorToPairSlicePairVector(
    const uint64_t & from,
    const uint64_t & to,
    const vector<char> & buffer) {
  if (buffer.size() <= from)
    log_fatal("Start index bigger than buffer.");
  vector<PairSlicePair> to_return;
  if (buffer.size() == from)
    return to_return;
  const size_t slice_size = sizeof(slice);
  uint64_t num_elements = (to - from) / (4 * slice_size);

  // Make sure buffer is an integer multiple of sizeof(PairSlicePair).
  if (to <= from || to != from + num_elements * 4 * slice_size) {
    log_fatal(
        "Cannot cast a buffer of size %s as a vector of PairSlicePairs "
        "(wrong number of bytes).",
        Itoa((uint64_t)to - from).c_str());
  }

  for (uint64_t i = 0; i < num_elements; ++i) {
    to_return.push_back(make_pair(
        make_pair(
            CharVectorToValue<slice>(
                from + i * 4 * slice_size,
                from + (i * 4 + 1) * slice_size,
                buffer),
            CharVectorToValue<slice>(
                from + (i * 4 + 1) * slice_size,
                from + (i * 4 + 2) * slice_size,
                buffer)),
        make_pair(
            CharVectorToValue<slice>(
                from + (i * 4 + 2) * slice_size,
                from + (i * 4 + 3) * slice_size,
                buffer),
            CharVectorToValue<slice>(
                from + (i * 4 + 3) * slice_size,
                from + (i * 4 + 4) * slice_size,
                buffer))));
  }
  return to_return;
}
vector<pair<pair<slice, slice>, pair<slice, slice>>>
CharVectorToPairSlicePairVector(
    const uint64_t & from, const vector<char> & buffer) {
  return CharVectorToPairSlicePairVector(from, buffer.size(), buffer);
}
vector<pair<pair<slice, slice>, pair<slice, slice>>>
CharVectorToPairSlicePairVector(const vector<char> & buffer) {
  return CharVectorToPairSlicePairVector(0, buffer.size(), buffer);
}
bool PairSlicePairToCharVector(
    const pair<pair<slice, slice>, pair<slice, slice>> & input,
    vector<char> * buffer) {
  return (
      SlicePairToCharVector(input.first, buffer) &&
      SlicePairToCharVector(input.second, buffer));
}
bool PairSlicePairVectorToCharVector(
    const vector<pair<pair<slice, slice>, pair<slice, slice>>> & input,
    vector<char> * buffer) {
  for (const PairSlicePair & pair_slice_pair : input) {
    if (!PairSlicePairToCharVector(pair_slice_pair, buffer))
      return false;
  }
  return true;
}

// Char <-> double
double CharVectorToDouble(const vector<char> & buffer) {
  if (buffer.size() != sizeof(double)) {
    log_fatal(
        "Cannot cast a buffer of size %s as a double (wrong number of "
        "bytes).",
        Itoa((uint64_t)buffer.size()).c_str());
  }

  double to_return;
  for (size_t i = 0; i < sizeof(double); ++i) {
    *((unsigned char *)(&to_return) + sizeof(double) - i - 1) =
        (unsigned char)buffer[i];
  }

  return to_return;
}
vector<double> CharVectorToDoubleVector(const vector<char> & buffer) {
  uint64_t num_elements = buffer.size() / sizeof(double);

  // Make sure buffer is an integer multiple of sizeof(double).
  if (buffer.size() != num_elements * sizeof(double)) {
    log_fatal(
        "Cannot cast a buffer of size %s as a vector of doubles (wrong "
        "number of bytes).",
        Itoa((uint64_t)buffer.size()).c_str());
  }

  vector<double> to_return;
  for (uint64_t i = 0; i < num_elements; ++i) {
    to_return.push_back(double());
    double & current_value = to_return.back();
    for (size_t j = 0; j < sizeof(double); ++j) {
      *((unsigned char *)(&current_value) + sizeof(double) - j - 1) =
          (unsigned char)buffer[sizeof(double) * i + j];
    }
  }

  return to_return;
}
bool DoubleToCharVector(const double & input, vector<char> * buffer) {
  if (buffer == nullptr)
    log_fatal("Null input.");
  char * wrong_endian = (char *)&input;

  for (size_t i = 0; i < sizeof(double); ++i) {
    buffer->push_back(wrong_endian[sizeof(double) - i - 1]);
  }

  return true;
}
bool DoubleVectorToCharVector(
    const vector<double> & input, vector<char> * buffer) {
  if (buffer == nullptr)
    log_fatal("Null input.");
  for (const double & value : input) {
    char * wrong_endian = (char *)&value;
    for (size_t i = 0; i < sizeof(double); ++i) {
      buffer->push_back(wrong_endian[sizeof(double) - i - 1]);
    }
  }

  return true;
}
