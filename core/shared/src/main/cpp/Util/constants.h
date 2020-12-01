/**
 * Copyright (C) 2016-2020 Stealth Software Technologies, Inc.
 *
 * Developed under IR&D
 *
 * Description: Constants, plus a function for converting values to
 * slices.
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <cstdint> // For [u]int64_t
#include <utility> // For pair

namespace math_utils {

// Define 'slice' to be either uint32_t or uint64_t, depending on processor capability.
// This uses existence of defined macros to detect processor type; as such,
// it depends on checking the correct macros (and in particular, the logic below
// will be outdated when new processors are introduced, and hence is likely
// not exhaustive). The list of macros associated with various processors was
// taken from:
// http://nadeausoftware.com/articles/2012/02/
//      c_c_tip_how_detect_processor_type_using_compiler_predefined_macros
//   - Test processor: Itanium
#if defined(__ia64) || defined(__itanium__) || defined(_M_IA64)
typedef uint64_t slice;
//   - Test processor: PowerPC
#elif defined(__powerpc64__) || defined(__ppc64__) || defined(__PPC64__)
typedef uint64_t slice;
//   - Test processor: SPARC
#elif defined(__sparc64)
typedef uint64_t slice;
//   - Test processors: AMD and Intel x86
#elif defined(__x86_64__) || defined(_M_X64)
typedef uint64_t slice;
//   - Test Windows processors.
#elif defined(WINDOWS) || defined(__WIN32__) || defined(__WIN64__)
#if defined(_WIN64) || defined(__WIN64__)
// 64-bit programs run only on Win64.
typedef uint64_t slice;
#elif defined(_WIN32) || defined(__WIN32__)
// 32-bit programs run on both 32-bit and 64-bit Windows
// so must sniff
// TODO(PHB): Figure out how to do this via #if, as I'm not allowed to
// do ordinary if/else statement here.
/*
    #include <windows.h>
    BOOL is_64 = FALSE;
    if (IsWow64Process(GetCurrentProcess(), &is_64) && is_64) {
      typedef uint64_t slice;
    } else {
      typedef uint32_t slice;
    }
    */
typedef uint32_t slice;
#else
// Unable to detect processor type; to be safe, set slice to have 32 bits.
typedef uint32_t slice;
#endif
//   - Unable to detect processor type; to be safe, set slice to have 32 bits.
#else
typedef uint32_t slice;
#endif

typedef std::pair<slice, slice> SlicePair;
typedef std::pair<SlicePair, SlicePair> PairSlicePair;

extern const double Z_SCORE_FOR_TWO_TAILED_FIVE_PERCENT;
extern const double PI;
extern const double DELTA;
extern const double EPSILON;

extern const char NA_STRING[];
extern const char RHS_STRING[];

} // namespace math_utils

#endif
