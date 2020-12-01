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
 * <code>com.stealthsoftwareinc.commercial.safrn.CuckooTable</code>
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
import com.stealthsoftwareinc.sst.BigIntArray;
import com.stealthsoftwareinc.sst.FailureStatus;
import com.stealthsoftwareinc.sst.InternalErrorStatus;
import com.stealthsoftwareinc.sst.InvalidArgumentStatus;
import com.stealthsoftwareinc.sst.InvalidOperationStatus;
import java.io.File;
import java.io.IOException;
import java.lang.Exception;
import java.lang.Integer;
import java.lang.String;
import java.security.InvalidKeyException;
import java.security.NoSuchAlgorithmException;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Queue;
import java.util.Set;
import javax.crypto.BadPaddingException;
import javax.crypto.Cipher;
import javax.crypto.IllegalBlockSizeException;
import javax.crypto.NoSuchPaddingException;
import javax.crypto.spec.SecretKeySpec;

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

public final class CuckooTable
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

public static final int NULL_XR = 1;

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

private final Cipher[] hashCiphers;

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

private boolean isClosed;

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

private final int rightBits;

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

private final int subtableBits;

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

public final BigIntArray[] subtables;

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

private final int subtableSize;

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

private final int xBits;

/**
 * Creates a cuckoo table that stores
 * <code>xBits</code>-bit
 * elements in subtables of size
 * 2<sup><code>subtableBits</code></sup>.
 *
 * @since SAFRN&nbsp;X.X.X (XXXX-XX-XX)
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

public CuckooTable(
  final int subtableBits,
  final int xBits
) {
  final CuckooTable that;
  try {
    that =
      new CuckooTable(
        subtableBits,
        xBits,
        false,
        null
      )
    ;
  } catch (final IOException e) {
    throw (InternalErrorStatus)
      new InternalErrorStatus(
      ).initCause(e)
    ;
  }
  this.hashCiphers = that.hashCiphers;
  this.isClosed = that.isClosed;
  this.rightBits = that.rightBits;
  this.subtableBits = that.subtableBits;
  this.subtableSize = that.subtableSize;
  this.subtables = that.subtables;
  this.xBits = that.xBits;
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

public CuckooTable(
  final int subtableBits,
  final int xBits,
  final boolean useFiles,
  final File filesDirectory
) throws
  IOException
{
  if (BuildConfig.ENABLE_INVALID_ARGUMENT_CHECKS) {
    if (subtableBits < 1) {
      throw (InvalidArgumentStatus)
        new InvalidArgumentStatus(
          "subtableBits < 1"
        ).initCause(null)
      ;
    }
    if (subtableBits > 30) {
      throw (InvalidArgumentStatus)
        new InvalidArgumentStatus(
          "subtableBits > 30"
        ).initCause(null)
      ;
    }
    if (xBits < 2) {
      throw (InvalidArgumentStatus)
        new InvalidArgumentStatus(
          "xBits < 2"
        ).initCause(null)
      ;
    }
    if (xBits > 32) {
      throw (InvalidArgumentStatus)
        new InvalidArgumentStatus(
          "xBits > 32"
        ).initCause(null)
      ;
    }
    if (xBits <= subtableBits) {
      throw (InvalidArgumentStatus)
        new InvalidArgumentStatus(
          "xBits <= subtableBits"
        ).initCause(null)
      ;
    }
  }
  this.hashCiphers = new Cipher[4];
  for (int wa = 0; wa != 4; ++wa) {
    try {
      this.hashCiphers[wa] = Cipher.getInstance("AES/ECB/NoPadding");
    } catch (final NoSuchAlgorithmException e) {
      throw (InternalErrorStatus)
        new InternalErrorStatus(
        ).initCause(e)
      ;
    } catch (final NoSuchPaddingException e) {
      throw (InternalErrorStatus)
        new InternalErrorStatus(
        ).initCause(e)
      ;
    }
    try {
      this.hashCiphers[wa].init(
        Cipher.ENCRYPT_MODE,
        new SecretKeySpec(
          new byte[] {
            0, 0, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, (byte)wa
          },
          "AES"
        )
      );
    } catch (final InvalidKeyException e) {
      throw (InternalErrorStatus)
        new InternalErrorStatus(
        ).initCause(e)
      ;
    }
  }
  this.isClosed = false;
  this.subtableBits = subtableBits;
  this.subtableSize = 1 << this.subtableBits;
  this.subtables = new BigIntArray[4];
  for (int wa = 0; wa != 4; ++wa) {
    this.subtables[wa] =
      new BigIntArray(
        this.subtableSize,
        useFiles,
        filesDirectory
      )
    ;
  }
  this.xBits = xBits;
  this.rightBits = this.xBits - this.subtableBits;
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

public final List<Map<Integer, Integer>> buildAlice(
  final ResultSet xs,
  final int maxDepth
) throws
  SQLException
{
  if (BuildConfig.ENABLE_INVALID_ARGUMENT_CHECKS) {
    if (this.isClosed) {
      throw (InvalidOperationStatus)
        new InvalidOperationStatus(
          "this.isClosed"
        ).initCause(null)
      ;
    }
    if (maxDepth < 1) {
      throw (InvalidArgumentStatus)
        new InvalidArgumentStatus(
        ).initCause(null)
      ;
    }
  }
  final int columnCount = xs.getMetaData().getColumnCount();
  final List<Map<Integer, Integer>> dvMaps =
    new ArrayList<Map<Integer, Integer>>(columnCount - 1)
  ;
  for (int i = 2; i <= columnCount; ++i) {
    dvMaps.add(new HashMap<Integer, Integer>());
  }
  if (xs == null) {
    for (int wa = 0; wa != 4; ++wa) {
      for (int hwax = 0; hwax != this.subtableSize; ++hwax) {
        this.subtables[wa].setValue(hwax, CuckooTable.NULL_XR);
      }
    }
    return dvMaps;
  }
  @SuppressWarnings("unchecked")
  final Queue<Integer>[][] U = new Queue[4][];
  for (int wa = 0; wa != 4; ++wa) {
    @SuppressWarnings("unchecked")
    final Queue<Integer>[] q = new Queue[this.subtableSize];
    U[wa] = q;
  }
  final Set<Integer> seen = new HashSet<Integer>();
  while (xs.next()) {
    final int x = xs.getInt(1) * 2;
    for (int i = 2; i <= columnCount; ++i) {
      dvMaps.get(i - 2).put(x, xs.getInt(i));
    }
    if (BuildConfig.ENABLE_INVALID_ARGUMENT_CHECKS) {
      if ((x >>> (this.xBits - 1) >>> 1) != 0) {
        throw (InvalidArgumentStatus)
          new InvalidArgumentStatus(
          ).initCause(null)
        ;
      }
      if (seen.contains(x)) {
        throw (InvalidArgumentStatus)
          new InvalidArgumentStatus(
          ).initCause(null)
        ;
      }
      seen.add(x);
    }
    final int xL = this.splitL(x);
    final int xR = this.splitR(x);
    final int w = 0;
    for (int a = 0; a != 2; ++a) {
      final int wa = w * 2 + a;
      final int hwax = this.hash(wa, xL, xR);
      if (U[wa][hwax] == null) {
        U[wa][hwax] = new ArrayDeque<Integer>();
      }
      U[wa][hwax].add(xR);
    }
  }
  this.buildAliceMain(U, maxDepth);
  return dvMaps;
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

public final void buildAlice(
  final int[] xs,
  final int maxDepth
) {
  if (BuildConfig.ENABLE_INVALID_ARGUMENT_CHECKS) {
    if (this.isClosed) {
      throw (InvalidOperationStatus)
        new InvalidOperationStatus(
          "this.isClosed"
        ).initCause(null)
      ;
    }
    if (maxDepth < 1) {
      throw (InvalidArgumentStatus)
        new InvalidArgumentStatus(
        ).initCause(null)
      ;
    }
  }
  if (xs == null) {
    for (int wa = 0; wa != 4; ++wa) {
      for (int hwax = 0; hwax != this.subtableSize; ++hwax) {
        this.subtables[wa].setValue(hwax, CuckooTable.NULL_XR);
      }
    }
    return;
  }
  @SuppressWarnings("unchecked")
  final Queue<Integer>[][] U = new Queue[4][];
  for (int wa = 0; wa != 4; ++wa) {
    @SuppressWarnings("unchecked")
    final Queue<Integer>[] q = new Queue[this.subtableSize];
    U[wa] = q;
  }
  final Set<Integer> seen = new HashSet<Integer>();
  for (int i = 0; i != xs.length; ++i) {
    final int x = xs[i] * 2;
    if (BuildConfig.ENABLE_INVALID_ARGUMENT_CHECKS) {
      if ((x >>> (this.xBits - 1) >>> 1) != 0) {
        throw (InvalidArgumentStatus)
          new InvalidArgumentStatus(
          ).initCause(null)
        ;
      }
      if (seen.contains(x)) {
        throw (InvalidArgumentStatus)
          new InvalidArgumentStatus(
          ).initCause(null)
        ;
      }
      seen.add(x);
    }
    final int xL = this.splitL(x);
    final int xR = this.splitR(x);
    final int w = 0;
    for (int a = 0; a != 2; ++a) {
      final int wa = w * 2 + a;
      final int hwax = this.hash(wa, xL, xR);
      if (U[wa][hwax] == null) {
        U[wa][hwax] = new ArrayDeque<Integer>();
      }
      U[wa][hwax].add(xR);
    }
  }
  this.buildAliceMain(U, maxDepth);
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

private final void buildAliceMain(
  final Queue<Integer>[][] U,
  final int maxDepth
) {
  int w = 0;
  final Queue<Integer> S = new ArrayDeque<Integer>();
  for (int depth = 0; depth != maxDepth; ++depth) {
    for (int a = 0; a != 2; ++a) {
      final int b = 1 - a;
      final int wa = w * 2 + a;
      final int wb = w * 2 + b;
      for (int hwax = 0; hwax != this.subtableSize; ++hwax) {
        if (U[wa][hwax] != null) {
          while (U[wa][hwax].size() > 1) {
            final int xR = U[wa][hwax].remove();
            final int x = this.unhash(wa, hwax, xR);
            final int xL = this.splitL(x);
            final int hwbx = this.hash(wb, xL, xR);
            U[wb][hwbx].remove(Integer.valueOf(xR));
            S.add(x);
          }
        }
      }
    }
    if (S.isEmpty()) {
      for (int v = 0; v != 2; ++v) {
        for (int c = 0; c != 2; ++c) {
          final int vc = v * 2 + c;
          for (int hvcx = 0; hvcx != this.subtableSize; ++hvcx) {
            if (U[vc][hvcx] == null || U[vc][hvcx].isEmpty()) {
              this.subtables[vc].setValue(
                hvcx,
                CuckooTable.NULL_XR
              );
            } else {
              this.subtables[vc].setValue(
                hvcx,
                U[vc][hvcx].peek()
              );
            }
          }
        }
      }
      return;
    }
    w = 1 - w;
    while (S.size() != 0) {
      final int x = S.remove();
      final int xL = this.splitL(x);
      final int xR = this.splitR(x);
      for (int a = 0; a != 2; ++a) {
        final int wa = w * 2 + a;
        final int hwax = this.hash(wa, xL, xR);
        if (U[wa][hwax] == null) {
          U[wa][hwax] = new ArrayDeque<Integer>();
        }
        U[wa][hwax].add(xR);
      }
    }
  }
  throw (FailureStatus)
    new FailureStatus(
      "maxDepth exceeded"
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

public final void buildBob(
  final ResultSet xs,
  final int maxDepth
) throws
  SQLException
{
  if (BuildConfig.ENABLE_INVALID_ARGUMENT_CHECKS) {
    if (this.isClosed) {
      throw (InvalidOperationStatus)
        new InvalidOperationStatus(
          "this.isClosed"
        ).initCause(null)
      ;
    }
    if (maxDepth < 1) {
      throw (InvalidArgumentStatus)
        new InvalidArgumentStatus(
        ).initCause(null)
      ;
    }
  }
  if (xs == null) {
    for (int wa = 0; wa != 4; ++wa) {
      for (int hwax = 0; hwax != this.subtableSize; ++hwax) {
        this.subtables[wa].setValue(hwax, CuckooTable.NULL_XR);
      }
    }
    return;
  }
  @SuppressWarnings("unchecked")
  final Queue<Integer>[][] U = new Queue[4][];
  for (int wa = 0; wa != 4; ++wa) {
    @SuppressWarnings("unchecked")
    final Queue<Integer>[] q = new Queue[this.subtableSize];
    U[wa] = q;
  }
  final int columnCount = xs.getMetaData().getColumnCount();
  final Set<Integer> seen = new HashSet<Integer>();
  while (xs.next()) {
    final int x = xs.getInt(columnCount) * 2;
    if (xs.wasNull()) {
      break;
    }
    if (BuildConfig.ENABLE_INVALID_ARGUMENT_CHECKS) {
      if ((x >>> (this.xBits - 1) >>> 1) != 0) {
        throw (InvalidArgumentStatus)
          new InvalidArgumentStatus(
          ).initCause(null)
        ;
      }
      if (seen.contains(x)) {
        throw (InvalidArgumentStatus)
          new InvalidArgumentStatus(
          ).initCause(null)
        ;
      }
      seen.add(x);
    }
    final int xL = this.splitL(x);
    final int xR = this.splitR(x);
    final int w = 0;
    for (int a = 0; a != 2; ++a) {
      final int aw = a * 2 + w;
      final int hawx = this.hash(aw, xL, xR);
      if (U[aw][hawx] == null) {
        U[aw][hawx] = new ArrayDeque<Integer>();
      }
      U[aw][hawx].add(xR);
    }
  }
  this.buildBobMain(U, maxDepth);
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

public final void buildBob(
  final int[] xs,
  final int maxDepth
) {
  if (BuildConfig.ENABLE_INVALID_ARGUMENT_CHECKS) {
    if (this.isClosed) {
      throw (InvalidOperationStatus)
        new InvalidOperationStatus(
          "this.isClosed"
        ).initCause(null)
      ;
    }
    if (maxDepth < 1) {
      throw (InvalidArgumentStatus)
        new InvalidArgumentStatus(
        ).initCause(null)
      ;
    }
  }
  if (xs == null) {
    for (int wa = 0; wa != 4; ++wa) {
      for (int hwax = 0; hwax != this.subtableSize; ++hwax) {
        this.subtables[wa].setValue(hwax, CuckooTable.NULL_XR);
      }
    }
    return;
  }
  @SuppressWarnings("unchecked")
  final Queue<Integer>[][] U = new Queue[4][];
  for (int wa = 0; wa != 4; ++wa) {
    @SuppressWarnings("unchecked")
    final Queue<Integer>[] q = new Queue[this.subtableSize];
    U[wa] = q;
  }
  final Set<Integer> seen = new HashSet<Integer>();
  for (int i = 0; i != xs.length; ++i) {
    final int x = xs[i] * 2;
    if (BuildConfig.ENABLE_INVALID_ARGUMENT_CHECKS) {
      if ((x >>> (this.xBits - 1) >>> 1) != 0) {
        throw (InvalidArgumentStatus)
          new InvalidArgumentStatus(
          ).initCause(null)
        ;
      }
      if (seen.contains(x)) {
        throw (InvalidArgumentStatus)
          new InvalidArgumentStatus(
          ).initCause(null)
        ;
      }
      seen.add(x);
    }
    final int xL = this.splitL(x);
    final int xR = this.splitR(x);
    final int w = 0;
    for (int a = 0; a != 2; ++a) {
      final int aw = a * 2 + w;
      final int hawx = this.hash(aw, xL, xR);
      if (U[aw][hawx] == null) {
        U[aw][hawx] = new ArrayDeque<Integer>();
      }
      U[aw][hawx].add(xR);
    }
  }
  this.buildBobMain(U, maxDepth);
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

private final void buildBobMain(
  final Queue<Integer>[][] U,
  final int maxDepth
) {
  aLoop:
  for (int a = 0; a != 2; ++a) {
    int w = 0;
    final Queue<Integer> S = new ArrayDeque<Integer>();
    for (int depth = 0; depth != maxDepth; ++depth) {
      {
        final int aw = a * 2 + w;
        for (int hawx = 0; hawx != this.subtableSize; ++hawx) {
          if (U[aw][hawx] != null) {
            while (U[aw][hawx].size() > 1) {
              final int xR = U[aw][hawx].remove();
              final int x = this.unhash(aw, hawx, xR);
              S.add(x);
            }
          }
        }
      }
      if (S.isEmpty()) {
        if (a == 0) {
          continue aLoop;
        }
        for (int v = 0; v != 2; ++v) {
          for (int c = 0; c != 2; ++c) {
            final int vc = v * 2 + c;
            for (int hvcx = 0; hvcx != this.subtableSize; ++hvcx) {
              if (U[vc][hvcx] == null || U[vc][hvcx].isEmpty()) {
                this.subtables[vc].setValue(
                  hvcx,
                  CuckooTable.NULL_XR
                );
              } else {
                this.subtables[vc].setValue(
                  hvcx,
                  U[vc][hvcx].peek()
                );
              }
            }
          }
        }
        return;
      }
      w = 1 - w;
      while (S.size() != 0) {
        final int x = S.remove();
        final int xL = this.splitL(x);
        final int xR = this.splitR(x);
        final int aw = a * 2 + w;
        final int hawx = this.hash(aw, xL, xR);
        if (U[aw][hawx] == null) {
          U[aw][hawx] = new ArrayDeque<Integer>();
        }
        U[aw][hawx].add(xR);
      }
    }
    throw (FailureStatus)
      new FailureStatus(
        "maxDepth exceeded"
      ).initCause(null)
    ;
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
    for (int wa = 0; wa != 4; ++wa) {
      this.subtables[wa].close();
    }
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

private final int f(
  final int wa,
  final int xR
) {
  if (BuildConfig.ENABLE_INVALID_ARGUMENT_CHECKS) {
    if (wa < 0) {
      throw (InvalidArgumentStatus)
        new InvalidArgumentStatus(
        ).initCause(null)
      ;
    }
    if (wa > 4) {
      throw (InvalidArgumentStatus)
        new InvalidArgumentStatus(
        ).initCause(null)
      ;
    }
    if ((xR >>> this.rightBits) != 0) {
      throw (InvalidArgumentStatus)
        new InvalidArgumentStatus(
        ).initCause(null)
      ;
    }
  }
  final byte[] a;
  try {
    a =
      this.hashCiphers[wa].doFinal(
        new byte[] {
          0, 0, 0, 0,
          0, 0, 0, 0,
          0, 0, 0, 0,
          (byte)(xR >>> 24),
          (byte)(xR >>> 16),
          (byte)(xR >>>  8),
          (byte)(xR >>>  0)
        }
      )
    ;
  } catch (final BadPaddingException e) {
    throw (InternalErrorStatus)
      new InternalErrorStatus(
      ).initCause(e)
    ;
  } catch (final IllegalBlockSizeException e) {
    throw (InternalErrorStatus)
      new InternalErrorStatus(
      ).initCause(e)
    ;
  }
  final long b =
    ((long)(a[0] & 0xFF) << 24) |
    ((long)(a[1] & 0xFF) << 16) |
    ((long)(a[2] & 0xFF) <<  8) |
    ((long)(a[3] & 0xFF) <<  0)
  ;
  return (int)(b & (this.subtableSize - 1));
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

private final int hash(
  final int wa,
  final int xL,
  final int xR
) {
  if (BuildConfig.ENABLE_INVALID_ARGUMENT_CHECKS) {
    if (wa < 0) {
      throw (InvalidArgumentStatus)
        new InvalidArgumentStatus(
        ).initCause(null)
      ;
    }
    if (wa > 4) {
      throw (InvalidArgumentStatus)
        new InvalidArgumentStatus(
        ).initCause(null)
      ;
    }
    if ((xL >>> this.subtableBits) != 0) {
      throw (InvalidArgumentStatus)
        new InvalidArgumentStatus(
        ).initCause(null)
      ;
    }
    if ((xR >>> this.rightBits) != 0) {
      throw (InvalidArgumentStatus)
        new InvalidArgumentStatus(
        ).initCause(null)
      ;
    }
  }
  return xL ^ this.f(wa, xR);
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

private final int splitL(
  final int x
) {
  if (BuildConfig.ENABLE_INVALID_ARGUMENT_CHECKS) {
    if ((x >>> (this.xBits - 1) >>> 1) != 0) {
      throw (InvalidArgumentStatus)
        new InvalidArgumentStatus(
        ).initCause(null)
      ;
    }
  }
  return x >>> this.rightBits;
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

private final int splitR(
  final int x
) {
  if (BuildConfig.ENABLE_INVALID_ARGUMENT_CHECKS) {
    if ((x >>> (this.xBits - 1) >>> 1) != 0) {
      throw (InvalidArgumentStatus)
        new InvalidArgumentStatus(
        ).initCause(null)
      ;
    }
  }
  return x & ((1 << this.rightBits) - 1);
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

public final int unhash(
  final int wa,
  final int hwax,
  final int xR
) {
  if (BuildConfig.ENABLE_INVALID_ARGUMENT_CHECKS) {
    if (wa < 0) {
      throw (InvalidArgumentStatus)
        new InvalidArgumentStatus(
        ).initCause(null)
      ;
    }
    if (wa > 4) {
      throw (InvalidArgumentStatus)
        new InvalidArgumentStatus(
        ).initCause(null)
      ;
    }
    if ((hwax >>> this.subtableBits) != 0) {
      throw (InvalidArgumentStatus)
        new InvalidArgumentStatus(
        ).initCause(null)
      ;
    }
    if ((xR >>> this.rightBits) != 0) {
      throw (InvalidArgumentStatus)
        new InvalidArgumentStatus(
        ).initCause(null)
      ;
    }
  }
  return ((hwax ^ this.f(wa, xR)) << this.rightBits) | xR;
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

public static boolean testIntersection1(
) {
  int[] a = new int[100000];
  int[] b = new int[50000];
  boolean[] inter = new boolean[25000];

  for (int i = 0; i != 100000; ++i) {
    if (i < 25000) {
      inter[i] = false;
      a[i] = i;
      b[i] = i;
    } else if (i < 50000) {
      a[i] = 2 * i;
      b[i] = 2 * i + 1;
    } else {
      a[i] = 2 * i;
    }
  }
  CuckooTable aTable = new CuckooTable(20, 32);
  CuckooTable bTable = new CuckooTable(20, 32);

  try {
    aTable.buildAlice(a, 100);
  } catch (Exception e) {
    System.out.println("Alice Table Build Failed!");
    System.out.println(e.getMessage());
    throw e;
  }

  try {
    bTable.buildBob(b, 100);
  } catch (Exception e) {
    System.out.println("Bob Table Build Failed!");
    System.out.println(e.getMessage());
    throw e;
  }

  for (int i = 0; i != 4; ++i) {
    for (int j = 0; j != aTable.subtableSize; ++j) {
      if (aTable.subtables[i].getValue(j) == bTable.subtables[i].getValue(j)) {
        int p = aTable.unhash(i, j, aTable.subtables[i].getValue(j));
        if (p < 25000) {
          inter[p] = true;
        } else {
          System.out.println("Invalid Value In Intersection!");
          return false;
        }
      }
    }
  }
  int lastfail = 0;
  int failcount = 0;
  for (int i = 0; i != 25000; ++i) {
    if (inter[i] == false) {
      lastfail = i;
      ++failcount;
    }
  }
  if (failcount > 0) {
   System.out.println("Failed to find " + failcount + " Values In Intersection!");
   System.out.println("Last Missing Value " + lastfail + " In Intersection!");
   return false;
  }

  return true;
}

}
