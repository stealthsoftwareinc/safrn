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
 * <code>com.stealthsoftwareinc.commercial.safrn.DotProductTable</code>
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

import java.io.IOException;
import java.math.BigInteger;

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

public final class DotProductTable
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

public final DotProductCell[][] cells;

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

public DotProductTable(
  final int rowCount,
  final int columnCount,
  final int groupSizeBound,
  final BigInteger modulus,
  final boolean useFilesForLargeMemory
) throws
  IOException
{
  this.cells = new DotProductCell[rowCount][];
  for (int i = 0; i != rowCount; ++i) {
    this.cells[i] = new DotProductCell[columnCount];
    for (int j = 0; j != columnCount; ++j) {
      this.cells[i][j] =
        new DotProductCell(
          groupSizeBound,
          modulus,
          useFilesForLargeMemory
        )
      ;
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

public final void close(
) {
  for (int i = 0; i != this.cells.length; ++i) {
    for (int j = 0; j != this.cells[i].length; ++j) {
      this.cells[i][j].close();
    }
  }
}

}
