/**
 * Copyright (C) 2016-2020 Stealth Software Technologies, Inc.
 *
 * Developed under IR&D
 */

#include "random_utils.h"

#include "char_casting_utils.h"
#include "constants.h" // For slice
#include "string_utils.h"

#include <vector>

/* Logging Configuration */
#include <ff/logging.h>

using namespace math_utils;
using namespace string_utils;
using namespace std;

namespace crypto {
namespace random_number {

bool RandomBit() {
  return RandomByte() & 1;
}

unsigned char RandomByte() {
  vector<unsigned char> temp(1);
  RandomBytes(1, temp.data());
  return temp[0];
}

void RandomBytes(
    const uint64_t & num_bytes, std::vector<unsigned char> * buffer) {
  const uint64_t orig_size = buffer->size();
  buffer->resize(orig_size + num_bytes);
  RandomBytes(num_bytes, buffer->data() + orig_size);
}

slice RandomSlice() {
  vector<unsigned char> buffer;
  RandomBytes(sizeof(slice), &buffer);
  return CharVectorToSlice(buffer);
}

uint32_t Random32BitInt() {
  vector<unsigned char> buffer;
  RandomBytes((sizeof(uint32_t)), &buffer);
  return CharVectorToValue<uint32_t>(buffer);
}

uint64_t Random64BitInt() {
  vector<unsigned char> buffer;
  RandomBytes((sizeof(uint64_t)), &buffer);
  return CharVectorToValue<uint64_t>(buffer);
}

unsigned short RandomShortInt() {
  vector<unsigned char> buffer;
  RandomBytes((sizeof(unsigned short)), &buffer);
  return CharVectorToValue<unsigned short>(buffer);
}

uint32_t RandomInModulus(const uint32_t & modulus) {
  uint32_t to_return;
  if (!RandomInModulus(modulus, &to_return)) {
    return 0;
  }
  return to_return;
}

uint64_t RandomInModulus(const uint64_t & modulus) {
  uint64_t to_return;
  if (!RandomInModulus(modulus, &to_return)) {
    return 0;
  }
  return to_return;
}

bool RandomInModulus(const uint32_t & modulus, uint32_t * random) {
  if (modulus == 1) {
    *random = 0;
    return true;
  }

  // Get the number of bytes in modulus (to get a range for how big of a
  // number to generate).
  int bits = 1;
  uint64_t temp = 2;
  uint32_t mask = 1;
  while (temp < modulus && bits < 32) {
    bits++;
    temp *= 2;
    mask *= 2;
    mask |= 1;
  }

  const int bytes = bits / CHAR_BIT + (bits % CHAR_BIT > 0);

  vector<unsigned char> buffer(bytes);
  do {
    buffer.clear();
    RandomBytes(bytes, &buffer);

    // Cast randomly generated bytes as an uint32. We treat the leading bytes
    // of buffer as the least-significant, since 'bytes' may be less than
    // sizeof(uint32_t), in which case only the first elements of buffer
    // are valid/set (in RandomBytes() above); plus, since this is just
    // generating random values, there is no need to e.g. preserve endianess.
    *random = uint32_t(buffer[0]);
    for (int i = 1; i < bytes; ++i) {
      *random |= ((uint32_t)buffer[i]) << (CHAR_BIT * i);
    }

    // Adjust random as necessary:
    // If bits is not a multiple of 8 (CHAR_BIT), then there will be extra
    // random bits that were generated (This is a product of being forced to
    // use a RandomBytes() call instead of RandomBits()).
    // Clear those extra random bits, to increase the chance that the random
    // value generated is actually smaller than 'modulus'.
    *random &= mask;
    // Make sure the random number generated is within [0..modulus].
    // NOTE: Since we generated randomness in [0..2^bits - 1], in the worst case,
    // modulus = 2^b, in which case bits = b + 1, and then there is (roughly) a
    // 50% chance random > modulus.
  } while (*random >= modulus);

  return true;
}

bool RandomInModulus(const uint64_t & modulus, uint64_t * random) {
  if (modulus <= 0 || modulus > ULLONG_MAX)
    log_fatal("Bad input to RandomInModulus().");
  if (modulus <= 0)
    log_fatal("Bad input to RandomInModulus().");
  if (modulus == 1) {
    *random = 0;
    return true;
  }

  // Get the number of bytes in modulus (to get a range for how big of a
  // number to generate).
  int bits = 1;
  uint64_t temp = 2;
  uint64_t mask = 1;
  while (temp < modulus && bits < 64) {
    bits++;
    temp *= 2;
    mask *= 2;
    mask |= 1;
  }

  const int bytes = bits / CHAR_BIT + (bits % CHAR_BIT > 0);

  vector<unsigned char> buffer(bytes);
  do {
    buffer.clear();
    RandomBytes(bytes, &buffer);

    // Cast randomly generated bytes as an uint64. We treat the leading bytes
    // of buffer as the least-significant, since 'bytes' may be less than
    // sizeof(uint64_t), in which case only the first elements of buffer
    // are valid/set (in RandomBytes() above); plus, since this is just
    // generating random values, there is no need to e.g. preserve endianess.
    *random = uint64_t(buffer[0]);
    for (int i = 1; i < bytes; ++i) {
      *random |= ((uint64_t)buffer[i]) << (CHAR_BIT * i);
    }

    // Adjust random as necessary:
    // If bits is not a multiple of 8 (CHAR_BIT), then there will be extra
    // random bits that were generated (This is a product of being forced to
    // use a RandomBytes() call instead of RandomBits()).
    // Clear those extra random bits, to increase the chance that the random
    // value generated is actually smaller than 'modulus'.
    *random &= mask;
    // Make sure the random number generated is within [0..modulus].
    // NOTE: Since we generated randomness in [0..2^bits - 1], in the worst case,
    // modulus = 2^b, in which case bits = b + 1, and then there is (roughly) a
    // 50% chance random > modulus.
  } while (*random >= modulus);

  return true;
}

} // namespace random_number
} // namespace crypto
