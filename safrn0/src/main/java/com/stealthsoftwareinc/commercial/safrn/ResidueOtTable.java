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
 * <code>com.stealthsoftwareinc.commercial.safrn.ResidueOtTable</code>
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

public final class ResidueOtTable
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

public final ResidueOtCell[][] cells;

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

public ResidueOtTable(
  final int rowCount,
  final int columnCount,
  final int party,
  final long otCount,
  final BigInteger modulus,
  final boolean useFiles,
  final File filesDirectory
) throws
  IOException
{
  this.cells = new ResidueOtCell[rowCount][];
  for (int i = 0; i != rowCount; ++i) {
    this.cells[i] = new ResidueOtCell[columnCount];
    for (int j = 0; j != columnCount; ++j) {
      this.cells[i][j] =
        new ResidueOtCell(
          party,
          otCount,
          modulus,
          useFiles,
          filesDirectory
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
) {
  this.randomize(
    Rand.defaultSrc()
  );
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
  final Random src
) {
  this.randomize(
    src,
    Rand.defaultMemoryHint()
  );
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
  for (int i = 0; i != this.cells.length; ++i) {
    for (int j = 0; j != this.cells[i].length; ++j) {
      this.cells[i][j].randomize(src, memoryHint);
    }
  }
}

}
