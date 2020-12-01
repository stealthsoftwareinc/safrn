/**
 * Copyright (C) 2016-2020 Stealth Software Technologies, Inc.
 *
 * Developed under IR&D
 */

#include "constants.h" // For slice

#include <vector>

#ifndef RANDOM_UTILS_H
#define RANDOM_UTILS_H

namespace crypto {
namespace random_number {

// ======================== RandomBit(s), RandomByte(s) ========================
// NOTE:
// The only really original function is RandomBytes(uint64_t, unsigned char*),
// the others just use that in the appropriate way.
// The implementation of the above RandomBytes() function is OS-dependent.
// As such, the implementations exist in [windows | linux]_random_utils.cpp (as
// opposed to random_utils.cpp, which contains system-agnostic implementations).

// Returns a random bit (true for '1' or false for '0').
// NOTE: Wasteful. This will actually call RandomBytes() to get a random byte,
// and then use one of the bits of that random byte.
// Instead of using this API, if you need to generate multiple (say 'N') random
// bits, consider calling RandomBytes() (N / CHAR_BIT) times, and then using
// the bits of those bytes; see oblivious_transfer_utils.cpp for an example.
extern bool RandomBit();

// Returns a random byte (as an unsigned char).
extern unsigned char RandomByte();

// Puts 'num_bytes' into buffer.
// NOTES:
//   1) buffer should have 'num_bytes' already allocated (e.g. if calling this
//      from a vector<unsigned char> foo by using foo.data(), then foo should
//      have already been resized with room for num_bytes (although, in this
//      case, probably should've just used API below and simply pass in &foo,
//      in which case and user shoud *not* resize foo first).
//   2) The implementation of RandomBytes() is OS-specific (and hence it is *not*
//      in random_utils.cpp, but rather in [windows | linux]_random_utils.cpp).
//      The implementation is both thread-safe (so multiple threads can call
//      RandomBytes() at the same time, although may have to wait) as well
//      as cryptographically sound. Namely:
//        a) Windows: Uses CryptGenRandom()
//        b) Linux: Uses ctr_crypt() (via Nettle's aes_encrypt())
extern void
RandomBytes(const uint64_t & num_bytes, unsigned char * buffer);
// Same as above, with different API (array instead of vector).
// Appends 'num_bytes' random bytes to the end of 'buffer'.
extern void RandomBytes(
    const uint64_t & num_bytes, std::vector<unsigned char> * buffer);
// ====================== END RandomBit(s), RandomByte(s) ======================

// Returns a random slice.
extern math_utils::slice RandomSlice();
// Returns a random 32-bit number.
extern uint32_t Random32BitInt();
// Same as above, for 64-bit (unsigned) value.
extern uint64_t Random64BitInt();
// Same as above, for 16-bit (unsigned) value.
extern unsigned short RandomShortInt();

// Returns a random integer in Z_n (n = 'modulus'); i.e. a random value in [0..n].
extern bool
RandomInModulus(const uint32_t & modulus, uint32_t * random);
extern bool
RandomInModulus(const uint64_t & modulus, uint64_t * random);
// Same as above, with different API (returns value instead of bool).
extern uint32_t RandomInModulus(const uint32_t & modulus);
extern uint64_t RandomInModulus(const uint64_t & modulus);
// DEPRECATED (to avoid circular dependency). Use instead LargeInt::RandomInModulus().
//extern LargeInt RandomInModulus(const LargeInt& modulus);

} // namespace random_number
} // namespace crypto

#endif
