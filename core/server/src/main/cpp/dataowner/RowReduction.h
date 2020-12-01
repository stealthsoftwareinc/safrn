/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* C and POSIX Headers */

/* C++ Headers */
#include <vector>

/* 3rd Party Headers */
#include <openssl/bn.h>
#include <openssl/rand.h>

/* SAFRN Headers */
#include <sst/bn_ctx.h> // BN_CTX wrapper.

#ifndef SAFRN_DATAOWNER_ROW_REDUCTION_H_
#define SAFRN_DATAOWNER_ROW_REDUCTION_H_

namespace safrn {
namespace dataowner {

void bn_mod_row_reduce(
    std::vector<BIGNUM *> & b,
    std::vector<BIGNUM *> & M,
    const size_t n,
    BIGNUM const & modulus,
    sst::bn_ctx & ctx);

} // namespace dataowner
} // namespace safrn

#endif //SAFRN_DATAOWNER_ROW_REDUCTION_H_
