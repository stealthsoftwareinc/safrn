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
 * <code>com.stealthsoftwareinc.commercial.safrn.BackServerGlobals</code>
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

import com.stealthsoftwareinc.commercial.safrn.BackServerRawInitializer;
import com.stealthsoftwareinc.commercial.safrn.Config;
import com.stealthsoftwareinc.commercial.safrn.GmShare;
import com.stealthsoftwareinc.commercial.safrn.RawChannelFactory;
import com.stealthsoftwareinc.commercial.safrn.WfShare;
import com.stealthsoftwareinc.sst.ConcurrentPool;
import com.stealthsoftwareinc.sst.FailureStatus;
import com.stealthsoftwareinc.sst.LimitExceededStatus;
import com.stealthsoftwareinc.sst.NullPointerStatus;
import io.netty.bootstrap.Bootstrap;
import io.netty.channel.ChannelFuture;
import io.netty.channel.ChannelOption;
import io.netty.channel.EventLoopGroup;
import io.netty.channel.socket.nio.NioSocketChannel;
import io.netty.util.concurrent.EventExecutor;
import io.netty.util.concurrent.Future;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintStream;
import java.lang.Double;
import java.lang.Integer;
import java.lang.Long;
import java.lang.Math;
import java.lang.NumberFormatException;
import java.lang.String;
import java.math.BigInteger;
import java.nio.ByteBuffer;
import java.sql.Connection;
import java.util.Arrays;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;
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

public class BackServerGlobals
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
 * @since SAFRN&nbsp;X.X.X (XXXX-XX-XX)
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

public final Map<String, ArrayList<String>> domainNames;

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

public final Map<String, Integer> domainSizes;

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

public final ConcurrentMap<ByteBuffer, GmShare> gmShares;

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
 * @since SAFRN&nbsp;0.1.0 (2017-11-08)
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

public final ConcurrentPool<Future<Connection>> sqlChannels;

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
 * @since SAFRN&nbsp;0.1.0 (2017-11-08)
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

public final ConcurrentMap<ByteBuffer, WfShare> wfShares;

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

public BackServerGlobals(
  final Config config,
  final PrintStream stdout,
  final EventLoopGroup eventLoopGroup,
  final String jdbcUrl,
  final EventExecutor executor
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
  if (eventLoopGroup == null) {
    throw (NullPointerStatus)
      new NullPointerStatus(
        "eventLoopGroup is a null reference"
      ).initCause(null)
    ;
  }
  if (jdbcUrl == null) {
    throw (NullPointerStatus)
      new NullPointerStatus(
        "jdbcUrl is a null reference"
      ).initCause(null)
    ;
  }
  this.config = config;
  // domainNames and domainSizes
  {
    final String s = this.config.get("valid_ivs");
    this.domainNames = new HashMap<String, ArrayList<String>>();
    this.domainSizes = new HashMap<String, Integer>();
    for (final String name : s.split(",", -1)) {
      final String[] domain =
        this.config.get(name.trim() + "_domain_values").split(",", -1)
      ;
      this.domainSizes.put(name.trim(), domain.length);
      for (int i = 0; i != domain.length; ++i) {
        domain[i] = domain[i].trim();
      }
      this.domainNames.put(name.trim(), new ArrayList<String>(Arrays.asList(domain)));
    }
  }
  // gmShares
  {
    this.gmShares =
      new ConcurrentHashMap<ByteBuffer, GmShare>()
    ;
  }
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
  // logConnections
  {
    this.logConnections =
      !this.config.get("log_connections").isEmpty()
    ;
  }
  // modulus
  {
    final String s = this.config.get("modulus");
    this.modulus = new BigInteger(s);
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
    if (s.equals("0")) {
      throw (FailureStatus)
        new FailureStatus(
          "local_party must not be zero for a back server"
        ).initCause(null)
      ;
    }
    try {
      this.localParty = Integer.parseInt(s);
    } catch (final NumberFormatException e) {
      throw (FailureStatus)
        new FailureStatus(
          "local_party must be less than party_count"
        ).initCause(null)
      ;
    }
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
    final Bootstrap rawConnector = new Bootstrap();
    rawConnector.group(eventLoopGroup);
    rawConnector.channel(NioSocketChannel.class);
    rawConnector.handler(
      new BackServerRawInitializer(
        this,
        i
      )
    );
    rawConnector.option(
      ChannelOption.SO_KEEPALIVE,
      true
    );
    rawConnector.option(
      ChannelOption.TCP_NODELAY,
      true
    );
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
  // sqlChannels
  {
    this.sqlChannels =
      new ConcurrentPool<Future<Connection>>(
        new SqlChannelFactory(
          jdbcUrl,
          executor
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
  // wfShares
  {
    this.wfShares =
      new ConcurrentHashMap<ByteBuffer, WfShare>()
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
