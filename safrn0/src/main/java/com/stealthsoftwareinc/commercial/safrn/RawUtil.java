/*
 * This file is from the SAFRN package.
 *
 * The following copyright notice is generally applicable:
 *
 *      Copyright (C)
 *         Stealth Software Technologies Commercial, Inc.
 *
 * The full copyright information depends on the distribution
 * of the package. For more information, see the COPYING file.
 * However, depending on the context in which you are viewing
 * this file, the COPYING file may not be available.
 */

/*!
 * @file
 *
 * Defines the
 * <code>com.stealthsoftwareinc.commercial.safrn.RawUtil</code>
 * Java class.
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

package com.stealthsoftwareinc.commercial.safrn;

/* begin_imports */

import com.stealthsoftwareinc.sst.ParseFailureStatus;
import io.netty.buffer.ByteBuf;
import io.netty.buffer.ByteBufUtil;
import java.lang.String;
import java.math.BigInteger;
import java.nio.charset.Charset;
import java.nio.charset.StandardCharsets;
import java.util.Random;

/* end_imports */

/**
 *
 * @since SAFRN&nbsp;0.1.0 (2017-11-08)
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

public final class RawUtil
{

/**
 * Prevents objects of this class from being constructed.
 *
 * @throws java.lang.Throwable
 * (or a subclass thereof)
 * if and only if the operation failed
 *
 * @accesses
 * nothing
 *
 * @modifies
 * nothing
 *
 * @wellbehaved
 * always
 *
 * @since SAFRN&nbsp;0.1.0 (2017-11-08)
 *
 * @inheritancenotes
 * none
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

private RawUtil(
) {
}

/**
 *
 * @since SAFRN&nbsp;0.1.0 (2017-11-08)
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

public static final BigInteger getRandom(
  final BigInteger modulus,
  final Random random
) {
  BigInteger x;
  do {
    x = new BigInteger(modulus.bitLength(), random);
  } while (x.compareTo(modulus) >= 0);
  return x;
}

/**
 *
 * @since SAFRN&nbsp;0.1.0 (2017-11-08)
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

public static final BigInteger readBigInteger(
  final ByteBuf in
) {
  if (!in.isReadable(4)) {
    return null;
  }
  in.markReaderIndex();
  final int n = in.readInt();
  if (n < 0) {
    throw (ParseFailureStatus)
      new ParseFailureStatus(
      ).initCause(null)
    ;
  }
  if (!in.isReadable(n)) {
    in.resetReaderIndex();
    return null;
  }
  final byte[] b = new byte[n];
  in.readBytes(b);
  return new BigInteger(b);
}

/**
 *
 * @since SAFRN&nbsp;0.1.0 (2017-11-08)
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

public static final String readString(
  final ByteBuf in
) {
  if (!in.isReadable(4)) {
    return null;
  }
  in.markReaderIndex();
  final int n = in.readInt();
  if (n < 0) {
    throw (ParseFailureStatus)
      new ParseFailureStatus(
      ).initCause(null)
    ;
  }
  if (!in.isReadable(n)) {
    in.resetReaderIndex();
    return null;
  }
  final Charset charset = StandardCharsets.UTF_8;
  final String s = in.toString(in.readerIndex(), n, charset);
  in.skipBytes(n);
  return s;
}

/**
 *
 * @since SAFRN&nbsp;0.1.0 (2017-11-08)
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

public static final void writeBigInteger(
  final ByteBuf out,
  final BigInteger n
) {
  final byte[] b = n.toByteArray();
  out.writeInt(b.length);
  out.writeBytes(b);
}

/**
 *
 * @since SAFRN&nbsp;0.1.0 (2017-11-08)
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

public static final void writeString(
  final ByteBuf out,
  final CharSequence s
) {
  final int i = out.writerIndex();
  out.writeZero(4);
  ByteBufUtil.writeUtf8(out, s);
  final int j = out.writerIndex();
  out.writerIndex(i);
  out.writeInt(j - i - 4);
  out.writerIndex(j);
}

}
