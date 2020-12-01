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
 * <code>com.stealthsoftwareinc.commercial.safrn.BlobOtCell</code>
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

import com.stealthsoftwareinc.commercial.safrn.BuildConfig;
import com.stealthsoftwareinc.sst.InvalidArgumentStatus;
import com.stealthsoftwareinc.sst.InvalidOperationStatus;
import com.stealthsoftwareinc.sst.LimitExceededStatus;
import com.stealthsoftwareinc.sst.NegativeArrayIndexStatus;
import com.stealthsoftwareinc.sst.NegativeArraySizeStatus;
import com.stealthsoftwareinc.sst.NullPointerStatus;
import com.stealthsoftwareinc.sst.OversizeArrayIndexStatus;
import com.stealthsoftwareinc.sst.Rand;
import com.stealthsoftwareinc.sst.ZeroArraySizeStatus;
import java.io.File;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.lang.Integer;
import java.math.BigInteger;
import java.nio.ByteBuffer;
import java.nio.channels.FileChannel;
import java.util.Random;

/* end_imports */

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

public class BlobOtCell
{

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

public static final int DEALER = 0;

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

public static final int RECEIVER = 1;

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

public static final int SENDER = 2;

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

protected ByteBuffer[] b;

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

private File bFile;

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

private RandomAccessFile bFileStream;

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

protected final int blobSize;

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

protected boolean isClosed;

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

protected final long otCount;

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

protected final int party;

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

protected ByteBuffer[] s;

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

private File sFile;

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

private RandomAccessFile sFileStream;

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

protected ByteBuffer[] u;

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

private File uFile;

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

private RandomAccessFile uFileStream;

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

public BlobOtCell(
  final int party,
  final long otCount,
  final int blobSize,
  final boolean useFiles,
  final File filesDirectory
) throws
  IOException
{
  if (BuildConfig.ENABLE_INVALID_ARGUMENT_CHECKS) {
    if (
      party != BlobOtCell.DEALER &&
      party != BlobOtCell.RECEIVER &&
      party != BlobOtCell.SENDER
    ) {
      throw (InvalidArgumentStatus)
        new InvalidArgumentStatus(
          "party != BlobOtCell.DEALER && " +
          "party != BlobOtCell.RECEIVER && " +
          "party != BlobOtCell.SENDER"
        ).initCause(null)
      ;
    }
    if (otCount < 0) {
      throw (NegativeArraySizeStatus)
        new NegativeArraySizeStatus(
          "otCount < 0"
        ).initCause(null)
      ;
    }
    if (blobSize < 0) {
      throw (NegativeArraySizeStatus)
        new NegativeArraySizeStatus(
          "blobSize < 0"
        ).initCause(null)
      ;
    }
    if (blobSize == 0) {
      throw (ZeroArraySizeStatus)
        new ZeroArraySizeStatus(
          "blobSize == 0"
        ).initCause(null)
      ;
    }
  }
  final BigInteger intMax = BigInteger.valueOf(Integer.MAX_VALUE);
  if (party == BlobOtCell.DEALER || party == BlobOtCell.RECEIVER) {
    final BigInteger[] nBytesQr =
      BigInteger.valueOf(
        otCount
      ).divideAndRemainder(
        BigInteger.valueOf(8)
      )
    ;
    final BigInteger nBytes =
      nBytesQr[0].add(BigInteger.valueOf(nBytesQr[1].signum()))
    ;
    final BigInteger nBytesPerBuffer =
      intMax
    ;
    final BigInteger[] nBuffersQr =
      nBytes.divideAndRemainder(nBytesPerBuffer)
    ;
    final BigInteger nBuffers =
      nBuffersQr[0].add(BigInteger.valueOf(nBuffersQr[1].signum()))
    ;
    if (nBuffers.compareTo(intMax) > 0) {
      throw (LimitExceededStatus)
        new LimitExceededStatus(
        ).initCause(null)
      ;
    }
    final int nBuffersInt = nBuffers.intValue();
    final int nBytesPerBufferInt = nBytesPerBuffer.intValue();
    this.b = new ByteBuffer[nBuffersInt];
    if (useFiles) {
      this.bFile = File.createTempFile("tmp", null, filesDirectory);
      this.bFile.deleteOnExit();
      this.bFileStream = new RandomAccessFile(this.bFile, "rw");
      try {
        this.bFileStream.setLength(nBytes.longValue());
        final FileChannel c = this.bFileStream.getChannel();
        try {
          for (int i = 0; i != nBuffersInt - 1; ++i) {
            this.b[i] =
              c.map(
                FileChannel.MapMode.READ_WRITE,
                (long)i * nBytesPerBufferInt,
                nBytesPerBufferInt
              )
            ;
          }
          this.b[nBuffersInt - 1] =
            c.map(
              FileChannel.MapMode.READ_WRITE,
              (long)(nBuffersInt - 1) * nBytesPerBufferInt,
              nBuffersQr[1].intValue()
            )
          ;
        } finally {
          try {
            c.close();
          } catch (final Exception e) {
          }
        }
      } catch (final Exception e1) {
        try {
          this.bFileStream.close();
        } catch (final Exception e2) {
        }
        try {
          this.bFile.delete();
        } catch (final Exception e2) {
        }
        throw e1;
      }
    } else {
      this.bFile = null;
      this.bFileStream = null;
      for (int i = 0; i != nBuffersInt - 1; ++i) {
        this.b[i] =
          ByteBuffer.allocate(nBytesPerBufferInt)
        ;
      }
      this.b[nBuffersInt - 1] =
        ByteBuffer.allocate(nBuffersQr[1].intValue())
      ;
    }
  } else {
    this.b = null;
    this.bFile = null;
    this.bFileStream = null;
  }
  this.blobSize = blobSize;
  this.isClosed = false;
  this.otCount = otCount;
  this.party = party;
  if (party == BlobOtCell.DEALER || party == BlobOtCell.SENDER) {
    final BigInteger nBytesPerPair =
      BigInteger.valueOf(
        2
      ).multiply(
        BigInteger.valueOf(blobSize)
      )
    ;
    if (nBytesPerPair.compareTo(intMax) > 0) {
      throw (LimitExceededStatus)
        new LimitExceededStatus(
        ).initCause(null)
      ;
    }
    final BigInteger nBytes =
      BigInteger.valueOf(otCount).multiply(nBytesPerPair)
    ;
    final BigInteger nPairsPerBuffer =
      intMax.divide(nBytesPerPair)
    ;
    final BigInteger nBytesPerBuffer =
      nPairsPerBuffer.multiply(nBytesPerPair)
    ;
    final BigInteger[] nBuffersQr =
      nBytes.divideAndRemainder(nBytesPerBuffer)
    ;
    final BigInteger nBuffers =
      nBuffersQr[0].add(BigInteger.valueOf(nBuffersQr[1].signum()))
    ;
    if (nBuffers.compareTo(intMax) > 0) {
      throw (LimitExceededStatus)
        new LimitExceededStatus(
        ).initCause(null)
      ;
    }
    final int nBuffersInt = nBuffers.intValue();
    final int nBytesPerBufferInt = nBytesPerBuffer.intValue();
    this.s = new ByteBuffer[nBuffersInt];
    if (useFiles) {
      this.sFile = File.createTempFile("tmp", null, filesDirectory);
      this.sFile.deleteOnExit();
      this.sFileStream = new RandomAccessFile(this.sFile, "rw");
      try {
        this.sFileStream.setLength(nBytes.longValue());
        final FileChannel c = this.sFileStream.getChannel();
        try {
          for (int i = 0; i != nBuffersInt - 1; ++i) {
            this.s[i] =
              c.map(
                FileChannel.MapMode.READ_WRITE,
                (long)i * nBytesPerBufferInt,
                nBytesPerBufferInt
              )
            ;
          }
          this.s[nBuffersInt - 1] =
            c.map(
              FileChannel.MapMode.READ_WRITE,
              (long)(nBuffersInt - 1) * nBytesPerBufferInt,
              nBuffersQr[1].intValue()
            )
          ;
        } finally {
          try {
            c.close();
          } catch (final Exception e) {
          }
        }
      } catch (final Exception e1) {
        try {
          this.sFileStream.close();
        } catch (final Exception e2) {
        }
        try {
          this.sFile.delete();
        } catch (final Exception e2) {
        }
        throw e1;
      }
    } else {
      this.sFile = null;
      this.sFileStream = null;
      for (int i = 0; i != nBuffersInt - 1; ++i) {
        this.s[i] =
          ByteBuffer.allocate(nBytesPerBufferInt)
        ;
      }
      this.s[nBuffersInt - 1] =
        ByteBuffer.allocate(nBuffersQr[1].intValue())
      ;
    }
  } else {
    this.s = null;
    this.sFile = null;
    this.sFileStream = null;
  }
  if (party == BlobOtCell.DEALER || party == BlobOtCell.RECEIVER) {
    final BigInteger nBytesPerBlob =
      BigInteger.valueOf(blobSize)
    ;
    final BigInteger nBytes =
      BigInteger.valueOf(otCount).multiply(nBytesPerBlob)
    ;
    final BigInteger nBlobsPerBuffer =
      intMax.divide(nBytesPerBlob)
    ;
    final BigInteger nBytesPerBuffer =
      nBlobsPerBuffer.multiply(nBytesPerBlob)
    ;
    final BigInteger[] nBuffersQr =
      nBytes.divideAndRemainder(nBytesPerBuffer)
    ;
    final BigInteger nBuffers =
      nBuffersQr[0].add(BigInteger.valueOf(nBuffersQr[1].signum()))
    ;
    if (nBuffers.compareTo(intMax) > 0) {
      throw (LimitExceededStatus)
        new LimitExceededStatus(
        ).initCause(null)
      ;
    }
    final int nBuffersInt = nBuffers.intValue();
    final int nBytesPerBufferInt = nBytesPerBuffer.intValue();
    this.u = new ByteBuffer[nBuffersInt];
    if (useFiles) {
      this.uFile = File.createTempFile("tmp", null, filesDirectory);
      this.uFile.deleteOnExit();
      this.uFileStream = new RandomAccessFile(this.uFile, "rw");
      try {
        this.uFileStream.setLength(nBytes.longValue());
        final FileChannel c = this.uFileStream.getChannel();
        try {
          for (int i = 0; i != nBuffersInt - 1; ++i) {
            this.u[i] =
              c.map(
                FileChannel.MapMode.READ_WRITE,
                (long)i * nBytesPerBufferInt,
                nBytesPerBufferInt
              )
            ;
          }
          this.u[nBuffersInt - 1] =
            c.map(
              FileChannel.MapMode.READ_WRITE,
              (long)(nBuffersInt - 1) * nBytesPerBufferInt,
              nBuffersQr[1].intValue()
            )
          ;
        } finally {
          try {
            c.close();
          } catch (final Exception e) {
          }
        }
      } catch (final Exception e1) {
        try {
          this.uFileStream.close();
        } catch (final Exception e2) {
        }
        try {
          this.uFile.delete();
        } catch (final Exception e2) {
        }
        throw e1;
      }
    } else {
      for (int i = 0; i != nBuffersInt - 1; ++i) {
        this.u[i] =
          ByteBuffer.allocate(nBytesPerBufferInt)
        ;
      }
      this.u[nBuffersInt - 1] =
        ByteBuffer.allocate(nBuffersQr[1].intValue())
      ;
    }
  } else {
    this.u = null;
    this.uFile = null;
    this.uFileStream = null;
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

public final void close(
) {
  if (!this.isClosed) {
    if (this.bFileStream != null) {
      try {
        this.bFileStream.close();
      } catch (final Exception e) {
      }
    }
    if (this.bFile != null) {
      try {
        this.bFile.delete();
      } catch (final Exception e) {
      }
    }
    if (this.sFileStream != null) {
      try {
        this.sFileStream.close();
      } catch (final Exception e) {
      }
    }
    if (this.sFile != null) {
      try {
        this.sFile.delete();
      } catch (final Exception e) {
      }
    }
    if (this.uFileStream != null) {
      try {
        this.uFileStream.close();
      } catch (final Exception e) {
      }
    }
    if (this.uFile != null) {
      try {
        this.uFile.delete();
      } catch (final Exception e) {
      }
    }
    this.b = null;
    this.bFile = null;
    this.bFileStream = null;
    this.s = null;
    this.sFile = null;
    this.sFileStream = null;
    this.u = null;
    this.uFile = null;
    this.uFileStream = null;
    this.isClosed = true;
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

public final ByteBuffer getBBuffer(
  final int bufferIndex
) {
  if (BuildConfig.ENABLE_INVALID_ARGUMENT_CHECKS) {
    if (this.party == BlobOtCell.SENDER) {
      throw (InvalidOperationStatus)
        new InvalidOperationStatus(
          "this.party == BlobOtCell.SENDER"
        ).initCause(null)
      ;
    }
    if (this.isClosed) {
      throw (InvalidOperationStatus)
        new InvalidOperationStatus(
          "this.isClosed"
        ).initCause(null)
      ;
    }
    if (bufferIndex < 0) {
      throw (NegativeArrayIndexStatus)
        new NegativeArrayIndexStatus(
          "bufferIndex < 0"
        ).initCause(null)
      ;
    }
    if (bufferIndex >= this.b.length) {
      throw (OversizeArrayIndexStatus)
        new OversizeArrayIndexStatus(
          "bufferIndex >= this.b.length"
        ).initCause(null)
      ;
    }
  }
  this.b[bufferIndex].position(0);
  return this.b[bufferIndex].duplicate();
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

public final int getBBufferCount(
) {
  if (BuildConfig.ENABLE_INVALID_ARGUMENT_CHECKS) {
    if (this.party == BlobOtCell.SENDER) {
      throw (InvalidOperationStatus)
        new InvalidOperationStatus(
          "this.party == BlobOtCell.SENDER"
        ).initCause(null)
      ;
    }
    if (this.isClosed) {
      throw (InvalidOperationStatus)
        new InvalidOperationStatus(
          "this.isClosed"
        ).initCause(null)
      ;
    }
  }
  return this.b.length;
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

public final int getBValue(
  final long otIndex
) {
  if (BuildConfig.ENABLE_INVALID_ARGUMENT_CHECKS) {
    if (this.party == BlobOtCell.SENDER) {
      throw (InvalidOperationStatus)
        new InvalidOperationStatus(
          "this.party == BlobOtCell.SENDER"
        ).initCause(null)
      ;
    }
    if (this.isClosed) {
      throw (InvalidOperationStatus)
        new InvalidOperationStatus(
          "this.isClosed"
        ).initCause(null)
      ;
    }
    if (otIndex < 0) {
      throw (NegativeArrayIndexStatus)
        new NegativeArrayIndexStatus(
          "otIndex < 0"
        ).initCause(null)
      ;
    }
    if (otIndex >= this.otCount) {
      throw (OversizeArrayIndexStatus)
        new OversizeArrayIndexStatus(
          "otIndex >= this.otCount"
        ).initCause(null)
      ;
    }
  }
  final long pB = otIndex / 8;
  final int iB = (int)(pB / this.b[0].capacity());
  final int jB = (int)(pB % this.b[0].capacity());
  final int kB = (int)(otIndex % 8);
  return (this.b[iB].get(jB) >> kB) & 1;
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

public final long getOtCount(
) {
  if (BuildConfig.ENABLE_INVALID_ARGUMENT_CHECKS) {
    if (this.isClosed) {
      throw (InvalidOperationStatus)
        new InvalidOperationStatus(
          "this.isClosed"
        ).initCause(null)
      ;
    }
  }
  return this.otCount;
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

public final ByteBuffer getSBuffer(
  final int bufferIndex
) {
  if (BuildConfig.ENABLE_INVALID_ARGUMENT_CHECKS) {
    if (this.party == BlobOtCell.RECEIVER) {
      throw (InvalidOperationStatus)
        new InvalidOperationStatus(
          "this.party == BlobOtCell.RECEIVER"
        ).initCause(null)
      ;
    }
    if (this.isClosed) {
      throw (InvalidOperationStatus)
        new InvalidOperationStatus(
          "this.isClosed"
        ).initCause(null)
      ;
    }
    if (bufferIndex < 0) {
      throw (NegativeArrayIndexStatus)
        new NegativeArrayIndexStatus(
          "bufferIndex < 0"
        ).initCause(null)
      ;
    }
    if (bufferIndex >= this.s.length) {
      throw (OversizeArrayIndexStatus)
        new OversizeArrayIndexStatus(
          "bufferIndex >= this.s.length"
        ).initCause(null)
      ;
    }
  }
  this.s[bufferIndex].position(0);
  return this.s[bufferIndex].duplicate();
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

public final int getSBufferCount(
) {
  if (BuildConfig.ENABLE_INVALID_ARGUMENT_CHECKS) {
    if (this.party == BlobOtCell.RECEIVER) {
      throw (InvalidOperationStatus)
        new InvalidOperationStatus(
          "this.party == BlobOtCell.RECEIVER"
        ).initCause(null)
      ;
    }
    if (this.isClosed) {
      throw (InvalidOperationStatus)
        new InvalidOperationStatus(
          "this.isClosed"
        ).initCause(null)
      ;
    }
  }
  return this.s.length;
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

public final void getSValue(
  final long otIndex,
  final int pairIndex,
  final byte[] value,
  final int valueOffset
) {
  if (BuildConfig.ENABLE_INVALID_ARGUMENT_CHECKS) {
    if (this.party == BlobOtCell.RECEIVER) {
      throw (InvalidOperationStatus)
        new InvalidOperationStatus(
          "this.party == BlobOtCell.RECEIVER"
        ).initCause(null)
      ;
    }
    if (this.isClosed) {
      throw (InvalidOperationStatus)
        new InvalidOperationStatus(
          "this.isClosed"
        ).initCause(null)
      ;
    }
    if (otIndex < 0) {
      throw (NegativeArrayIndexStatus)
        new NegativeArrayIndexStatus(
          "otIndex < 0"
        ).initCause(null)
      ;
    }
    if (otIndex >= this.otCount) {
      throw (OversizeArrayIndexStatus)
        new OversizeArrayIndexStatus(
          "otIndex >= this.otCount"
        ).initCause(null)
      ;
    }
    if (pairIndex < 0) {
      throw (NegativeArrayIndexStatus)
        new NegativeArrayIndexStatus(
          "pairIndex < 0"
        ).initCause(null)
      ;
    }
    if (pairIndex > 1) {
      throw (OversizeArrayIndexStatus)
        new OversizeArrayIndexStatus(
          "pairIndex > 1"
        ).initCause(null)
      ;
    }
    if (value == null) {
      throw (NullPointerStatus)
        new NullPointerStatus(
          "value == null"
        ).initCause(null)
      ;
    }
    if (valueOffset < 0) {
      throw (NegativeArrayIndexStatus)
        new NegativeArrayIndexStatus(
          "valueOffset < 0"
        ).initCause(null)
      ;
    }
    if (valueOffset > value.length - this.blobSize) {
      throw (OversizeArrayIndexStatus)
        new OversizeArrayIndexStatus(
          "valueOffset > value.length - this.blobSize"
        ).initCause(null)
      ;
    }
  }
  final long pS = (otIndex * 2 + pairIndex) * this.blobSize;
  final int iS = (int)(pS / this.s[0].capacity());
  final int jS = (int)(pS % this.s[0].capacity());
  this.s[iS].position(jS);
  this.s[iS].get(value, valueOffset, this.blobSize);
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

public final ByteBuffer getUBuffer(
  final int bufferIndex
) {
  if (BuildConfig.ENABLE_INVALID_ARGUMENT_CHECKS) {
    if (this.party == BlobOtCell.SENDER) {
      throw (InvalidOperationStatus)
        new InvalidOperationStatus(
          "this.party == BlobOtCell.SENDER"
        ).initCause(null)
      ;
    }
    if (this.isClosed) {
      throw (InvalidOperationStatus)
        new InvalidOperationStatus(
          "this.isClosed"
        ).initCause(null)
      ;
    }
    if (bufferIndex < 0) {
      throw (NegativeArrayIndexStatus)
        new NegativeArrayIndexStatus(
          "bufferIndex < 0"
        ).initCause(null)
      ;
    }
    if (bufferIndex >= this.u.length) {
      throw (OversizeArrayIndexStatus)
        new OversizeArrayIndexStatus(
          "bufferIndex >= this.u.length"
        ).initCause(null)
      ;
    }
  }
  this.u[bufferIndex].position(0);
  return this.u[bufferIndex].duplicate();
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

public final int getUBufferCount(
) {
  if (BuildConfig.ENABLE_INVALID_ARGUMENT_CHECKS) {
    if (this.party == BlobOtCell.SENDER) {
      throw (InvalidOperationStatus)
        new InvalidOperationStatus(
          "this.party == BlobOtCell.SENDER"
        ).initCause(null)
      ;
    }
    if (this.isClosed) {
      throw (InvalidOperationStatus)
        new InvalidOperationStatus(
          "this.isClosed"
        ).initCause(null)
      ;
    }
  }
  return this.u.length;
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

public final void getUValue(
  final long otIndex,
  final byte[] value,
  final int valueOffset
) {
  if (BuildConfig.ENABLE_INVALID_ARGUMENT_CHECKS) {
    if (this.party == BlobOtCell.SENDER) {
      throw (InvalidOperationStatus)
        new InvalidOperationStatus(
          "this.party == BlobOtCell.SENDER"
        ).initCause(null)
      ;
    }
    if (this.isClosed) {
      throw (InvalidOperationStatus)
        new InvalidOperationStatus(
          "this.isClosed"
        ).initCause(null)
      ;
    }
    if (otIndex < 0) {
      throw (NegativeArrayIndexStatus)
        new NegativeArrayIndexStatus(
          "otIndex < 0"
        ).initCause(null)
      ;
    }
    if (otIndex >= this.otCount) {
      throw (OversizeArrayIndexStatus)
        new OversizeArrayIndexStatus(
          "otIndex >= this.otCount"
        ).initCause(null)
      ;
    }
  }
  final long pU = otIndex * this.blobSize;
  final int iU = (int)(pU / this.u[0].capacity());
  final int jU = (int)(pU % this.u[0].capacity());
  this.u[iU].position(jU);
  this.u[iU].get(value, valueOffset, this.blobSize);
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

public final void randomize(
  final Random src,
  final int memoryHint
) {
  this.randomize(src, memoryHint, 0, this.otCount);
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

public void randomize(
  final Random src,
  final int memoryHint,
  final long otOffset,
  final long otLength
) {
  if (BuildConfig.ENABLE_INVALID_ARGUMENT_CHECKS) {
    if (this.party != BlobOtCell.DEALER) {
      throw (InvalidOperationStatus)
        new InvalidOperationStatus(
          "this.party != BlobOtCell.DEALER"
        ).initCause(null)
      ;
    }
    if (this.isClosed) {
      throw (InvalidOperationStatus)
        new InvalidOperationStatus(
          "this.isClosed"
        ).initCause(null)
      ;
    }
    if (otOffset < 0) {
      throw (NegativeArrayIndexStatus)
        new NegativeArrayIndexStatus(
          "otOffset < 0"
        ).initCause(null)
      ;
    }
    if (otLength < 0) {
      throw (NegativeArraySizeStatus)
        new NegativeArraySizeStatus(
          "otLength < 0"
        ).initCause(null)
      ;
    }
    if (otOffset > this.otCount - otLength) {
      throw (OversizeArrayIndexStatus)
        new OversizeArrayIndexStatus(
          "otOffset > this.otCount - otLength"
        ).initCause(null)
      ;
    }
    if (otOffset % 8 != 0) {
      throw (InvalidArgumentStatus)
        new InvalidArgumentStatus(
          "otOffset % 8 != 0"
        ).initCause(null)
      ;
    }
    if (otLength % 8 != 0 && otLength != this.otCount - otOffset) {
      throw (InvalidArgumentStatus)
        new InvalidArgumentStatus(
          "otLength % 8 != 0 && otLength != this.otCount - otOffset"
        ).initCause(null)
      ;
    }
  }
  if (src == null) {
    this.randomize(
      Rand.defaultSrc(),
      memoryHint,
      otOffset,
      otLength
    );
    return;
  }
  if (memoryHint < 0) {
    this.randomize(
      Rand.defaultSrc(),
      Rand.defaultMemoryHint(),
      otOffset,
      otLength
    );
    return;
  }
  {
    final long pB0 = otOffset / 8;
    final int iB0 = (int)(pB0 / this.b[0].capacity());
    final int jB0 = (int)(pB0 % this.b[0].capacity());
    final long pB1 = (otOffset + otLength - 1) / 8;
    final int iB1 = (int)(pB1 / this.b[0].capacity());
    final int jB1 = (int)(pB1 % this.b[0].capacity());
    if (iB0 == iB1) {
      this.b[iB0].position(jB0);
      Rand.bytes(
        src,
        this.b[iB0],
        jB1 - jB0 + 1,
        memoryHint
      );
    } else {
      this.b[iB0].position(jB0);
      Rand.bytes(
        src,
        this.b[iB0],
        this.b[iB0].remaining(),
        memoryHint
      );
      for (int i = iB0 + 1; i != iB1; ++i) {
        this.b[i].position(0);
        Rand.bytes(
          src,
          this.b[i],
          this.b[i].remaining(),
          memoryHint
        );
      }
      this.b[iB1].position(0);
      Rand.bytes(
        src,
        this.b[iB1],
        jB1 + 1,
        memoryHint
      );
    }
  }
  {
    final long pS0 = otOffset * 2 * this.blobSize;
    final int iS0 = (int)(pS0 / this.s[0].capacity());
    final int jS0 = (int)(pS0 % this.s[0].capacity());
    final long pS1 = (otOffset + otLength) * 2 * this.blobSize - 1;
    final int iS1 = (int)(pS1 / this.s[0].capacity());
    final int jS1 = (int)(pS1 % this.s[0].capacity());
    if (iS0 == iS1) {
      this.s[iS0].position(jS0);
      Rand.bytes(
        src,
        this.s[iS0],
        jS1 - jS0 + 1,
        memoryHint
      );
    } else {
      this.s[iS0].position(jS0);
      Rand.bytes(
        src,
        this.s[iS0],
        this.s[iS0].remaining(),
        memoryHint
      );
      for (int i = iS0 + 1; i != iS1; ++i) {
        this.s[i].position(0);
        Rand.bytes(
          src,
          this.s[i],
          this.s[i].remaining(),
          memoryHint
        );
      }
      this.s[iS1].position(0);
      Rand.bytes(
        src,
        this.s[iS1],
        jS1 + 1,
        memoryHint
      );
    }
  }
  for (long i = otOffset; i != otOffset + otLength; ++i) {
    final long pS = (i * 2 + this.getBValue(i)) * this.blobSize;
    final int iS = (int)(pS / this.s[0].capacity());
    final int jS = (int)(pS % this.s[0].capacity());
    final long pU = i * this.blobSize;
    final int iU = (int)(pU / this.u[0].capacity());
    final int jU = (int)(pU % this.u[0].capacity());
    this.s[iS].limit(jS + this.blobSize);
    this.s[iS].position(jS);
    this.u[iU].position(jU);
    this.u[iU].put(this.s[iS]);
    this.s[iS].limit(this.s[iS].capacity());
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

public final void setBValue(
  final long otIndex,
  final int value
) {
  if (BuildConfig.ENABLE_INVALID_ARGUMENT_CHECKS) {
    if (this.party == BlobOtCell.SENDER) {
      throw (InvalidOperationStatus)
        new InvalidOperationStatus(
          "this.party == BlobOtCell.SENDER"
        ).initCause(null)
      ;
    }
    if (this.isClosed) {
      throw (InvalidOperationStatus)
        new InvalidOperationStatus(
          "this.isClosed"
        ).initCause(null)
      ;
    }
    if (otIndex < 0) {
      throw (NegativeArrayIndexStatus)
        new NegativeArrayIndexStatus(
          "otIndex < 0"
        ).initCause(null)
      ;
    }
    if (otIndex >= this.otCount) {
      throw (OversizeArrayIndexStatus)
        new OversizeArrayIndexStatus(
          "otIndex >= this.otCount"
        ).initCause(null)
      ;
    }
  }
  if (value != 0 && value != 1) {
    this.setBValue(otIndex, (value != 0) ? 1 : 0);
    return;
  }
  final long pB = otIndex / 8;
  final int iB = (int)(pB / this.b[0].capacity());
  final int jB = (int)(pB % this.b[0].capacity());
  final int kB = (int)(otIndex % 8);
  if (value == 0) {
    this.b[iB].put(jB, (byte)(this.b[iB].get(jB) & ~(1 << kB)));
  } else {
    this.b[iB].put(jB, (byte)(this.b[iB].get(jB) | (1 << kB)));
  }
  if (this.party == BlobOtCell.DEALER) {
    final long pS = (otIndex * 2 + value) * this.blobSize;
    final int iS = (int)(pS / this.s[0].capacity());
    final int jS = (int)(pS % this.s[0].capacity());
    final long pU = otIndex * this.blobSize;
    final int iU = (int)(pU / this.u[0].capacity());
    final int jU = (int)(pU % this.u[0].capacity());
    this.s[iS].limit(jS + this.blobSize);
    this.s[iS].position(jS);
    this.u[iU].position(jU);
    this.u[iU].put(this.s[iS]);
    this.s[iS].limit(this.s[iS].capacity());
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

public void setSValue(
  final long otIndex,
  final int pairIndex,
  final byte[] value,
  final int valueOffset
) {
  if (BuildConfig.ENABLE_INVALID_ARGUMENT_CHECKS) {
    if (this.party == BlobOtCell.RECEIVER) {
      throw (InvalidOperationStatus)
        new InvalidOperationStatus(
          "this.party == BlobOtCell.RECEIVER"
        ).initCause(null)
      ;
    }
    if (this.isClosed) {
      throw (InvalidOperationStatus)
        new InvalidOperationStatus(
          "this.isClosed"
        ).initCause(null)
      ;
    }
    if (otIndex < 0) {
      throw (NegativeArrayIndexStatus)
        new NegativeArrayIndexStatus(
          "otIndex < 0"
        ).initCause(null)
      ;
    }
    if (otIndex >= this.otCount) {
      throw (OversizeArrayIndexStatus)
        new OversizeArrayIndexStatus(
          "otIndex >= this.otCount"
        ).initCause(null)
      ;
    }
    if (pairIndex < 0) {
      throw (NegativeArrayIndexStatus)
        new NegativeArrayIndexStatus(
          "pairIndex < 0"
        ).initCause(null)
      ;
    }
    if (pairIndex > 1) {
      throw (OversizeArrayIndexStatus)
        new OversizeArrayIndexStatus(
          "pairIndex > 1"
        ).initCause(null)
      ;
    }
    if (value == null) {
      throw (NullPointerStatus)
        new NullPointerStatus(
          "value == null"
        ).initCause(null)
      ;
    }
    if (valueOffset < 0) {
      throw (NegativeArrayIndexStatus)
        new NegativeArrayIndexStatus(
          "valueOffset < 0"
        ).initCause(null)
      ;
    }
    if (valueOffset > value.length - this.blobSize) {
      throw (OversizeArrayIndexStatus)
        new OversizeArrayIndexStatus(
          "valueOffset > value.length - this.blobSize"
        ).initCause(null)
      ;
    }
  }
  final long pS = (otIndex * 2 + pairIndex) * this.blobSize;
  final int iS = (int)(pS / this.s[0].capacity());
  final int jS = (int)(pS % this.s[0].capacity());
  this.s[iS].position(jS);
  this.s[iS].put(value, valueOffset, this.blobSize);
  if (this.party == BlobOtCell.DEALER) {
    if (pairIndex == this.getBValue(otIndex)) {
      final long pU = otIndex * this.blobSize;
      final int iU = (int)(pU / this.u[0].capacity());
      final int jU = (int)(pU % this.u[0].capacity());
      this.u[iU].position(jU);
      this.u[iU].put(value, valueOffset, this.blobSize);
    }
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

public void setUValue(
  final long otIndex,
  final byte[] value,
  final int valueOffset
) {
  if (BuildConfig.ENABLE_INVALID_ARGUMENT_CHECKS) {
    if (this.party == BlobOtCell.SENDER) {
      throw (InvalidOperationStatus)
        new InvalidOperationStatus(
          "this.party == BlobOtCell.SENDER"
        ).initCause(null)
      ;
    }
    if (this.isClosed) {
      throw (InvalidOperationStatus)
        new InvalidOperationStatus(
          "this.isClosed"
        ).initCause(null)
      ;
    }
    if (otIndex < 0) {
      throw (NegativeArrayIndexStatus)
        new NegativeArrayIndexStatus(
          "otIndex < 0"
        ).initCause(null)
      ;
    }
    if (otIndex >= this.otCount) {
      throw (OversizeArrayIndexStatus)
        new OversizeArrayIndexStatus(
          "otIndex >= this.otCount"
        ).initCause(null)
      ;
    }
    if (value == null) {
      throw (NullPointerStatus)
        new NullPointerStatus(
          "value == null"
        ).initCause(null)
      ;
    }
    if (valueOffset < 0) {
      throw (NegativeArrayIndexStatus)
        new NegativeArrayIndexStatus(
          "valueOffset < 0"
        ).initCause(null)
      ;
    }
    if (valueOffset > value.length - this.blobSize) {
      throw (OversizeArrayIndexStatus)
        new OversizeArrayIndexStatus(
          "valueOffset > value.length - this.blobSize"
        ).initCause(null)
      ;
    }
  }
  final long pU = otIndex * this.blobSize;
  final int iU = (int)(pU / this.u[0].capacity());
  final int jU = (int)(pU % this.u[0].capacity());
  this.u[iU].position(jU);
  this.u[iU].put(value, valueOffset, this.blobSize);
  if (this.party == BlobOtCell.DEALER) {
    final int pairIndex = this.getBValue(otIndex);
    final long pS = (otIndex * 2 + pairIndex) * this.blobSize;
    final int iS = (int)(pS / this.s[0].capacity());
    final int jS = (int)(pS % this.s[0].capacity());
    this.s[iS].position(jS);
    this.s[iS].put(value, valueOffset, this.blobSize);
  }
}

}
