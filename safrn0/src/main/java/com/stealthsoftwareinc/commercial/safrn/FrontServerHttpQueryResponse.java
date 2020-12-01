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
 * <code>com.stealthsoftwareinc.commercial.safrn.FrontServerHttpQueryResponse</code>
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

import com.stealthsoftwareinc.commercial.safrn.FrontServerDv;
import com.stealthsoftwareinc.commercial.safrn.FrontServerIv;
import com.stealthsoftwareinc.sst.AllocationImpossibleStatus;
import io.netty.handler.codec.http.DefaultFullHttpResponse;
import java.lang.Integer;
import java.lang.String;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

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

public final class FrontServerHttpQueryResponse
extends FrontServerHttpResponse
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

public final String analysis;

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

public int countdown;

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

public final int countonlymeanint;

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

public final List<FrontServerDv> dvList;

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

public final List<FrontServerIv> ivList;

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

public final List<List<BigInteger>> sums;

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

public FrontServerHttpQueryResponse(
  final DefaultFullHttpResponse response,
  final int countdown,
  final String analysis,
  final List<FrontServerIv> ivList,
  final List<FrontServerDv> dvList,
  final int countonlymeanint,
  final boolean countColumn
) {
  super(response);
  this.analysis = analysis;
  this.countdown = countdown;
  this.countonlymeanint = countonlymeanint;
  this.dvList =
    Collections.unmodifiableList(
      new ArrayList<FrontServerDv>(dvList)
    )
  ;
  this.ivList =
    Collections.unmodifiableList(
      new ArrayList<FrontServerIv>(ivList)
    )
  ;
  {
    final int rows;
    {
      int r = 1;
      for (final FrontServerIv iv : this.ivList) {
        if (iv.domain.size() > Integer.MAX_VALUE - 1) {
          throw (AllocationImpossibleStatus)
            new AllocationImpossibleStatus(
            ).initCause(null)
          ;
        }
        if (r > Integer.MAX_VALUE / (iv.domain.size() + 1)) {
          throw (AllocationImpossibleStatus)
            new AllocationImpossibleStatus(
            ).initCause(null)
          ;
        }
        r *= iv.domain.size() + 1;
      }
      rows = r;
    }
    if (this.dvList.size() > Integer.MAX_VALUE - 1) {
      throw (AllocationImpossibleStatus)
        new AllocationImpossibleStatus(
        ).initCause(null)
      ;
    }
    final int columns = this.dvList.size() + ((countColumn) ? 1 : 0);
    this.sums = new ArrayList<List<BigInteger>>(rows);
    for (int i = 0; i != rows; ++i) {
      this.sums.add(new ArrayList<BigInteger>(columns));
      for (int j = 0; j != columns; ++j) {
        this.sums.get(i).add(BigInteger.ZERO);
      }
    }
  }
}

}
