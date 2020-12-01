/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* C and POSIX Headers */

/* C++ Headers */

/* 3rd Party Headers */

/* SAFRN Headers */
#include <dataowner/RowReduction.h>

namespace safrn {
namespace dataowner {

void bn_mod_row_reduce(
    std::vector<BIGNUM *> & b,
    std::vector<BIGNUM *> & M,
    const size_t n,
    BIGNUM const & modulus,
    sst::bn_ctx & ctx) {
  BIGNUM & ratio = ctx.get();
  BIGNUM & inv = ctx.get();
  BIGNUM & tmp = ctx.get();
  size_t i, j, k; // Row and column indexes.
  /* Row reduce the Matrix to be upper-triangular. */
  for (i = 0; i < n - 1;
       ++i) { // Indexes rows, r-1 b/c j starts at i+1.
    if (!ctx.is_zero(*M[n * i + i])) { // Don't divide by zero.
      for (j = i + 1; j < n;
           ++j) { // Indexes rows, the row to be mutated.
        ctx.mod_inverse(inv, *M[n * i + i], modulus);
        ctx.mod_mul(ratio, *M[n * j + i], inv, modulus); // 1/M[j][i].
        ctx.sub(ratio, modulus, ratio); // Same as -ratio.
        for (k = 0; k < n + 1; ++k) { // n+1 b/c M|b.
          /* row(j) = row(j) + (-ratio)*row(i). */
          if (k == n) {
            ctx.mod_mul(tmp, ratio, *b[i], modulus);
            ctx.mod_add(*b[j], *b[j], tmp, modulus);
          } else {
            ctx.mod_mul(tmp, ratio, *M[n * i + k], modulus);
            ctx.mod_add(*M[n * j + k], *M[n * j + k], tmp, modulus);
          }
        }
      }
    }
  }
  /* Back-solve to remove the upper-triangular terms off the diagonal. */
  for (i = 1; i < n; ++i) { // ROWS, i starts ahead of j.
    if (!ctx.is_zero(*M[n * i + i])) { // Don't divide by zero.
      for (j = 0; j < i; ++j) { // ROWS, the row to be mutated.
        if (!ctx.is_zero(
                *M[n * j +
                   i])) { // Only loop if there's something to remove from off the diagonal.
          ctx.mod_inverse(inv, *M[n * i + i], modulus);
          ctx.mod_mul(ratio, *M[n * j + i], inv, modulus); // 1/M[j][i].
          ctx.sub(ratio, modulus, ratio); // Same as -ratio.
          for (
              k = i; k < n + 1;
              ++k) { // Start k at i b/c the lower triangle is 0s. n+1 b/c M|b.
            /* row(j) = row(j) + (-ratio)*row(i). */
            if (k == n) {
              ctx.mod_mul(tmp, ratio, *b[i], modulus);
              ctx.mod_add(*b[j], *b[j], tmp, modulus);
            } else {
              ctx.mod_mul(tmp, ratio, *M[n * i + k], modulus);
              ctx.mod_add(*M[n * j + k], *M[n * j + k], tmp, modulus);
            }
          }
        }
      }
    }
  }
  /* Divide Each row by the pivot. */
  for (i = 0; i < n; ++i) { // i indexes rows.
    if (!ctx.is_zero(*M[n * i + i])) { // Don't divide by 0.
      ctx.mod_inverse(inv, *M[n * i + i], modulus);
      ctx.mod_mul(*M[n * i + i], *M[n * i + i], inv, modulus);
      ctx.mod_mul(*b[i], *b[i], inv, modulus);
    }
  }
}

} // namespace dataowner
} // namespace safrn
