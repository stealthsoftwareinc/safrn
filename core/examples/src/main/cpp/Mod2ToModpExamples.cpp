/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* platform-specific includes */

/* c/c++ standard includes */
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>

/* third-party library includes */

/* project-specific includes */

/* same module include */
#include "Mod2ToModpExamples.h"

bool extractBit(const uint8_t * byteBuffer, const size_t bitNumber) {
  const size_t byteNumber =
      bitNumber / std::numeric_limits<uint8_t>::digits;
  const size_t bitInByteNumber =
      bitNumber % std::numeric_limits<uint8_t>::digits;

  const auto bitmask =
      (uint8_t)((uint8_t)0x1 << (uint8_t)bitInByteNumber);

  /* NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic) */
  return (bool)(byteBuffer[byteNumber] & bitmask);
}

std::unique_ptr<uint8_t> generateRandomData(const size_t bytes) {
  std::unique_ptr<uint8_t> randomBytes(new uint8_t[bytes]());
  RAND_bytes(randomBytes.get(), (int)bytes);

  return randomBytes;
}

std::vector<uint8_t> getRandomNumberBitField(const size_t bits) {
  const auto numberRandomBytes = (size_t)ceil(
      (double)bits / (double)std::numeric_limits<uint8_t>::digits);
  std::unique_ptr<uint8_t> randomBytes =
      generateRandomData(numberRandomBytes);

  /* Allocate vector with explicit size. */
  std::vector<uint8_t> result((size_t)bits);
  for (size_t j = 0; j < bits; j++) {
    result[j] = ((uint8_t)extractBit(randomBytes.get(), j));
  }

  return result;
}

Bitset_t xor2BitFields(const Bitset_t & A, const Bitset_t & B) {
  if (A.size() != B.size()) {
    throw std::runtime_error("Mismatched bitfield sizes.");
  }

  /* Allocate vector with explicit size. */
  std::vector<uint8_t> result((size_t)A.size());
  for (size_t it = 0; it < A.size(); it++) {
    result[it] = (A[it] xor B[it]);
  }

  return result;
}

std::vector<Bitset_t> generateMod2Shares(
    const size_t bitsPerShare,
    const size_t partyCount,
    const Bitset_t & x) {
  if (partyCount < 2) {
    throw std::runtime_error("Incorrect party count.");
  }

  /* Allocate vector with explicit size. */
  std::vector<Bitset_t> result((size_t)partyCount);

  /* Iterate through the rest of the parties
   * and calculate the shares. */
  for (size_t it = 1; it < partyCount; it++) {
    result[it] = getRandomNumberBitField(bitsPerShare);
  }

  /* Set initial value to x. */
  result.front() = x;

  /* First party is the xor of all the other
   * parties random shares. */
  for (size_t it = 1; it < partyCount; it++) {
    result.front() = xor2BitFields(result.front(), result[it]);
  }

  return result;
}

ModPValue_t generateRandomModPValue(const ModPValue_t p) {
  const auto bitsNeededForP = log2((double)p);
  const auto bytesNeededForP = (size_t)(ceil(
      ceil(bitsNeededForP) /
      (double)std::numeric_limits<uint8_t>::digits));
  auto rawData = generateRandomData(bytesNeededForP);
  ModPValue_t result = 0;
  /* Copy random data into value. */
  memcpy(&result, rawData.get(), bytesNeededForP);

  /* Ensure random data does not exceed p. */
  result = (result % p);

  return result;
}

ZYSet_t generateZYSet(const size_t c) {
  ZYSet_t result;
  result.z = getRandomNumberBitField(c);

  /* Create y-value pairs. */
  result.ys.resize((size_t)c);
  for (size_t j = 0; j < c; j++) {
    if (result.z[j] == 0) {
      result.ys[j] = std::make_pair(
          0, (ModPValue_t)((ModPValue_t)0x1 << (ModPValue_t)j));
    } else {
      result.ys[j] = std::make_pair(
          (ModPValue_t)((ModPValue_t)0x1 << (ModPValue_t)j), 0);
    }
  }

  return result;
}

ModPValue_t calculateModPValueFromSumPlusX(
    const ModPValue_t p, const ModPValue_t sum, const ModPValue_t x) {
  const int64_t signedResult = ((int64_t)p - (int64_t)sum) + (int64_t)x;
  const int64_t signedResultMod = signedResult % (int64_t)p;
  return (ModPValue_t)(
      signedResultMod < 0 ? (signedResultMod + (int64_t)p) :
                            (signedResultMod));
}

YValuePair_t generateFirstPartyModPSharePair(
    const ModPValue_t p,
    const YValuePair_t & x,
    const size_t j,
    const std::vector<std::vector<YValuePair_t>> & ys) {
  YValuePair_t sum;

  /* Add all parties shares together
   * except the first one. */
  for (size_t partyIt = 1; partyIt < ys.size(); partyIt++) {
    sum.first += ys[partyIt][j].first;
    sum.second += ys[partyIt][j].second;
  }

  return std::make_pair(
      calculateModPValueFromSumPlusX(p, sum.first, x.first),
      calculateModPValueFromSumPlusX(p, sum.second, x.second));
}

ZsYs_t generateModPCorrelatedRandomness(
    const size_t partyCount, const ModPValue_t p, const size_t c) {
  if (partyCount < 2) {
    throw std::runtime_error("Incorrect party count.");
  }
  ZsYs_t modp_shares;

  /* Genreate mod-p x values. */
  const auto modp_x = generateZYSet(c);

  /* Generate mod-p share z values. */
  modp_shares.zs = generateMod2Shares(c, partyCount, modp_x.z);

  /* Allocate ys array. */
  modp_shares.ys.assign(
      (size_t)partyCount, std::vector<YValuePair_t>((size_t)c));

  /* First generate all mod-p shares for parties
     * except for the first one. */
  for (size_t partyIt = 1; partyIt < partyCount; partyIt++) {

    /* Generate mod-p share y values. */
    for (size_t j = 0; j < c; j++) {
      modp_shares.ys[partyIt][j] = std::make_pair(
          generateRandomModPValue(p), generateRandomModPValue(p));
    }
  }

  for (size_t j = 0; j < c; j++) {
    /* Next, generate mod-p share (for first party) from those values. */
    modp_shares.ys.front()[j] = generateFirstPartyModPSharePair(
        p, modp_x.ys[j], j, modp_shares.ys);
  }

  return modp_shares;
}

std::vector<ModPValue_t> CalculateModPForm(
    const std::vector<Bitset_t> & mod2_shares,
    const ZsYs_t & modp_shares,
    const size_t c,
    const size_t partyCount,
    const ModPValue_t p) {

  std::vector<ModPValue_t> selected_sums((size_t)partyCount);
  std::vector<Bitset_t> b_shares((size_t)partyCount);

  for (size_t partyIt = 0; partyIt < partyCount; partyIt++) {
    b_shares[partyIt] =
        xor2BitFields(mod2_shares[partyIt], modp_shares.zs[partyIt]);
  }

  for (size_t j = 0; j < c; j++) {

    uint8_t b_final = 0;

    for (size_t partyIt = 0; partyIt < partyCount; partyIt++) {
      b_final = b_final xor b_shares[partyIt][j];
    }

    /* Sum up "modp_shares". */
    for (size_t partyIt = 0; partyIt < partyCount; partyIt++) {

      /* Retrieve selected_y value and
       * accumulate it into selected_sums. */
      if (b_final == 0) {
        selected_sums[partyIt] += modp_shares.ys[partyIt][j].first;
      } else {
        selected_sums[partyIt] += modp_shares.ys[partyIt][j].second;
      }
    }
  }

  /* Make sure each selected sum
   * is a mod-p value. */
  for (auto & selected_sum : selected_sums) {
    selected_sum %= p;
  }

  return selected_sums;
}

void ConvertMod2ToModPExample() {
  constexpr size_t C = 4;
  constexpr size_t P = 19;
  constexpr size_t PARTY_COUNT = 2;

  /* Generate XOR/Mod-2 shares. */
  const auto mod2_x = getRandomNumberBitField(C);
  const auto mod2_shares = generateMod2Shares(C, PARTY_COUNT, mod2_x);

  /* Generate mod-p based correlated randomness. */
  const auto modp_shares =
      generateModPCorrelatedRandomness(PARTY_COUNT, P, C);

  /* Convert mod-2 to mod-p shares. */
  const auto selected_sums =
      CalculateModPForm(mod2_shares, modp_shares, C, PARTY_COUNT, P);
}
