/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */
#include <gtest/gtest.h>

#include <EventWrapper/EvBufferWrapper.h>

#include <cstdlib>
#include <ctime>

using namespace safrn;

TEST(EvBufferWrapper, without_typenames) {
  srand((unsigned int)time(nullptr));

  const uint8_t bit8 = (uint8_t)rand();
  const uint16_t bit16 = (uint16_t)rand();
  const uint32_t bit32 = (uint32_t)rand();
  const uint64_t bit64 =
      ((uint64_t)rand()) | (((uint64_t)rand()) << 32);

  dbuid_t const dbuid = {(uint8_t)rand(),
                         (uint8_t)rand(),
                         (uint8_t)rand(),
                         (uint8_t)rand(),
                         (uint8_t)rand(),
                         (uint8_t)rand(),
                         (uint8_t)rand(),
                         (uint8_t)rand(),
                         (uint8_t)rand(),
                         (uint8_t)rand(),
                         (uint8_t)rand(),
                         (uint8_t)rand(),
                         (uint8_t)rand(),
                         (uint8_t)rand(),
                         (uint8_t)rand(),
                         (uint8_t)rand()};

  std::string str = "abcdefghijklmnopqrstuvwxyz1234567890";

  OwnedEvBufferWrapper out;

  out.write(bit8);
  out.write(bit16);
  out.write(bit32);
  out.write(bit64);
  out.write(dbuid);
  out.write(str);

  EXPECT_EQ(
      out.length(),
      sizeof(bit8) + sizeof(bit16) + sizeof(bit32) + sizeof(bit64) +
          sizeof(dbuid) + sizeof(uint32_t) + str.length());

  uint8_t in8 = 0;
  uint16_t in16 = 0;
  uint32_t in32 = 0;
  uint64_t in64 = 0;

  dbuid_t inDbuid;

  std::string inStr;

  out.read(in8);
  out.read(in16);
  out.read(in32);
  out.read(in64);
  out.read(inDbuid);
  out.read(inStr);

  EXPECT_EQ(in8, bit8);
  EXPECT_EQ(in16, bit16);
  EXPECT_EQ(in32, bit32);
  EXPECT_EQ(in64, bit64);
  EXPECT_EQ(inDbuid, dbuid);
  EXPECT_EQ(inStr, str);
}

TEST(EvBufferWrapper, with_typenames) {
  srand((unsigned int)time(nullptr));

  const uint8_t bit8 = (uint8_t)rand();
  const uint16_t bit16 = (uint16_t)rand();
  const uint32_t bit32 = (uint32_t)rand();
  const uint64_t bit64 =
      ((uint64_t)rand()) | (((uint64_t)rand()) << 32);

  dbuid_t const dbuid = {(uint8_t)rand(),
                         (uint8_t)rand(),
                         (uint8_t)rand(),
                         (uint8_t)rand(),
                         (uint8_t)rand(),
                         (uint8_t)rand(),
                         (uint8_t)rand(),
                         (uint8_t)rand(),
                         (uint8_t)rand(),
                         (uint8_t)rand(),
                         (uint8_t)rand(),
                         (uint8_t)rand(),
                         (uint8_t)rand(),
                         (uint8_t)rand(),
                         (uint8_t)rand(),
                         (uint8_t)rand()};

  std::string str = "abcdefghijklmnopqrstuvwxyz1234567890";

  OwnedEvBufferWrapper out;

  out.write<uint8_t>(bit8);
  out.write<uint16_t>(bit16);
  out.write<uint32_t>(bit32);
  out.write<uint64_t>(bit64);
  out.write<dbuid_t>(dbuid);
  out.write<std::string>(str);

  EXPECT_EQ(
      out.length(),
      sizeof(bit8) + sizeof(bit16) + sizeof(bit32) + sizeof(bit64) +
          sizeof(dbuid) + sizeof(uint32_t) + str.length());

  uint8_t in8 = 0;
  uint16_t in16 = 0;
  uint32_t in32 = 0;
  uint64_t in64 = 0;

  dbuid_t inDbuid;

  std::string inStr;

  out.read<uint8_t>(in8);
  out.read<uint16_t>(in16);
  out.read<uint32_t>(in32);
  out.read<uint64_t>(in64);
  out.read<dbuid_t>(inDbuid);
  out.read<std::string>(inStr);

  EXPECT_EQ(in8, bit8);
  EXPECT_EQ(in16, bit16);
  EXPECT_EQ(in32, bit32);
  EXPECT_EQ(in64, bit64);
  EXPECT_EQ(inDbuid, dbuid);
  EXPECT_EQ(inStr, str);
}
