/**
 * Copyright (C) 2017-2020 Stealth Software Technologies, Inc.
 *
 * Developed under IR&D
 *
 * Description: This file provides the Linux-specific implementation
 *  of the RandomBit(), RandomByte() and RandomBytes() functions in
 *  random_utils.h.
 */

#include "char_casting_utils.h"
#include "constants.h" // For slice
#include "random_utils.h"
#include "string_utils.h"

#include <cstring> // For memset.
#include <nettle/aes.h>
#include <nettle/ctr.h>
#include <pthread.h>
#include <vector>

/* Logging Configuration */
#include <ff/logging.h>

using namespace math_utils;
using namespace string_utils;
using namespace std;

namespace crypto {
namespace random_number {

static pthread_once_t kPthreadOnceInit = PTHREAD_ONCE_INIT;
static pthread_key_t kPthreadKey;

//if you change this you need to change the aes context used in PthreadKeyinfo
static const int kNumSeedBytes = 16;

struct PthreadKeyInfo {
  aes128_ctx ctx_;
  unsigned char ctr_[kNumSeedBytes];
};

static void OnceInit() {
  if (pthread_key_create(&kPthreadKey, free) != 0) {
    log_fatal("Unable to create pthread.");
  }
}

void RandomBytes(const uint64_t & num_bytes, unsigned char * buffer) {
  if (pthread_once(&kPthreadOnceInit, OnceInit) != 0) {
    log_fatal("Unable to initialize pthread.");
  }

  PthreadKeyInfo * p =
      (PthreadKeyInfo *)pthread_getspecific(kPthreadKey);

  if (p == nullptr) {
    p = (PthreadKeyInfo *)malloc(sizeof(*p));
    if (p == nullptr)
      log_fatal("Unable to allocate memory for pthread");
    if (pthread_setspecific(kPthreadKey, p) != 0) {
      log_fatal("Unable to set pthread key.");
    }

    // Generate a seed for Nettle from /dev/urandom
    FILE * f = fopen("/dev/urandom", "rb");
    if (f == nullptr || fread(p->ctr_, kNumSeedBytes, 1, f) != 1) {
      log_fatal("Unable to get random bytes from /dev/urandom");
    }
    fclose(f);

    nettle_aes128_set_encrypt_key(&p->ctx_, p->ctr_);
    memset(p->ctr_, 0, kNumSeedBytes);
  }

  memset(buffer, 0, num_bytes);
  uint64_t num_bytes_needed = num_bytes;
  while (num_bytes_needed > 0) {
    uint64_t n = num_bytes_needed <= UINT_MAX ?
        num_bytes_needed :
        UINT_MAX / kNumSeedBytes * kNumSeedBytes;

#if defined AWS_LINUX
    ctr_crypt(
        &p->ctx_,
        (nettle_crypt_func *)aes_encrypt,
        kNumSeedBytes,
        p->ctr_,
        n,
        buffer,
        buffer);
#else
    ctr_crypt(
        &p->ctx_,
        (nettle_cipher_func *)aes_encrypt,
        kNumSeedBytes,
        p->ctr_,
        n,
        buffer,
        buffer);
#endif
    buffer += n;
    num_bytes_needed -= n;
  }
}

} // namespace random_number
} // namespace crypto
