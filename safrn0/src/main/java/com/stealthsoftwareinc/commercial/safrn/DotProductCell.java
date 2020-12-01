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
 * <code>com.stealthsoftwareinc.commercial.safrn.DotProductCell</code>
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

import com.stealthsoftwareinc.sst.LimitExceededStatus;
import com.stealthsoftwareinc.sst.MissingCaseStatus;
import java.io.File;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.lang.Integer;
import java.lang.Long;
import java.math.BigInteger;
import java.nio.MappedByteBuffer;
import java.nio.channels.FileChannel;

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

public final class DotProductCell
{

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

private final BigInteger[] abBig;

/**
 *
 * @since SAFRN&nbsp;X.X.X (XXXX-XX-XX)
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

private File abFile;

/**
 *
 * @since SAFRN&nbsp;X.X.X (XXXX-XX-XX)
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

private MappedByteBuffer abFileMap;

/**
 *
 * @since SAFRN&nbsp;X.X.X (XXXX-XX-XX)
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

private RandomAccessFile abFileStream;

/**
 *
 * @since SAFRN&nbsp;X.X.X (XXXX-XX-XX)
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

private final int[] abInt;

/**
 *
 * @since SAFRN&nbsp;X.X.X (XXXX-XX-XX)
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

private final int abLength;

/**
 *
 * @since SAFRN&nbsp;X.X.X (XXXX-XX-XX)
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

private final long[] abLong;

/**
 *
 * @since SAFRN&nbsp;X.X.X (XXXX-XX-XX)
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

private final int abMaxSize;

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

public BigInteger rz;

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

public DotProductCell(
  final int groupSizeBound,
  final BigInteger modulus,
  final boolean useFilesForLargeMemory
) throws
  IOException
{
  final BigInteger maxInt = BigInteger.valueOf(Integer.MAX_VALUE);
  final BigInteger maxLong = BigInteger.valueOf(Long.MAX_VALUE);
  final BigInteger abMaxValue = modulus.subtract(BigInteger.ONE);
  this.abLength = groupSizeBound;
  this.abMaxSize = abMaxValue.toByteArray().length;
  if (false) {
  } else if (useFilesForLargeMemory) {
    this.abBig = null;
    this.abFile = File.createTempFile("safrn", ".tmp");
    this.abFile.deleteOnExit();
    this.abFileStream = new RandomAccessFile(this.abFile, "rw");
    try {
      if (groupSizeBound > Integer.MAX_VALUE / this.abMaxSize) {
        throw (LimitExceededStatus)
          new LimitExceededStatus(
          ).initCause(null)
        ;
      }
      final int n = groupSizeBound * this.abMaxSize;
      this.abFileStream.setLength(n);
      final FileChannel c = this.abFileStream.getChannel();
      try {
        this.abFileMap = c.map(FileChannel.MapMode.READ_WRITE, 0, n);
        this.abFileMap.limit(this.abFileMap.capacity());
      } finally {
        try {
          c.close();
        } catch (final Exception e) {
        }
      }
    } catch (final Exception e1) {
      try {
        this.abFileStream.close();
      } catch (final Exception e2) {
      }
      try {
        this.abFile.delete();
      } catch (final Exception e2) {
      }
      throw e1;
    }
    this.abInt = null;
    this.abLong = null;
  } else if (modulus.compareTo(maxInt) <= 0) {
    this.abBig = null;
    this.abFile = null;
    this.abFileMap = null;
    this.abFileStream = null;
    this.abInt = new int[groupSizeBound];
    this.abLong = null;
  } else if (modulus.compareTo(maxLong) <= 0) {
    this.abBig = null;
    this.abFile = null;
    this.abFileMap = null;
    this.abFileStream = null;
    this.abInt = null;
    this.abLong = new long[groupSizeBound];
  } else {
    this.abBig = new BigInteger[groupSizeBound];
    for (int i = 0; i != groupSizeBound; ++i) {
      this.abBig[i] = BigInteger.ZERO;
    }
    this.abFile = null;
    this.abFileMap = null;
    this.abFileStream = null;
    this.abInt = null;
    this.abLong = null;
  }
  this.rz = BigInteger.ZERO;
}

/**
 *
 * @since SAFRN&nbsp;X.X.X (XXXX-XX-XX)
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

public final void close(
) {
  if (this.abFile != null) {
    this.abFileMap = null;
    if (this.abFileStream != null) {
      try {
        this.abFileStream.close();
      } catch (final Exception e) {
      }
      this.abFileStream = null;
    }
    try {
      this.abFile.delete();
    } catch (final Exception e) {
    }
    this.abFile = null;
  }
}

/**
 *
 * @since SAFRN&nbsp;X.X.X (XXXX-XX-XX)
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

public final BigInteger getAbBig(
  final int i
) {
  if (this.abBig != null) {
    return this.abBig[i];
  }
  if (this.abFile != null) {
    final byte[] b = new byte[this.abMaxSize];
    this.abFileMap.position(i * this.abMaxSize);
    this.abFileMap.get(b);
    return new BigInteger(b);
  }
  if (this.abInt != null) {
    return BigInteger.valueOf(this.abInt[i]);
  }
  if (this.abLong != null) {
    return BigInteger.valueOf(this.abLong[i]);
  }
  throw (MissingCaseStatus)
    new MissingCaseStatus(
    ).initCause(null)
  ;
}

/**
 *
 * @since SAFRN&nbsp;X.X.X (XXXX-XX-XX)
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

public final int getAbLength(
) {
  return this.abLength;
}

/**
 *
 * @since SAFRN&nbsp;X.X.X (XXXX-XX-XX)
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

public final void setAb(
  final int i,
  final BigInteger x
) {
  if (this.abBig != null) {
    this.abBig[i] = x;
    return;
  }
  if (this.abFile != null) {
    final byte[] b = x.toByteArray();
    this.abFileMap.position(i * this.abMaxSize);
    for (int j = 0; j != this.abMaxSize - b.length; ++j) {
      this.abFileMap.put((byte)0);
    }
    this.abFileMap.put(b);
    return;
  }
  if (this.abInt != null) {
    this.abInt[i] = x.intValue();
    return;
  }
  if (this.abLong != null) {
    this.abLong[i] = x.longValue();
    return;
  }
  throw (MissingCaseStatus)
    new MissingCaseStatus(
    ).initCause(null)
  ;
}

}
