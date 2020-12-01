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
 * <code>com.stealthsoftwareinc.commercial.safrn.FrontServerGlobals</code>
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

import com.stealthsoftwareinc.commercial.safrn.Config;
import com.stealthsoftwareinc.commercial.safrn.FrontServerIv;
import com.stealthsoftwareinc.commercial.safrn.RawChannelFactory;
import com.stealthsoftwareinc.sst.ConcurrentPool;
import com.stealthsoftwareinc.sst.FailureStatus;
import com.stealthsoftwareinc.sst.LimitExceededStatus;
import com.stealthsoftwareinc.sst.NullPointerStatus;
import io.netty.bootstrap.Bootstrap;
import io.netty.channel.ChannelFuture;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintStream;
import java.lang.Double;
import java.lang.Integer;
import java.lang.Long;
import java.lang.Math;
import java.lang.NumberFormatException;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.concurrent.Executor;
import java.util.concurrent.ForkJoinPool;

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

public class FrontServerGlobals
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

public final Config config;

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

public final int groupSizeBound;

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

public final double hashSlop;

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

public final BigInteger incomeSizeBound;

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

public final int localParty;

/**
 *
 */

public final boolean logConnections;

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

public final BigInteger modulus;

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

public final int partyCount;

private final PrintStream performanceLog;
private final Executor logExecutor = new ForkJoinPool(1);

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

public final ConcurrentPool<ChannelFuture>[] rawChannels;

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

public final int rightBits;

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

public final int subtableBits;

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

public final BigInteger subtableSize;

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

public final File tmpdir;

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

public final boolean useFilesForLargeMemory;

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

public final int xBits;

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

public FrontServerGlobals(
  final Config config,
  final PrintStream stdout,
  final Bootstrap rawConnector
) throws
  IOException
{
  if (config == null) {
    throw (NullPointerStatus)
      new NullPointerStatus(
        "config is a null reference"
      ).initCause(null)
    ;
  }
  if (stdout == null) {
    throw (NullPointerStatus)
      new NullPointerStatus(
        "stdout is a null reference"
      ).initCause(null)
    ;
  }
  if (rawConnector == null) {
    throw (NullPointerStatus)
      new NullPointerStatus(
        "rawConnector is a null reference"
      ).initCause(null)
    ;
  }
  this.config = config;
  // groupSizeBound
  {
    final String s = this.config.get("group_size_bound");
    final BigInteger n = new BigInteger(s);
    if (n.compareTo(BigInteger.valueOf(Integer.MAX_VALUE)) > 0) {
      throw (LimitExceededStatus)
        new LimitExceededStatus(
        ).initCause(null)
      ;
    }
    this.groupSizeBound = n.intValue();
  }
  // hashSlop
  {
    final String s = this.config.get("hash_slop");
    this.hashSlop = Double.parseDouble(s);
  }
  // incomeSizeBound
  {
    final String s = this.config.get("income_size_bound");
    this.incomeSizeBound = new BigInteger(s);
  }
  // modulus
  {
    final String s = this.config.get("modulus");
    this.modulus = new BigInteger(s);
  }
  // dvList
  {
    final String s = this.config.get("valid_dvs");
    this.dvList = new ArrayList<FrontServerDv>();
    for (final String name : s.split(",", -1)) {
      this.dvList.add(
        new FrontServerDv(
          name.trim()
        )
      );
    }
  }
  // ivList
  {
    final String s = this.config.get("valid_ivs");
    this.ivList = new ArrayList<FrontServerIv>();
    for (final String name : s.split(",", -1)) {
      final String[] domain =
        this.config.get(name.trim() + "_domain_values").split(",", -1)
      ;
      for (int i = 0; i != domain.length; ++i) {
        domain[i] = domain[i].trim();
      }
      this.ivList.add(
        new FrontServerIv(
          name.trim(),
          Arrays.asList(domain),
          !this.config.get(name.trim() + "_domain_quote").equals("false")
        )
      );
    }
  }
  // logConnections
  {
    this.logConnections =
      !this.config.get("log_connections").isEmpty()
    ;
  }
  {
    final String s = this.config.get("party_count");
    try {
      this.partyCount = Integer.parseInt(s);
    } catch (final NumberFormatException e) {
      throw (LimitExceededStatus)
        new LimitExceededStatus(
        ).initCause(e)
      ;
    }
  }
  // performanceLog
  {
    final String s = this.config.get("performance_log_file");
    if (s.equals("")) {
      this.performanceLog = null;
    } else if (s.equals("-")) {
      this.performanceLog = stdout;
    } else {
      this.performanceLog =
        new PrintStream(new FileOutputStream(s, true), true, "UTF-8")
      ;
    }
  }
  {
    final String s = this.config.get("local_party");
    if (!s.equals("0")) {
      throw (FailureStatus)
        new FailureStatus(
          "local_party must be zero for the front server"
        ).initCause(null)
      ;
    }
    this.localParty = 0;
  }
  {
    @SuppressWarnings("unchecked")
    final ConcurrentPool<ChannelFuture>[] rawChannels =
      (ConcurrentPool<ChannelFuture>[])
      new ConcurrentPool[this.partyCount]
    ;
    this.rawChannels = rawChannels;
  }
  for (int i = this.localParty + 1; i != this.partyCount; ++i) {
    this.rawChannels[i] =
      new ConcurrentPool<ChannelFuture>(
        new RawChannelFactory(
          rawConnector,
          this.config.get("raw_connect_host_" + i),
          Integer.parseInt(this.config.get("raw_connect_port_" + i))
        )
      )
    ;
  }
  // subtableSize
  {
    final long k = 1000000000;
    BigInteger n =
      this.incomeSizeBound.multiply(
        BigInteger.valueOf(Math.round(this.hashSlop * k))
      ).divide(
        BigInteger.valueOf(k)
      )
    ;
    if (n.and(n.subtract(BigInteger.ONE)).signum() != 0) {
      do {
        n = n.and(n.subtract(BigInteger.ONE));
      } while (n.and(n.subtract(BigInteger.ONE)).signum() != 0);
      n = n.multiply(BigInteger.valueOf(2));
    }
    this.subtableSize = n;
  }
  // subtableBits
  {
    this.subtableBits = this.subtableSize.bitLength() - 1;
  }
  // tmpdir
  {
    final String s = this.config.get("tmpdir");
    if (s.isEmpty()) {
      this.tmpdir = null;
    } else {
      this.tmpdir = new File(s);
    }
  }
  // useFilesForLargeMemory
  {
    this.useFilesForLargeMemory =
      this.config.get("use_files_for_large_memory").equals("true")
    ;
  }
  // xBits
  {
    this.xBits = 31;
  }
  // rightBits
  {
    this.rightBits = this.xBits - this.subtableBits;
    if (this.rightBits <= 0) {
      throw (FailureStatus)
        new FailureStatus(
        ).initCause(null)
      ;
    }
  }
}

public final void log(
  final CharSequence message
) {
  if (performanceLog != null && message != null) {
    logExecutor.execute(() -> {
      synchronized (performanceLog) {
        performanceLog.println(message.toString());
        performanceLog.flush();
      }
    });
  }
}

}
