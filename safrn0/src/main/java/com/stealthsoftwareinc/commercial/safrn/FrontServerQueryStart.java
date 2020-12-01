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
 * <code>com.stealthsoftwareinc.commercial.safrn.FrontServerQueryStart</code>
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

import com.stealthsoftwareinc.commercial.safrn.BlobOtTable;
import com.stealthsoftwareinc.commercial.safrn.DotProductTable;
import com.stealthsoftwareinc.commercial.safrn.FrontServerDv;
import com.stealthsoftwareinc.commercial.safrn.FrontServerIv;
import com.stealthsoftwareinc.commercial.safrn.FrontServerQueryDone;
import com.stealthsoftwareinc.commercial.safrn.ResidueOtTable;
import com.stealthsoftwareinc.sst.PoolEntry;
import io.netty.channel.ChannelFuture;
import io.netty.channel.ChannelPipeline;
import java.lang.String;
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

public final class FrontServerQueryStart
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
 * @since SAFRN&nbsp;X.X.X (XXXX-XX-XX)
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

public final ResidueOtTable gsOt;

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

public final BlobOtTable guOt;

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

public final ResidueOtTable gwOt;

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

public final ChannelPipeline httpPipeline;

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

public final int index;

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

public final ResidueOtTable[] isOts;

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

public final BlobOtTable[] iuOts;

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
 * @since SAFRN&nbsp;X.X.X (XXXX-XX-XX)
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

public final ResidueOtTable[] iwOts;

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

public final boolean loanlinkbool;

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

public final FrontServerQueryDone queryDone;

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

public final byte[] queryId;

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

public final PoolEntry<ChannelFuture> rawChannel;

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

public final int remoteParty;

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

public FrontServerQueryStart(
  final String analysis,
  final List<FrontServerIv> ivList,
  final List<FrontServerDv> dvList,
  final byte[] queryId,
  final boolean loanlinkbool,
  final int countonlymeanint,
  final int remoteParty,
  final FrontServerQueryDone queryDone,
  final PoolEntry<ChannelFuture> rawChannel,
  final ChannelPipeline httpPipeline,
  final int index,
  final ResidueOtTable[] isOts,
  final ResidueOtTable gsOt,
  final BlobOtTable[] iuOts,
  final BlobOtTable guOt,
  final ResidueOtTable[] iwOts,
  final ResidueOtTable gwOt
) {
  this.analysis = analysis;
  this.countonlymeanint = countonlymeanint;
  this.dvList =
    Collections.unmodifiableList(
      new ArrayList<FrontServerDv>(dvList)
    )
  ;
  this.gsOt = gsOt;
  this.guOt = guOt;
  this.gwOt = gwOt;
  this.httpPipeline = httpPipeline;
  this.index = index;
  this.isOts = isOts;
  this.iuOts = iuOts;
  this.ivList =
    Collections.unmodifiableList(
      new ArrayList<FrontServerIv>(ivList)
    )
  ;
  this.iwOts = iwOts;
  this.loanlinkbool = loanlinkbool;
  this.queryDone = queryDone;
  this.queryId = queryId;
  this.rawChannel = rawChannel;
  this.remoteParty = remoteParty;
}

}
