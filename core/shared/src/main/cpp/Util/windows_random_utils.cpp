/**
 * Copyright (C) 2017-2020 Stealth Software Technologies, Inc.
 *
 * Developed under IR&D
 *
 * Description: This file provides the Windows-specific implementation
 * of the RandomBit(), RandomByte() and RandomBytes() functions in
 * random_utils.h.
 */

#include "random_utils.h"

#include "char_casting_utils.h"
#include "constants.h" // For slice
#include "string_utils.h"

#include <vector>

// clang-format off
#include <windows.h>
#include <wincrypt.h>
// clang-format on

/* Logging Configuration */
#include <ff/logging.h>

using namespace math_utils;
using namespace string_utils;
using namespace std;

namespace crypto {
namespace random_number {

namespace {
// When retrieving randomness via call to Windows' CryptGenRandom(), we
// generate more randomness than is asked for, since many protocols will
// need many random numbers, and minimizing the number of calls to
// CryptGenRandom() will speed things up. The following is the maximum
// number of bytes we allow from CryptGenRandom(); and every call to
// CryptGenRandom() will obtain this many random bytes; unused randomness
// will be used in subsequent calls.
static const uint64_t kRandomByteBufferSize =
    1048576; // 2^20 bytes = 1Mb.

} // namespace

void RandomBytes(const uint64_t & num_bytes, unsigned char * buffer) {
  // Thread-protect 'random_bytes', in case multiple threads want to access
  // it at same time.
  static HANDLE mutex_handle = CreateMutex(NULL, FALSE, NULL);
  WaitForSingleObject(mutex_handle, INFINITE);

  static unsigned char random_bytes[kRandomByteBufferSize];
  // First time through, set number of used bytes equal to kRandomByteBufferSize,
  // so that we necessary call CryptGenRandom().
  static size_t used_random_bytes = kRandomByteBufferSize;

  // We will start by inserting at the beginning of buffer; in the loop below,
  // we may fill buffer in chunks; so keep track of where we're inserting.
  size_t buffer_insertion_index = 0;
  size_t num_bytes_needed = num_bytes;

  // Check if the number of random bytes needed can be obtained from unused
  // random bytes in 'random_bytes.'
  while (num_bytes_needed > kRandomByteBufferSize - used_random_bytes) {
    // Not enough existing randomness, so we'll need to generate more.
    // First, use up all the remaining randomness.
    const size_t num_unused_random_bytes =
        kRandomByteBufferSize - used_random_bytes;
    memcpy(
        buffer + buffer_insertion_index,
        random_bytes + used_random_bytes,
        kRandomByteBufferSize - used_random_bytes);
    // Update buffer, indicating num_unused_random_bytes have already been done.
    buffer_insertion_index += num_unused_random_bytes;
    num_bytes_needed -= num_unused_random_bytes;

    // Acquire more random bytes.
    HCRYPTPROV h;
    if (!CryptAcquireContext(
            &h, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
      log_fatal("CryptAcquireContext() failed.");
    if (!CryptGenRandom(h, kRandomByteBufferSize, random_bytes))
      log_fatal("CryptGenRandom() failed.");
    if (!CryptReleaseContext(h, 0))
      log_fatal("CryptReleaseContext() failed.");

    // Indicate none of the newly retrieved random bytes have been used.
    used_random_bytes = 0;
  }

  // Use unused random bytes in 'random_bytes' to fill buffer.
  memcpy(
      buffer + buffer_insertion_index,
      random_bytes + used_random_bytes,
      num_bytes_needed);

  // Update the number of unused bytes remaining in 'random_bytes'.
  used_random_bytes += num_bytes_needed;
  ReleaseMutex(mutex_handle);
}

} // namespace random_number
} // namespace crypto
