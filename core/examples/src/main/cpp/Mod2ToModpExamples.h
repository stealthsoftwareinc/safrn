/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 *
 * DESCRIPTION: Examples module design to show
 * a localized (non-distributed) version of
 * mod-2 to mod-p conversion.
 */

#ifndef INC_5A3005EFC8A940629BB230DD781C4A5B
#define INC_5A3005EFC8A940629BB230DD781C4A5B

/* platform-specific includes */

/* c/c++ standard includes */
#include <cstdint>
#include <limits>
#include <memory>
#include <utility>
#include <vector>

/* third-party library includes */
#include <openssl/rand.h>

/* project-specific includes */

/* ---- typedefs ---- */
using Bitset_t = std::vector<uint8_t>;
using ModPValue_t = uint64_t;
using YValuePair_t = std::pair<ModPValue_t, ModPValue_t>;

using ZYSet_t = struct {
  Bitset_t z;
  std::vector<YValuePair_t> ys;
};

using ZsYs_t = struct {
  std::vector<Bitset_t> zs;
  std::vector<std::vector<YValuePair_t>> ys;
};

/* ---- functions ---- */
bool extractBit(const uint8_t * byteBuffer, const size_t bitNumber);
std::unique_ptr<uint8_t> generateRandomData(const size_t bytes);
std::vector<uint8_t> getRandomNumberBitField(const size_t bits);
std::vector<uint8_t> xor2BitFields(
    const std::vector<uint8_t> & A, const std::vector<uint8_t> & B);
std::vector<Bitset_t> generateMod2Shares(
    const size_t bitsPerShare,
    const size_t partyCount,
    const Bitset_t & x);
ModPValue_t generateRandomModPValue(const ModPValue_t p);
ZYSet_t generateZYSet(const size_t c);
ModPValue_t calculateModPValueFromSumPlusX(
    const ModPValue_t p, const ModPValue_t sum, const ModPValue_t x);
YValuePair_t generateFirstPartyModPSharePair(
    const ModPValue_t p,
    const YValuePair_t & x,
    std::vector<YValuePair_t>::const_iterator begin,
    std::vector<YValuePair_t>::const_iterator end);
ZsYs_t generateModPCorrelatedRandomness(
    const size_t partyCount, const ModPValue_t p, const size_t c);
std::vector<ModPValue_t> CalculateModPForm(
    const std::vector<Bitset_t> & mod2_shares,
    const ZsYs_t & modp_shares,
    const size_t c,
    const size_t partyCount,
    const ModPValue_t p);
void ConvertMod2ToModPExample();

#endif
