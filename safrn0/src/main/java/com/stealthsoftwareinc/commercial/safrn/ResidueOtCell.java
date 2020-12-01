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
 * <code>com.stealthsoftwareinc.commercial.safrn.ResidueOtCell</code>
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

import com.stealthsoftwareinc.commercial.safrn.BlobOtCell;
import com.stealthsoftwareinc.commercial.safrn.BuildConfig;
import com.stealthsoftwareinc.sst.InvalidArgumentStatus;
import com.stealthsoftwareinc.sst.InvalidOperationStatus;
import com.stealthsoftwareinc.sst.NegativeArrayIndexStatus;
import com.stealthsoftwareinc.sst.NegativeArraySizeStatus;
import com.stealthsoftwareinc.sst.NullPointerStatus;
import com.stealthsoftwareinc.sst.OversizeArrayIndexStatus;
import com.stealthsoftwareinc.sst.Rand;
import java.io.File;
import java.io.IOException;
import java.math.BigInteger;
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

public final class ResidueOtCell
extends BlobOtCell
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

private final BigInteger modulus;

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

public ResidueOtCell(
  final int party,
  final long otCount,
  final BigInteger modulus,
  final boolean useFiles,
  final File filesDirectory
) throws
  IOException
{
  super(
    party,
    otCount,
    ResidueOtCell.computeBlobSize(modulus),
    useFiles,
    filesDirectory
  );
  this.modulus = modulus;
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

private static int computeBlobSize(
  final BigInteger modulus
) {
  if (BuildConfig.ENABLE_INVALID_ARGUMENT_CHECKS) {
    if (modulus == null) {
      throw (NullPointerStatus)
        new NullPointerStatus(
          "modulus == null"
        ).initCause(null)
      ;
    }
    if (modulus.signum() < 0) {
      throw (InvalidArgumentStatus)
        new InvalidArgumentStatus(
          "modulus < 0"
        ).initCause(null)
      ;
    }
    if (modulus.signum() == 0) {
      throw (InvalidArgumentStatus)
        new InvalidArgumentStatus(
          "modulus == 0"
        ).initCause(null)
      ;
    }
  }
  return Rand.residueSize(modulus);
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

public final BigInteger getSValue(
  final long otIndex,
  final int pairIndex
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
  }
  final byte[] x = new byte[1 + this.blobSize];
  super.getSValue(otIndex, pairIndex, x, 1);
  return new BigInteger(x);
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

public final BigInteger getUValue(
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
  final byte[] x = new byte[1 + this.blobSize];
  super.getUValue(otIndex, x, 1);
  return new BigInteger(x);
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

@Override
public final void randomize(
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
      Rand.modular(
        src,
        this.modulus,
        this.s[iS0],
        jS1 - jS0 + 1,
        memoryHint
      );
    } else {
      this.s[iS0].position(jS0);
      Rand.modular(
        src,
        this.modulus,
        this.s[iS0],
        this.s[iS0].remaining(),
        memoryHint
      );
      for (int i = iS0 + 1; i != iS1; ++i) {
        this.s[i].position(0);
        Rand.modular(
          src,
          this.modulus,
          this.s[i],
          this.s[i].remaining(),
          memoryHint
        );
      }
      this.s[iS1].position(0);
      Rand.modular(
        src,
        this.modulus,
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

public final void setSValue(
  final long otIndex,
  final int pairIndex,
  final BigInteger value
) {
  if (BuildConfig.ENABLE_INVALID_ARGUMENT_CHECKS) {
    if (value != null) {
      if (value.signum() < 0) {
        throw (InvalidArgumentStatus)
          new InvalidArgumentStatus(
            "value < 0"
          ).initCause(null)
        ;
      }
      if (value.compareTo(this.modulus) >= 0) {
        throw (InvalidArgumentStatus)
          new InvalidArgumentStatus(
            "value >= this.modulus"
          ).initCause(null)
        ;
      }
    }
  }
  if (value == null) {
    this.setSValue(
      otIndex,
      pairIndex,
      BigInteger.ZERO
    );
    return;
  }
  final byte[] x = value.toByteArray();
  final int i;
  if (x.length == 1 || x[0] != 0) {
    i = 0;
  } else {
    i = 1;
  }
  final byte[] y = new byte[this.blobSize];
  System.arraycopy(
    x,
    i,
    y,
    y.length - (x.length - i),
    x.length - i
  );
  super.setSValue(otIndex, pairIndex, y, 0);
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

@Override
public final void setSValue(
  final long otIndex,
  final int pairIndex,
  final byte[] value,
  final int valueOffset
) {
  if (BuildConfig.ENABLE_INVALID_ARGUMENT_CHECKS) {
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
  final byte[] x = new byte[1 + this.blobSize];
  System.arraycopy(value, valueOffset, x, 1, this.blobSize);
  this.setSValue(otIndex, pairIndex, new BigInteger(x));
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

public final void setUValue(
  final long otIndex,
  final BigInteger value
) {
  if (BuildConfig.ENABLE_INVALID_ARGUMENT_CHECKS) {
    if (value != null) {
      if (value.signum() < 0) {
        throw (InvalidArgumentStatus)
          new InvalidArgumentStatus(
            "value < 0"
          ).initCause(null)
        ;
      }
      if (value.compareTo(this.modulus) >= 0) {
        throw (InvalidArgumentStatus)
          new InvalidArgumentStatus(
            "value >= this.modulus"
          ).initCause(null)
        ;
      }
    }
  }
  if (value == null) {
    this.setUValue(
      otIndex,
      BigInteger.ZERO
    );
    return;
  }
  final byte[] x = value.toByteArray();
  final int i;
  if (x.length == 1 || x[0] != 0) {
    i = 0;
  } else {
    i = 1;
  }
  final byte[] y = new byte[this.blobSize];
  System.arraycopy(
    x,
    i,
    y,
    y.length - (x.length - i),
    x.length - i
  );
  super.setUValue(otIndex, y, 0);
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

@Override
public final void setUValue(
  final long otIndex,
  final byte[] value,
  final int valueOffset
) {
  if (BuildConfig.ENABLE_INVALID_ARGUMENT_CHECKS) {
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
  final byte[] x = new byte[1 + this.blobSize];
  System.arraycopy(value, valueOffset, x, 1, this.blobSize);
  this.setUValue(otIndex, new BigInteger(x));
}

}
