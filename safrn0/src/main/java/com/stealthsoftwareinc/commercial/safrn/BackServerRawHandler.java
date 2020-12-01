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
 * <code>com.stealthsoftwareinc.commercial.safrn.BackServerRawHandler</code>
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

import com.stealthsoftwareinc.commercial.safrn.BackServerGlobals;
import com.stealthsoftwareinc.commercial.safrn.BackServerRawState;
import com.stealthsoftwareinc.commercial.safrn.BlobOtCell;
import com.stealthsoftwareinc.commercial.safrn.BlobOtTable;
import com.stealthsoftwareinc.commercial.safrn.BuildConfig;
import com.stealthsoftwareinc.commercial.safrn.DotProductTable;
import com.stealthsoftwareinc.commercial.safrn.GfShare;
import com.stealthsoftwareinc.commercial.safrn.GmShare;
import com.stealthsoftwareinc.commercial.safrn.MessageType;
import com.stealthsoftwareinc.commercial.safrn.QueryBuilder;
import com.stealthsoftwareinc.commercial.safrn.RawUtil;
import com.stealthsoftwareinc.commercial.safrn.ResidueOtTable;
import com.stealthsoftwareinc.commercial.safrn.WfShare;
import com.stealthsoftwareinc.sst.BigBitArray;
import com.stealthsoftwareinc.sst.BigByteArray;
import com.stealthsoftwareinc.sst.BigResidueArray;
import com.stealthsoftwareinc.sst.InternalErrorStatus;
import com.stealthsoftwareinc.sst.JdbcSubprotocol;
import com.stealthsoftwareinc.sst.LimitExceededStatus;
import com.stealthsoftwareinc.sst.Memory;
import com.stealthsoftwareinc.sst.MissingCaseStatus;
import com.stealthsoftwareinc.sst.NotImplementedStatus;
import com.stealthsoftwareinc.sst.NullPointerStatus;
import com.stealthsoftwareinc.sst.ParseFailureStatus;
import com.stealthsoftwareinc.sst.PoolEntry;
import com.stealthsoftwareinc.sst.Rand;
import io.netty.buffer.ByteBuf;
import io.netty.buffer.Unpooled;
import io.netty.channel.ChannelFuture;
import io.netty.channel.ChannelFutureListener;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.ChannelInboundHandlerAdapter;
import io.netty.channel.ChannelPipeline;
import io.netty.util.ReferenceCountUtil;
import io.netty.util.concurrent.Future;
import io.netty.util.concurrent.GenericFutureListener;
import io.netty.util.concurrent.Promise;
import java.io.PrintStream;
import java.lang.Exception;
import java.lang.Integer;
import java.lang.Object;
import java.lang.System;
import java.lang.Throwable;
import java.math.BigInteger;
import java.nio.ByteBuffer;
import java.security.SecureRandom;
import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.Statement;
import java.time.LocalTime;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Map;
import java.util.Random;
import java.util.concurrent.atomic.AtomicInteger;

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

public final class BackServerRawHandler
extends ChannelInboundHandlerAdapter
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

private BigByteArray[] C_S;

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

private BigByteArray[] C_U;

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

private BigBitArray[] PSIShare_S;

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

private BigBitArray[] PSIShare_U;

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

private static final Object WAKE_UP;

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

private String analysis;

/**
 * A running count of the number of bytes received.
 * <p>
 * This variable is initialized to zero and is incremented by
 * <code>in.readableBytes()</code>
 * each time the
 * <code>{@link com.stealthsoftwareinc.commercial.safrn.BackServerRawHandler.channelRead(ChannelHandlerContext, Object)}</code>
 * method is called, keeping a running count of the number of bytes
 * received over the lifetime of this handler.
 * To determine the number of bytes received between two points in the
 * <code>{@link com.stealthsoftwareinc.commercial.safrn.BackServerRawHandler.handle(ChannelHandlerContext, ByteBuf)}</code>
 * method, add a field to this class to hold the count, say
 * <code>myCount</code>,
 * and use the following code:
 * </p>
 *<!--code{.java}--><blockquote><pre>
 *<b></b>// start counting
 *this.myCount = in.readableBytes() - this.bytesReceived;
 *
 *<b></b>// stop counting
 *this.myCount += this.bytesReceived - in.readableBytes();
 *</pre></blockquote>
 *
 * @since SAFRN&nbsp;0.1.0 (2017-11-08)
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

private long bytesReceived;

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

private long bytesSent;

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

private int clip;

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

private int columnIndex;

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

private BigBitArray[] cselbits;

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

private ByteBuffer cselbitsBuffer;

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

private int cselbitsBufferIndex;

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

private int cselbitsRowIndex;

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

private BigByteArray[] csendsec;

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

private ByteBuffer csendsecBuffer;

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

private int csendsecBufferIndex;

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

private int csendsecRowIndex;

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

private CuckooTable[] cuckooTable;

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

private List<String> dvList;

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

private List<Map<Integer, Integer>> dvMaps;

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

private GfShare gfShare;

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

private final BackServerGlobals globals;

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

private GmShare gmShare;

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

private ResidueOtTable gsOt;

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

private ByteBuffer gsOtBBuffer;

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

private int gsOtBBufferIndex;

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

private int gsOtColumnIndex;

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

private int gsOtRowIndex;

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

private ByteBuffer gsOtUBuffer;

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

private int gsOtUBufferIndex;

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

private BlobOtTable guOt;

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

private ByteBuffer guOtBBuffer;

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

private int guOtBBufferIndex;

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

private int guOtColumnIndex;

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

private int guOtRowIndex;

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

private ByteBuffer guOtUBuffer;

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

private int guOtUBufferIndex;

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

private ResidueOtTable gwOt;

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

private ByteBuffer gwOtBBuffer;

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

private int gwOtBBufferIndex;

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

private int gwOtColumnIndex;

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

private int gwOtRowIndex;

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

private ByteBuffer gwOtUBuffer;

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

private int gwOtUBufferIndex;

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

private AtomicInteger iaCountdown;

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

private ChannelPipeline iaPipeline;

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

private boolean ignoreEverything;

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

private ResidueOtTable[] isOts;

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

private int isOtsColumnIndex;

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

private int isOtsPartyIndex;

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

private int isOtsRowIndex;

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

private ByteBuffer isOtsSBuffer;

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

private int isOtsSBufferIndex;

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

private BlobOtTable[] iuOts;

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

private int iuOtsColumnIndex;

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

private int iuOtsPartyIndex;

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

private int iuOtsRowIndex;

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

private ByteBuffer iuOtsSBuffer;

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

private int iuOtsSBufferIndex;

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

private List<String> ivList;

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

private ResidueOtTable[] iwOts;

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

private int iwOtsColumnIndex;

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

private int iwOtsPartyIndex;

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

private int iwOtsRowIndex;

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

private ByteBuffer iwOtsSBuffer;

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

private int iwOtsSBufferIndex;

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

private boolean loanlinkbool;

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

private BigResidueArray[][] maskedv;

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

private ByteBuffer maskedvBuffer;

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

private int maskedvBufferIndex;

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

private int maskedvColumnIndex;

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

private int maskedvRowIndex;

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

private PoolEntry<ChannelFuture> myChannel;

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

private int partyIndex;

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

private long queryBytesReceived;

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

private long queryBytesSent;

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

private ByteBuffer queryId;

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

private long queryNanoseconds;

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

private int remoteParty;

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

private int rowIndex;

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

private BigBitArray[] selbits;

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

private ByteBuffer selbitsBuffer;

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

private int selbitsBufferIndex;

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

private int selbitsRowIndex;

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

private BigByteArray[] sendsec;

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

private ByteBuffer sendsecBuffer;

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

private int sendsecBufferIndex;

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

private int sendsecRowIndex;

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

private ByteBuf sponge1;

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

private ByteBuf sponge2;

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

private PoolEntry<Future<Connection>> sqlChannel;

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

private ResultSet sqlResult;

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

private Statement sqlStatement;

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

private BackServerRawState state;

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

private int vectorIndex;

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

private WfShare wfShare;

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

private List<BigInteger> wgColumn;

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

private BigBitArray[][] yselbits;

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

private ByteBuffer yselbitsBuffer;

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

private int yselbitsBufferIndex;

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

private int yselbitsColumnIndex;

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

private int yselbitsRowIndex;

static {
  WAKE_UP = new Object();
}

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

public BackServerRawHandler(
  final BackServerGlobals globals,
  final int remoteParty
) {
  if (globals == null) {
    throw (NullPointerStatus)
      new NullPointerStatus(
        "globals is a null reference"
      ).initCause(null)
    ;
  }
  this.C_S = null;
  this.C_U = null;
  this.PSIShare_S = null;
  this.PSIShare_U = null;
  this.analysis = null;
  this.bytesReceived = 0;
  this.bytesSent = 0;
  this.clip = 0;
  this.columnIndex = 0;
  this.cselbits = null;
  this.cselbitsBuffer = null;
  this.cselbitsBufferIndex = 0;
  this.cselbitsRowIndex = 0;
  this.csendsec = null;
  this.csendsecBuffer = null;
  this.csendsecBufferIndex = 0;
  this.csendsecRowIndex = 0;
  this.cuckooTable = null;
  this.dvList = null;
  this.dvMaps = null;
  this.gfShare = null;
  this.globals = globals;
  this.gmShare = null;
  this.gsOt = null;
  this.gsOtBBuffer = null;
  this.gsOtBBufferIndex = 0;
  this.gsOtColumnIndex = 0;
  this.gsOtRowIndex = 0;
  this.gsOtUBuffer = null;
  this.gsOtUBufferIndex = 0;
  this.guOt = null;
  this.guOtBBuffer = null;
  this.guOtBBufferIndex = 0;
  this.guOtColumnIndex = 0;
  this.guOtRowIndex = 0;
  this.guOtUBuffer = null;
  this.guOtUBufferIndex = 0;
  this.gwOt = null;
  this.gwOtBBuffer = null;
  this.gwOtBBufferIndex = 0;
  this.gwOtColumnIndex = 0;
  this.gwOtRowIndex = 0;
  this.gwOtUBuffer = null;
  this.gwOtUBufferIndex = 0;
  this.iaCountdown = null;
  this.iaPipeline = null;
  this.ignoreEverything = false;
  this.isOts = null;
  this.isOtsColumnIndex = 0;
  this.isOtsPartyIndex = 0;
  this.isOtsRowIndex = 0;
  this.isOtsSBuffer = null;
  this.isOtsSBufferIndex = 0;
  this.iuOts = null;
  this.iuOtsColumnIndex = 0;
  this.iuOtsPartyIndex = 0;
  this.iuOtsRowIndex = 0;
  this.iuOtsSBuffer = null;
  this.iuOtsSBufferIndex = 0;
  this.ivList = null;
  this.iwOts = null;
  this.iwOtsColumnIndex = 0;
  this.iwOtsPartyIndex = 0;
  this.iwOtsRowIndex = 0;
  this.iwOtsSBuffer = null;
  this.iwOtsSBufferIndex = 0;
  this.loanlinkbool = false;
  this.maskedv = null;
  this.maskedvBuffer = null;
  this.maskedvBufferIndex = 0;
  this.maskedvColumnIndex = 0;
  this.maskedvRowIndex = 0;
  this.myChannel = null;
  this.partyIndex = 0;
  this.queryBytesReceived = 0;
  this.queryBytesSent = 0;
  this.queryId = null;
  this.queryNanoseconds = 0;
  this.remoteParty = remoteParty;
  this.rowIndex = 0;
  this.selbits = null;
  this.selbitsBuffer = null;
  this.selbitsBufferIndex = 0;
  this.selbitsRowIndex = 0;
  this.sendsec = null;
  this.sendsecBuffer = null;
  this.sendsecBufferIndex = 0;
  this.sendsecRowIndex = 0;
  this.sponge1 = Unpooled.buffer();
  this.sponge2 = Unpooled.buffer();
  this.sqlChannel = null;
  this.sqlResult = null;
  this.sqlStatement = null;
  if (this.globals.localParty == 1) {
    setState(BackServerRawState.I_0);
  } else if (this.globals.localParty != this.globals.partyCount - 1) {
    setState(BackServerRawState.G_0);
  } else {
    setState(BackServerRawState.W_0);
  }
  this.vectorIndex = 0;
  this.wfShare = null;
  this.wgColumn = null;
  this.yselbits = null;
  this.yselbitsBuffer = null;
  this.yselbitsBufferIndex = 0;
  this.yselbitsColumnIndex = 0;
  this.yselbitsRowIndex = 0;
}

private void log(
  final CharSequence message
) {
  if (message != null) {
    globals.log(
      "localParty = " +
      globals.localParty +
      ", " +
      "remoteParty = " +
      remoteParty +
      ": " +
      message.toString()
    );
  }
}

private void setState(
  final BackServerRawState newState
) {
  state = newState;
  log(state.name());
}

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

@Override
public final void channelRead(
  final ChannelHandlerContext ctx,
  final Object msg
) throws
  Exception
{
  final ByteBuf in = (ByteBuf)msg;
  try {
    this.bytesReceived += in.readableBytes();
    if (this.sponge1.readerIndex() > 65536) {
      this.sponge2.clear();
      this.sponge2.writeBytes(this.sponge1);
      final ByteBuf t = this.sponge1;
      this.sponge1 = this.sponge2;
      this.sponge2 = t;
    }
    this.sponge1.writeBytes(in);
    this.handle(ctx, this.sponge1);
  } catch (final Exception e1) {
    try {
      ReferenceCountUtil.release(in);
    } catch (final Exception e2) {
    }
    throw e1;
  }
  ReferenceCountUtil.release(in);
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

private final void cleanup(
) {
  if (this.C_S != null) {
    for (int i = 0; i != this.C_S.length; ++i) {
      if (this.C_S[i] != null) {
        try {
          this.C_S[i].close();
        } catch (final Exception e) {
        }
      }
    }
    this.C_S = null;
  }
  if (this.C_U != null) {
    for (int i = 0; i != this.C_U.length; ++i) {
      if (this.C_U[i] != null) {
        try {
          this.C_U[i].close();
        } catch (final Exception e) {
        }
      }
    }
    this.C_U = null;
  }
  if (this.PSIShare_S != null) {
    for (int i = 0; i != this.PSIShare_S.length; ++i) {
      if (this.PSIShare_S[i] != null) {
        try {
          this.PSIShare_S[i].close();
        } catch (final Exception e) {
        }
      }
    }
    this.PSIShare_S = null;
  }
  if (this.PSIShare_U != null) {
    for (int i = 0; i != this.PSIShare_U.length; ++i) {
      if (this.PSIShare_U[i] != null) {
        try {
          this.PSIShare_U[i].close();
        } catch (final Exception e) {
        }
      }
    }
    this.PSIShare_U = null;
  }
  if (this.cselbits != null) {
    for (int i = 0; i != this.cselbits.length; ++i) {
      if (this.cselbits[i] != null) {
        try {
          this.cselbits[i].close();
        } catch (final Exception e) {
        }
      }
    }
    this.cselbits = null;
  }
  if (this.csendsec != null) {
    for (int i = 0; i != this.csendsec.length; ++i) {
      if (this.csendsec[i] != null) {
        try {
          this.csendsec[i].close();
        } catch (final Exception e) {
        }
      }
    }
    this.csendsec = null;
  }
  if (this.cuckooTable != null) {
    for (int i = 0; i != this.cuckooTable.length; ++i) {
      if (this.cuckooTable[i] != null) {
        try {
          this.cuckooTable[i].close();
        } catch (final Exception e) {
        }
      }
    }
    this.cuckooTable = null;
  }
  if (this.dvMaps != null) {
    this.dvMaps = null;
  }
  if (this.gsOt != null) {
    try {
      this.gsOt.close();
    } catch (final Exception e) {
    }
    this.gsOt = null;
  }
  if (this.guOt != null) {
    try {
      this.guOt.close();
    } catch (final Exception e) {
    }
    this.guOt = null;
  }
  if (this.gwOt != null) {
    try {
      this.gwOt.close();
    } catch (final Exception e) {
    }
    this.gwOt = null;
  }
  if (this.globals.localParty == 1 && this.remoteParty == 0) {
    if (this.isOts != null) {
      for (int i = 0; i != this.isOts.length; ++i) {
        if (this.isOts[i] != null) {
          try {
            this.isOts[i].close();
          } catch (final Exception e) {
          }
        }
      }
      this.isOts = null;
    }
  }
  if (this.globals.localParty == 1 && this.remoteParty == 0) {
    if (this.iuOts != null) {
      for (int i = 0; i != this.iuOts.length; ++i) {
        if (this.iuOts[i] != null) {
          try {
            this.iuOts[i].close();
          } catch (final Exception e) {
          }
        }
      }
      this.iuOts = null;
    }
  }
  if (this.globals.localParty == 1 && this.remoteParty == 0) {
    if (this.iwOts != null) {
      for (int i = 0; i != this.iwOts.length; ++i) {
        if (this.iwOts[i] != null) {
          try {
            this.iwOts[i].close();
          } catch (final Exception e) {
          }
        }
      }
      this.iwOts = null;
    }
  }
  if (this.maskedv != null) {
    for (int i = 0; i != this.maskedv.length; ++i) {
      if (this.maskedv[i] != null) {
        for (int j = 0; j != this.maskedv[i].length; ++j) {
          if (this.maskedv[i][j] != null) {
            try {
              this.maskedv[i][j].close();
            } catch (final Exception e) {
            }
          }
        }
      }
    }
    this.maskedv = null;
  }
  if (this.selbits != null) {
    for (int i = 0; i != this.selbits.length; ++i) {
      if (this.selbits[i] != null) {
        try {
          this.selbits[i].close();
        } catch (final Exception e) {
        }
      }
    }
    this.selbits = null;
  }
  if (this.sendsec != null) {
    for (int i = 0; i != this.sendsec.length; ++i) {
      if (this.sendsec[i] != null) {
        try {
          this.sendsec[i].close();
        } catch (final Exception e) {
        }
      }
    }
    this.sendsec = null;
  }
  if (this.yselbits != null) {
    for (int i = 0; i != this.yselbits.length; ++i) {
      if (this.yselbits[i] != null) {
        for (int j = 0; j != this.yselbits[i].length; ++j) {
          if (this.yselbits[i][j] != null) {
            try {
              this.yselbits[i][j].close();
            } catch (final Exception e) {
            }
          }
        }
      }
    }
    this.yselbits = null;
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

private static byte decodegate(
  final int seclen,
  final byte[] wsec
) {
  final int b = (wsec[0] >> seclen) & 1;
  final int R = wsec[0] & ((1 << seclen) - 1);
  return (byte)(R ^ ((wsec[1] >> ((b ^ 1) * seclen)) & ((1 << seclen) - 1)));
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

private static void encodegate(
  final int seclen,
  final int[] zsec,
  final int[] osec,
  int b,
  int R0,
  int R1
) {
  b &= 1;
  R0 &= (1 << seclen) - 1;
  R1 &= (1 << seclen) - 1;
  osec[0] = ((b ^ 0) << seclen) | R0;
  osec[1] = ((b ^ 1) << seclen) | R1;
  osec[2] =
    (zsec[0] ^ R0) << ((b ^ 1) * seclen) |
    (zsec[2] ^ R1) << ((b ^ 0) * seclen)
  ;
  osec[3] =
    (zsec[1] ^ R0) << ((b ^ 1) * seclen) |
    (zsec[3] ^ R1) << ((b ^ 0) * seclen)
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

private static void evaluateFullEqualityCircuits(
  final BigByteArray insec,
  final BigBitArray output,
  final long m
) {
  final int[] seclens =
    new int[] {
      3, 4, 4, 6, 4, 2, 3, 2, 1
    }
  ;
  final int[][] wiring =
    new int[][] {
      new int[] {0, 0},
      new int[] {1, 1},
      new int[] {2, 2},
      new int[] {3, 3},
      new int[] {4, 4},
      new int[] {0, 1},
      new int[] {2, 3},
      new int[] {6, 4},
      new int[] {5, 7},
    }
  ;
  final byte[] equalitytable = new byte[9];
  final byte[] wsec = new byte[2];
  for (long i = 0; i != m; ++i) {
    for (int j = 0; j != 5; ++j) {
      equalitytable[j] = insec.getValue(i * 5 + j);
    }
    for (int j = 5; j != 9; ++j) {
      wsec[0] = equalitytable[wiring[j][0]];
      wsec[1] = equalitytable[wiring[j][1]];
      equalitytable[j] = decodegate(seclens[j], wsec);
    }
    output.setValue(i, equalitytable[8] & 1);
  }
}

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

@Override
public final void exceptionCaught(
  final ChannelHandlerContext ctx,
  final Throwable cause
) {
  this.cleanup();
  this.ignoreEverything = true;
  ctx.fireExceptionCaught(cause);
}

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

private final void finishQueryLogEntry(
  final int readableBytes
) {
  this.queryBytesReceived += this.bytesReceived - readableBytes;
  this.queryBytesSent += this.bytesSent;
  this.queryNanoseconds += System.nanoTime();
  String queryString = "analysis=" + this.analysis;
  final int ivCount = this.ivList.size();
  if (ivCount != 0) {
    queryString += "&iv=";
    for (int i = 0; i != ivCount; ++i) {
      queryString += this.ivList.get(i);
      if (i != ivCount - 1) {
        queryString += "+";
      }
    }
  }
  final int dvCount = this.dvList.size();
  if (dvCount != 0) {
    queryString += "&dv=";
    for (int i = 0; i != dvCount; ++i) {
      queryString += this.dvList.get(i);
      if (i != dvCount - 1) {
        queryString += "+";
      }
    }
  }
  globals.log(
    "/query?" + queryString + ", " +
    "queryId = " + Memory.toHexString(this.queryId.array()) + ", " +
    "incomeSizeBound = " + this.globals.incomeSizeBound + ", " +
    "localParty = " + this.globals.localParty + ", " +
    "remoteParty = " + this.remoteParty + ", " +
    "queryBytesReceived = " + this.queryBytesReceived + ", " +
    "queryBytesSent = " + this.queryBytesSent + ", " +
    "queryNanoseconds = " + this.queryNanoseconds
  );
}

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

private final void getGmShare(
) {
  final GmShare s1 = this.globals.gmShares.get(this.queryId);
  if (s1 != null) {
    this.gmShare = s1;
    return;
  }
  final GmShare s2 =
    new GmShare(
    )
  ;
  final GmShare s3 =
    this.globals.gmShares.putIfAbsent(this.queryId, s2)
  ;
  if (s3 == null) {
    this.gmShare = s2;
    return;
  }
  this.gmShare = s3;
}

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

private final int getRowCount(
) {
  int rows = 1;
  for (final String iv : this.ivList) {
    rows *= this.globals.domainSizes.get(iv) + 1;
  }
  return rows;
}

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

private final void getSqlChannel(
  final ChannelHandlerContext ctx
) {
  this.sqlChannel = this.globals.sqlChannels.acquire();
  this.sqlChannel.object.addListener(
    new GenericFutureListener<Future<Connection>>() {
      @Override
      public final void operationComplete(
        final Future<Connection> future
      ) throws
        Exception
      {
        if (!future.isSuccess()) {
          throw (Exception)future.cause();
        }
        ctx.channel().pipeline().fireUserEventTriggered(
          BackServerRawHandler.WAKE_UP
        );
      }
    }
  );
}

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

private final void getSqlResultSet(
  final ChannelHandlerContext ctx,
  final String query
) throws
  Exception
{
  this.sqlStatement =
    this.sqlChannel.object.syncUninterruptibly().get().createStatement()
  ;
  final int queryId = System.identityHashCode(query);
  ctx.executor().execute(
    new Runnable() {
      @Override
      public final void run(
      ) {
        try {
          BackServerRawHandler.this.log(
            "SQL query " + queryId + " running: " + query
          );
          BackServerRawHandler.this.sqlResult =
            BackServerRawHandler.this.sqlStatement.executeQuery(
              query
            )
          ;
          BackServerRawHandler.this.log(
            "SQL query " + queryId + " completed"
          );
          ctx.channel().pipeline().fireUserEventTriggered(
            BackServerRawHandler.WAKE_UP
          );
        } catch (final Exception e) {
          ctx.channel().pipeline().fireExceptionCaught(e);
        }
      }
    }
  );
}

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

private final void getWfShare(
) {
  final WfShare s1 = this.globals.wfShares.get(this.queryId);
  if (s1 != null) {
    this.wfShare = s1;
    return;
  }
  final WfShare s2 =
    new WfShare(
      this.globals.partyCount - 2,
      this.getRowCount()
    )
  ;
  final WfShare s3 =
    this.globals.wfShares.putIfAbsent(this.queryId, s2)
  ;
  if (s3 == null) {
    this.wfShare = s2;
    return;
  }
  this.wfShare = s3;
}

private static void expect(
  final boolean x
) {
  if (!x) {
    throw new ParseFailureStatus();
  }
}

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

private final void handle(
  final ChannelHandlerContext ctx,
  final ByteBuf in
) throws
  Exception
{
  if (this.ignoreEverything) {
    in.skipBytes(in.readableBytes());
    return;
  }
  globals.log(LocalTime.now()+Constants.DELIMETER+Constants.LATENCY+Constants.DELIMETER+this.getClass().getSimpleName()+Constants.DELIMETER+"handle()"+Constants.DELIMETER+
		  this.state.name() +Constants.DELIMETER+"Id()="+Thread.currentThread().getId());
  for (;;) {
	globals.log(LocalTime.now()+Constants.DELIMETER+Constants.LATENCY+Constants.DELIMETER+this.getClass().getSimpleName()+Constants.DELIMETER+"inside for loop"+Constants.DELIMETER+this.state.name() +Constants.DELIMETER+"Id()="+Thread.currentThread().getId()
			+Constants.DELIMETER+"localparty="+this.globals.localParty+Constants.DELIMETER+"remoteparty="+this.remoteParty+Constants.DELIMETER+"tsBackServerRawHandler="+System.currentTimeMillis());  
	switch (this.state) {
      case I_0: if (true) {
        if (this.remoteParty == -1) {
          if (!this.readRemoteParty(in)) {
            return;
          }
          if (this.remoteParty != 0) {
            throw (ParseFailureStatus)
              new ParseFailureStatus(
              ).initCause(null)
            ;
          }
          setState(BackServerRawState.I_A_0);
        } else {
          final ByteBuf out = ctx.alloc().buffer();
          out.writeInt(MessageType.ID);
          out.writeInt(this.globals.localParty);
          this.writeAndFlush(ctx, out);
          setState(BackServerRawState.I_G_0);
        }
      } break;
      case I_A_0: if (true) {
        if (!in.isReadable(4)) {
          return;
        }
        final int type = in.readInt();
        if (type != MessageType.QUERY) {
          throw (ParseFailureStatus)
            new ParseFailureStatus(
            ).initCause(null)
          ;
        }
        this.startQueryLogEntry(4 + in.readableBytes());
        setState(BackServerRawState.I_A_1);
      } break;
      case I_A_1: if (true) {
        if (!in.isReadable(16)) {
          return;
        }
        final byte[] x = new byte[16];
        in.readBytes(x);
        this.queryId = ByteBuffer.wrap(x);
        setState(BackServerRawState.I_A_2);
      } break;
      case I_A_2: if (true) {
        if (!this.readQuery(in)) {
          return;
        }
        if (this.analysis.equals("freq")) {
          if (this.ivList.size() != 0) {
            throw (ParseFailureStatus)
              new ParseFailureStatus(
              ).initCause(null)
            ;
          }
          this.sqlChannel = null;
          this.sqlResult = null;
          setState(BackServerRawState.I_A_F_0);
        } else {
          if (this.ivList.size() == 0) {
            this.sqlChannel = null;
            this.sqlResult = null;
            setState(BackServerRawState.I_A_M_NI_0);
          } else {
            this.isOts =
              new ResidueOtTable[this.globals.partyCount - 2]
            ;
            this.iuOts =
              new BlobOtTable[this.globals.partyCount - 2]
            ;
            this.iwOts =
              new ResidueOtTable[this.globals.partyCount - 2]
            ;
            final int rowCount = this.getRowCount();
            final int columnCount = this.dvList.size();
            final int bound = this.globals.groupSizeBound;
            for (int p = 0; p != this.isOts.length; ++p) {
              this.isOts[p] =
                new ResidueOtTable(
                  rowCount,
                  1,
                  BlobOtCell.SENDER,
                  4 * this.globals.subtableSize.longValue() * this.globals.rightBits,
                  BigInteger.valueOf(32),
                  this.globals.useFilesForLargeMemory,
                  this.globals.tmpdir
                )
              ;
            }
            for (int p = 0; p != this.iuOts.length; ++p) {
              this.iuOts[p] =
                new BlobOtTable(
                  rowCount,
                  1,
                  BlobOtCell.SENDER,
                  4 * this.globals.subtableSize.longValue() * 5,
                  1,
                  this.globals.useFilesForLargeMemory,
                  this.globals.tmpdir
                )
              ;
            }
            for (int p = 0; p != this.iwOts.length; ++p) {
              this.iwOts[p] =
                new ResidueOtTable(
                  rowCount,
                  columnCount,
                  BlobOtCell.SENDER,
                  4 * this.globals.subtableSize.longValue(),
                  this.globals.modulus,
                  this.globals.useFilesForLargeMemory,
                  this.globals.tmpdir
                )
              ;
            }
            this.isOtsColumnIndex = 0;
            this.isOtsPartyIndex = 0;
            this.isOtsRowIndex = 0;
            this.isOtsSBuffer = null;
            this.isOtsSBufferIndex = 0;
            this.iuOtsColumnIndex = 0;
            this.iuOtsPartyIndex = 0;
            this.iuOtsRowIndex = 0;
            this.iuOtsSBuffer = null;
            this.iuOtsSBufferIndex = 0;
            this.iwOtsColumnIndex = 0;
            this.iwOtsPartyIndex = 0;
            this.iwOtsRowIndex = 0;
            this.iwOtsSBuffer = null;
            this.iwOtsSBufferIndex = 0;
            this.partyIndex = 0;
            this.rowIndex = 0;
            this.columnIndex = 0;
            this.vectorIndex = 0;
            setState(BackServerRawState.I_A_M_YI_0);
          }
        }
      } break;
      case I_A_F_0: if (true) {
        if (in.isReadable()) {
          throw (ParseFailureStatus)
            new ParseFailureStatus(
            ).initCause(null)
          ;
        }
        if (this.sqlChannel == null) {
          this.getSqlChannel(ctx);
          return;
        }
        if (this.sqlResult == null) {
          final String query =
            new QueryBuilder(
              this.analysis,
              this.loanlinkbool,
              this.globals.domainNames,
              this.dvList,
              this.ivList,
              this.globals.localParty,
              this.remoteParty,
              this.globals.config.get("mysql_table"),
              JdbcSubprotocol.fromString(
                this.globals.config.get("jdbc_subprotocol")
              )
            ).buildQuery()
          ;
          this.getSqlResultSet(ctx, query);
          return;
        }
        this.sqlResult.next();
        final BigInteger summand =
          this.sqlResult.getBigDecimal(1).toBigInteger()
        ;
        final ByteBuf out = ctx.alloc().buffer();
        RawUtil.writeBigInteger(out, summand);
        this.writeAndFlush(ctx, out);
        this.sqlResult.close();
        this.sqlStatement.close();
        this.sqlChannel.release();
        setState(BackServerRawState.I_A_0);
        this.finishQueryLogEntry(in.readableBytes());
        this.cleanup();
      } break;
      case I_A_M_NI_0: if (true) {
        if (this.sqlChannel == null) {
          this.getSqlChannel(ctx);
          return;
        }
        if (this.sqlResult == null) {
          final String query =
            new QueryBuilder(
              this.analysis,
              this.loanlinkbool,
              this.globals.domainNames,
              this.dvList,
              this.ivList,
              this.globals.localParty,
              this.remoteParty,
              this.globals.config.get("mysql_table"),
              JdbcSubprotocol.fromString(
                this.globals.config.get("jdbc_subprotocol")
              )
            ).buildQuery()
          ;
          this.getSqlResultSet(ctx, query);
          return;
        }
        final ByteBuf out = ctx.alloc().buffer();
        this.sqlResult.next();
        for (int j = 0; j != this.dvList.size() + 1; ++j) {
          RawUtil.writeBigInteger(
            out,
            this.sqlResult.getBigDecimal(j + 1).toBigInteger()
          );
        }
        this.writeAndFlush(ctx, out);
        this.sqlResult.close();
        this.sqlStatement.close();
        this.sqlChannel.release();
        setState(BackServerRawState.I_A_0);
        this.finishQueryLogEntry(in.readableBytes());
        this.cleanup();
      } break;
      case I_A_M_YI_0: if (true) {
        for (; this.isOtsPartyIndex != this.isOts.length; ++this.isOtsPartyIndex) {
          for (; this.isOtsRowIndex != this.isOts[this.isOtsPartyIndex].cells.length; ++this.isOtsRowIndex) {
            for (; this.isOtsColumnIndex != this.isOts[this.isOtsPartyIndex].cells[this.isOtsRowIndex].length; ++this.isOtsColumnIndex) {
              for (; this.isOtsSBufferIndex != this.isOts[this.isOtsPartyIndex].cells[this.isOtsRowIndex][this.isOtsColumnIndex].getSBufferCount(); ++this.isOtsSBufferIndex) {
                if (this.isOtsSBuffer == null) {
                  this.isOtsSBuffer = this.isOts[this.isOtsPartyIndex].cells[this.isOtsRowIndex][this.isOtsColumnIndex].getSBuffer(this.isOtsSBufferIndex);
                }
                if (in.readableBytes() < this.isOtsSBuffer.remaining()) {
                  this.isOtsSBuffer.limit(this.isOtsSBuffer.position() + in.readableBytes());
                  in.readBytes(this.isOtsSBuffer);
                  this.isOtsSBuffer.limit(this.isOtsSBuffer.capacity());
                  return;
                }
                in.readBytes(this.isOtsSBuffer);
                this.isOtsSBuffer = null;
              }
              this.isOtsSBufferIndex = 0;
            }
            this.isOtsColumnIndex = 0;
          }
          this.isOtsRowIndex = 0;
        }
        for (; this.iuOtsPartyIndex != this.iuOts.length; ++this.iuOtsPartyIndex) {
          for (; this.iuOtsRowIndex != this.iuOts[this.iuOtsPartyIndex].cells.length; ++this.iuOtsRowIndex) {
            for (; this.iuOtsColumnIndex != this.iuOts[this.iuOtsPartyIndex].cells[this.iuOtsRowIndex].length; ++this.iuOtsColumnIndex) {
              for (; this.iuOtsSBufferIndex != this.iuOts[this.iuOtsPartyIndex].cells[this.iuOtsRowIndex][this.iuOtsColumnIndex].getSBufferCount(); ++this.iuOtsSBufferIndex) {
                if (this.iuOtsSBuffer == null) {
                  this.iuOtsSBuffer = this.iuOts[this.iuOtsPartyIndex].cells[this.iuOtsRowIndex][this.iuOtsColumnIndex].getSBuffer(this.iuOtsSBufferIndex);
                }
                if (in.readableBytes() < this.iuOtsSBuffer.remaining()) {
                  this.iuOtsSBuffer.limit(this.iuOtsSBuffer.position() + in.readableBytes());
                  in.readBytes(this.iuOtsSBuffer);
                  this.iuOtsSBuffer.limit(this.iuOtsSBuffer.capacity());
                  return;
                }
                in.readBytes(this.iuOtsSBuffer);
                this.iuOtsSBuffer = null;
              }
              this.iuOtsSBufferIndex = 0;
            }
            this.iuOtsColumnIndex = 0;
          }
          this.iuOtsRowIndex = 0;
        }
        for (; this.iwOtsPartyIndex != this.iwOts.length; ++this.iwOtsPartyIndex) {
          for (; this.iwOtsRowIndex != this.iwOts[this.iwOtsPartyIndex].cells.length; ++this.iwOtsRowIndex) {
            for (; this.iwOtsColumnIndex != this.iwOts[this.iwOtsPartyIndex].cells[this.iwOtsRowIndex].length; ++this.iwOtsColumnIndex) {
              for (; this.iwOtsSBufferIndex != this.iwOts[this.iwOtsPartyIndex].cells[this.iwOtsRowIndex][this.iwOtsColumnIndex].getSBufferCount(); ++this.iwOtsSBufferIndex) {
                if (this.iwOtsSBuffer == null) {
                  this.iwOtsSBuffer = this.iwOts[this.iwOtsPartyIndex].cells[this.iwOtsRowIndex][this.iwOtsColumnIndex].getSBuffer(this.iwOtsSBufferIndex);
                }
                if (in.readableBytes() < this.iwOtsSBuffer.remaining()) {
                  this.iwOtsSBuffer.limit(this.iwOtsSBuffer.position() + in.readableBytes());
                  in.readBytes(this.iwOtsSBuffer);
                  this.iwOtsSBuffer.limit(this.iwOtsSBuffer.capacity());
                  return;
                }
                in.readBytes(this.iwOtsSBuffer);
                this.iwOtsSBuffer = null;
              }
              this.iwOtsSBufferIndex = 0;
            }
            this.iwOtsColumnIndex = 0;
          }
          this.iwOtsRowIndex = 0;
        }
        setState(BackServerRawState.I_A_M_YI_1);
      } break;
      case I_A_M_YI_1: if (true) {
        this.iaCountdown =
          new AtomicInteger(this.globals.partyCount - 2)
        ;
        for (int i = 2; i != this.globals.partyCount; ++i) {
          final int remoteParty = i;
          final String host = this.globals.config.get(
            "raw_connect_host_" + remoteParty
          );
          final String port = this.globals.config.get(
            "raw_connect_port_" + remoteParty
          );
          this.logConnection(
            "connecting to " + host + ":" + port + "..."
          );
          final PoolEntry<ChannelFuture> myChannel =
            this.globals.rawChannels[remoteParty].acquire()
          ;
          myChannel.object.addListener(
            new ChannelFutureListener() {
              @Override
              public final void operationComplete(
                final ChannelFuture future
              ) throws
                Exception
              {
                if (!future.isSuccess()) {
                  throw (Exception)future.cause();
                }
                BackServerRawHandler.this.logConnection(
                  "connected to " + host + ":" + port
                );
                final ChannelPipeline pipeline =
                  myChannel.object.channel().pipeline()
                ;
                final BackServerRawHandler handler =
                  pipeline.get(BackServerRawHandler.class)
                ;
                handler.analysis = BackServerRawHandler.this.analysis;
                handler.loanlinkbool = BackServerRawHandler.this.loanlinkbool;
                handler.dvList = BackServerRawHandler.this.dvList;
                handler.iaCountdown = BackServerRawHandler.this.iaCountdown;
                handler.iaPipeline = ctx.pipeline();
                handler.isOts = BackServerRawHandler.this.isOts;
                handler.iuOts = BackServerRawHandler.this.iuOts;
                handler.ivList = BackServerRawHandler.this.ivList;
                handler.iwOts = BackServerRawHandler.this.iwOts;
                handler.myChannel = myChannel;
                handler.queryId = BackServerRawHandler.this.queryId;
                pipeline.fireUserEventTriggered(
                  BackServerRawHandler.WAKE_UP
                );
              }
            }
          );
        }
        setState(BackServerRawState.I_A_M_YI_2);
        return;
      } break;
      case I_A_M_YI_2: if (true) {
        if (in.isReadable()) {
          throw (ParseFailureStatus)
            new ParseFailureStatus(
            ).initCause(null)
          ;
        }
        final ByteBuf out = ctx.alloc().buffer();
        for (int i = 0; i != this.iwOts[0].cells.length; ++i) {
          for (int j = 0; j != this.iwOts[0].cells[i].length; ++j) {
            BigInteger sum = BigInteger.ZERO;
            for (int p = 0; p != this.globals.partyCount - 2; ++p) {
              sum =
                sum.add(
                  this.iwOts[p].cells[i][j].getSValue(
                    0,
                    0
                  )
                )
              ;
            }
            RawUtil.writeBigInteger(
              out,
              sum.mod(this.globals.modulus)
            );
          }
        }
        this.writeAndFlush(ctx, out);
        setState(BackServerRawState.I_A_0);
        this.finishQueryLogEntry(in.readableBytes());
        this.cleanup();
      } break;
      case I_G_0: if (true) {
        if (in.isReadable()) {
          throw (ParseFailureStatus)
            new ParseFailureStatus(
            ).initCause(null)
          ;
        }
        if (this.ivList.size() == 0) {
          throw (InternalErrorStatus)
            new InternalErrorStatus(
            ).initCause(null)
          ;
        }
        this.startQueryLogEntry(in.readableBytes());
        final ByteBuf out = ctx.alloc().buffer();
        this.writeQuery(out);
        this.writeAndFlush(ctx, out);
        this.sqlChannel = null;
        this.sqlResult = null;
        setState(BackServerRawState.I_G_M_YI_0);
      } break;
      case I_G_M_YI_0: if (true) {
        if (this.sqlChannel == null) {
          this.getSqlChannel(ctx);
          return;
        }
        if (this.sqlResult == null) {
          final String query =
            new QueryBuilder(
              this.analysis,
              this.loanlinkbool,
              this.globals.domainNames,
              this.dvList,
              this.ivList,
              this.globals.localParty,
              this.remoteParty,
              this.globals.config.get("mysql_table"),
              JdbcSubprotocol.fromString(
                this.globals.config.get("jdbc_subprotocol")
              )
            ).buildQuery()
          ;
          this.getSqlResultSet(ctx, query);
          return;
        }
        final long subtableSize =
          this.globals.subtableSize.longValue()
        ;
        final int rightBits =
          this.globals.rightBits
        ;
        final int rowCount =
          this.isOts[0].cells.length
        ;
        final int wColumnCount =
          this.iwOts[0].cells[0].length
        ;
        this.cuckooTable = new CuckooTable[1];
        this.cuckooTable[0] =
          new CuckooTable(
            this.globals.subtableBits,
            this.globals.xBits,
            this.globals.useFilesForLargeMemory,
            this.globals.tmpdir
          )
        ;
        this.dvMaps =
          this.cuckooTable[0].buildAlice(
            this.sqlResult,
            100
          )
        ;
        this.selbits = new BigBitArray[rowCount];
        for (int row = 0; row != rowCount; ++row) {
          this.selbits[row] =
            new BigBitArray(
              4 * subtableSize * rightBits,
              this.globals.useFilesForLargeMemory,
              this.globals.tmpdir
            )
          ;
        }
        this.selbitsBuffer = null;
        this.selbitsBufferIndex = 0;
        this.selbitsRowIndex = 0;
        setState(BackServerRawState.I_G_M_YI_1);
      } break;
      case I_G_M_YI_1: if (true) {
        for (; this.selbitsRowIndex != this.selbits.length; ++this.selbitsRowIndex) {
          for (; this.selbitsBufferIndex != this.selbits[this.selbitsRowIndex].getBufferCount(); ++this.selbitsBufferIndex) {
            if (this.selbitsBuffer == null) {
              this.selbitsBuffer = this.selbits[this.selbitsRowIndex].getBuffer(this.selbitsBufferIndex);
            }
            if (in.readableBytes() < this.selbitsBuffer.remaining()) {
              this.selbitsBuffer.limit(this.selbitsBuffer.position() + in.readableBytes());
              in.readBytes(this.selbitsBuffer);
              this.selbitsBuffer.limit(this.selbitsBuffer.capacity());
              return;
            }
            in.readBytes(this.selbitsBuffer);
            this.selbitsBuffer = null;
          }
          this.selbitsBufferIndex = 0;
        }
        final long subtableSize =
          this.globals.subtableSize.longValue()
        ;
        final int rightBits =
          this.globals.rightBits
        ;
        final int rowCount =
          this.isOts[0].cells.length
        ;
        final ResidueOtTable isOt =
          this.isOts[this.remoteParty - 2]
        ;
        this.sendsec = new BigByteArray[rowCount];
        this.C_S = new BigByteArray[rowCount];
        final SecureRandom random = new SecureRandom();
        final byte[] aBytes = new byte[rightBits];
        for (int row = 0; row != rowCount; ++row) {
          this.sendsec[row] =
            new BigByteArray(
              4 * subtableSize * rightBits * 2,
              this.globals.useFilesForLargeMemory,
              this.globals.tmpdir
            )
          ;
          this.C_S[row] =
            new BigByteArray(
              4 * subtableSize,
              this.globals.useFilesForLargeMemory,
              this.globals.tmpdir
            )
          ;
          for (int i = 0; i != 4; ++i) {
            for (long j = 0; j != subtableSize; ++j) {
              final int xR =
                this.cuckooTable[0].subtables[i].getValue(j)
              ;
              int sum = 0;
              random.nextBytes(aBytes);
              for (int k = 0; k != rightBits; ++k) {
                final int x = (xR >> k) & 1;
                final int a = aBytes[k] & 0x1F;
                sum = (sum + a) % 32;
                final int z =
                  this.selbits[row].getValue(
                    (i * subtableSize + j) * rightBits + k
                  )
                ;
                for (int p = 0; p != 2; ++p) {
                  final int s =
                    isOt.cells[row][0].getSValue(
                      (i * subtableSize + j) * rightBits + k,
                      p ^ z
                    ).intValue()
                  ;
                  this.sendsec[row].setValue(
                    ((i * subtableSize + j) * rightBits + k) * 2 + p,
                    (byte)((a + (p ^ x) + s) % 32)
                  );
                }
              }
              this.C_S[row].setValue(
                i * subtableSize + j,
                (byte)sum
              );
            }
          }
          final int bufferCount =
            this.sendsec[row].getBufferCount()
          ;
          for (int i = 0; i != bufferCount; ++i) {
            this.writeAndFlush(
              ctx,
              Unpooled.wrappedBuffer(
                this.sendsec[row].getBuffer(i)
              )
            );
          }
        }
        this.PSIShare_S = new BigBitArray[rowCount];
        this.csendsec = new BigByteArray[rowCount];
        this.cselbits = new BigBitArray[rowCount];
        for (int row = 0; row != rowCount; ++row) {
          this.PSIShare_S[row] =
            new BigBitArray(
              4 * subtableSize,
              this.globals.useFilesForLargeMemory,
              this.globals.tmpdir
            )
          ;
          this.csendsec[row] =
            new BigByteArray(
              4 * subtableSize * 5 * 2,
              this.globals.useFilesForLargeMemory,
              this.globals.tmpdir
            )
          ;
          makeFullEqualityCircuits(
            this.PSIShare_S[row],
            this.csendsec[row],
            4 * subtableSize
          );
          this.cselbits[row] =
            new BigBitArray(
              4 * subtableSize * 5,
              this.globals.useFilesForLargeMemory,
              this.globals.tmpdir
            )
          ;
        }
        this.cselbitsBuffer = null;
        this.cselbitsBufferIndex = 0;
        this.cselbitsRowIndex = 0;
        setState(BackServerRawState.I_G_M_YI_2);
      } break;
      case I_G_M_YI_2: if (true) {
        for (; this.cselbitsRowIndex != this.cselbits.length; ++this.cselbitsRowIndex) {
          for (; this.cselbitsBufferIndex != this.cselbits[this.cselbitsRowIndex].getBufferCount(); ++this.cselbitsBufferIndex) {
            if (this.cselbitsBuffer == null) {
              this.cselbitsBuffer = this.cselbits[this.cselbitsRowIndex].getBuffer(this.cselbitsBufferIndex);
            }
            if (in.readableBytes() < this.cselbitsBuffer.remaining()) {
              this.cselbitsBuffer.limit(this.cselbitsBuffer.position() + in.readableBytes());
              in.readBytes(this.cselbitsBuffer);
              this.cselbitsBuffer.limit(this.cselbitsBuffer.capacity());
              return;
            }
            in.readBytes(this.cselbitsBuffer);
            this.cselbitsBuffer = null;
          }
          this.cselbitsBufferIndex = 0;
        }
        final long subtableSize =
          this.globals.subtableSize.longValue()
        ;
        final int rightBits =
          this.globals.rightBits
        ;
        final int rowCount =
          this.isOts[0].cells.length
        ;
        final int wColumnCount =
          this.iwOts[0].cells[0].length
        ;
        final BlobOtTable iuOt =
          this.iuOts[this.remoteParty - 2]
        ;
        final int[] stuff = new int[2];
        final byte[] u = new byte[1];
        for (int row = 0; row != rowCount; ++row) {
          for (int i = 0; i != 4; ++i) {
            for (long j = 0; j != subtableSize; ++j) {
              final int c =
                this.C_S[row].getValue(
                  i * subtableSize + j
                )
              ;
              for (int k = 0; k != 5; ++k) {
                final int x = (c >> k) & 1;
                for (int p = 0; p != 2; ++p) {
                  stuff[p] =
                    this.csendsec[row].getValue(
                      ((i * subtableSize + j) * 5 + k) * 2 + p
                    )
                  ;
                }
                final int cs =
                  this.cselbits[row].getValue(
                    (i * subtableSize + j) * 5 + k
                  )
                ;
                for (int p = 0; p != 2; ++p) {
                  iuOt.cells[row][0].getSValue(
                    (i * subtableSize + j) * 5 + k,
                    p ^ cs,
                    u,
                    0
                  );
                  this.csendsec[row].setValue(
                    ((i * subtableSize + j) * 5 + k) * 2 + p,
                    (byte)(stuff[(1 - p) ^ x] ^ u[0])
                  );
                }
              }
            }
          }
          final int bufferCount =
            this.csendsec[row].getBufferCount()
          ;
          for (int i = 0; i != bufferCount; ++i) {
            this.writeAndFlush(
              ctx,
              Unpooled.wrappedBuffer(
                this.csendsec[row].getBuffer(i)
              )
            );
          }
        }
        this.yselbits = new BigBitArray[rowCount][];
        for (int row = 0; row != rowCount; ++row) {
          this.yselbits[row] = new BigBitArray[wColumnCount];
          for (int col = 0; col != wColumnCount; ++col) {
            this.yselbits[row][col] =
              new BigBitArray(
                4 * subtableSize,
                this.globals.useFilesForLargeMemory,
                this.globals.tmpdir
              )
            ;
          }
        }
        this.yselbitsBuffer = null;
        this.yselbitsBufferIndex = 0;
        this.yselbitsColumnIndex = 0;
        this.yselbitsRowIndex = 0;
        setState(BackServerRawState.I_G_M_YI_3);
      } break;
      case I_G_M_YI_3: if (true) {
        for (; this.yselbitsRowIndex != this.yselbits.length; ++this.yselbitsRowIndex) {
          for (; this.yselbitsColumnIndex != this.yselbits[this.yselbitsRowIndex].length; ++this.yselbitsColumnIndex) {
            for (; this.yselbitsBufferIndex != this.yselbits[this.yselbitsRowIndex][this.yselbitsColumnIndex].getBufferCount(); ++this.yselbitsBufferIndex) {
              if (this.yselbitsBuffer == null) {
                this.yselbitsBuffer = this.yselbits[this.yselbitsRowIndex][this.yselbitsColumnIndex].getBuffer(this.yselbitsBufferIndex);
              }
              if (in.readableBytes() < this.yselbitsBuffer.remaining()) {
                this.yselbitsBuffer.limit(this.yselbitsBuffer.position() + in.readableBytes());
                in.readBytes(this.yselbitsBuffer);
                this.yselbitsBuffer.limit(this.yselbitsBuffer.capacity());
                return;
              }
              in.readBytes(this.yselbitsBuffer);
              this.yselbitsBuffer = null;
            }
            this.yselbitsBufferIndex = 0;
          }
          this.yselbitsColumnIndex = 0;
        }
        final long subtableSize =
          this.globals.subtableSize.longValue()
        ;
        final int rowCount =
          this.isOts[0].cells.length
        ;
        final int wColumnCount =
          this.iwOts[0].cells[0].length
        ;
        final ResidueOtTable iwOt =
          this.iwOts[this.remoteParty - 2]
        ;
        final BigInteger[] r = new BigInteger[1];
        final BigInteger[] temp = new BigInteger[2];
        this.maskedv = new BigResidueArray[rowCount][];
        for (int row = 0; row != rowCount; ++row) {
          this.maskedv[row] = new BigResidueArray[wColumnCount];
          for (int col = 0; col != wColumnCount; ++col) {
            this.maskedv[row][col] =
              new BigResidueArray(
                4 * subtableSize * 2,
                this.globals.modulus,
                this.globals.useFilesForLargeMemory,
                this.globals.tmpdir
              )
            ;
            BigInteger out_S = BigInteger.ZERO;
            for (int i = 0; i != 4; ++i) {
              for (long j = 0; j != subtableSize; ++j) {
                Rand.modular(
                  Rand.defaultSrc(),
                  this.globals.modulus,
                  r
                );
                temp[0] =
                  this.globals.modulus.subtract(r[0])
                ;
                final int xR =
                  this.cuckooTable[0].subtables[i].getValue(j)
                ;
                if (xR == CuckooTable.NULL_XR) {
                  temp[1] = temp[0];
                } else {
                  temp[1] =
                    BigInteger.ONE.add(
                      BigInteger.valueOf(
                        this.dvMaps.get(col).get(
                          this.cuckooTable[0].unhash(
                            i,
                            (int)j,
                            xR
                          )
                        )
                      ).shiftLeft(32)
                    ).subtract(
                      r[0]
                    )
                  ;
                }
                final int psi =
                  this.PSIShare_S[row].getValue(
                    i * subtableSize + j
                  )
                ;
                if (psi == 1) {
                  final BigInteger t = temp[0];
                  temp[0] = temp[1];
                  temp[1] = t;
                }
                final int y =
                  this.yselbits[row][col].getValue(
                    i * subtableSize + j
                  )
                ;
                for (int k = 0; k != 2; ++k) {
                  this.maskedv[row][col].setValue(
                    (i * subtableSize + j) * 2 + k,
                    temp[k].add(
                      iwOt.cells[row][col].getSValue(
                        i * subtableSize + j,
                        k ^ y
                      )
                    ).mod(
                      this.globals.modulus
                    )
                  );
                }
                out_S = out_S.add(r[0]);
              }
            }
            iwOt.cells[row][col].setSValue(
              0,
              0,
              out_S.mod(this.globals.modulus)
            );
            final int bufferCount =
              this.maskedv[row][col].getBufferCount()
            ;
            for (int i = 0; i != bufferCount; ++i) {
              final ChannelFuture f =
                this.writeAndFlush(
                  ctx,
                  Unpooled.wrappedBuffer(
                    this.maskedv[row][col].getBuffer(i)
                  )
                )
              ;
              if (
                row == rowCount - 1 &&
                col == wColumnCount - 1 &&
                i == bufferCount - 1
              ) {
                final BigResidueArray[][] mv = this.maskedv;
                f.addListener(
                  new ChannelFutureListener() {
                    @Override
                    public final void operationComplete(
                      final ChannelFuture future
                    ) {
                      if (mv != null) {
                        for (int i = 0; i != mv.length; ++i) {
                          if (mv[i] != null) {
                            for (int j = 0; j != mv[i].length; ++j) {
                              if (mv[i][j] != null) {
                                try {
                                  mv[i][j].close();
                                } catch (final Exception e) {
                                }
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                );
                this.maskedv = null;
              }
            }
          }
        }
        if (this.iaCountdown.decrementAndGet() == 0) {
          this.iaPipeline.fireUserEventTriggered(
            BackServerRawHandler.WAKE_UP
          );
        }
        final PoolEntry<ChannelFuture> pe = this.myChannel;
        setState(BackServerRawState.I_G_0);
        this.finishQueryLogEntry(in.readableBytes());
        this.cleanup();
        pe.release();
        return;
      } break;
      case G_0: if (true) {
        if (this.remoteParty == -1) {
          if (!this.readRemoteParty(in)) {
            return;
          }
          if (this.remoteParty == 0) {
            setState(BackServerRawState.G_A_0);
          } else if (this.remoteParty == 1) {
            setState(BackServerRawState.G_I_0);
          } else {
            throw (ParseFailureStatus)
              new ParseFailureStatus(
              ).initCause(null)
            ;
          }
        } else {
          final ByteBuf out = ctx.alloc().buffer();
          out.writeInt(MessageType.ID);
          out.writeInt(this.globals.localParty);
          this.writeAndFlush(ctx, out);
          setState(BackServerRawState.G_W_0);
        }
      } break;
      case G_A_0: if (true) {
        if (!in.isReadable(4)) {
          return;
        }
        final int type = in.readInt();
        if (type != MessageType.QUERY) {
          throw (ParseFailureStatus)
            new ParseFailureStatus(
            ).initCause(null)
          ;
        }
        this.startQueryLogEntry(4 + in.readableBytes());
        setState(BackServerRawState.G_A_1);
      } break;
      case G_A_1: if (true) {
        if (!in.isReadable(16)) {
          return;
        }
        final byte[] x = new byte[16];
        in.readBytes(x);
        this.queryId = ByteBuffer.wrap(x);
        setState(BackServerRawState.G_A_2);
      } break;
      case G_A_2: if (true) {
        if (!this.readQuery(in)) {
          return;
        }
        if (this.analysis.equals("freq")) {
          setState(BackServerRawState.G_A_F_0);
        } else {
          if (this.ivList.size() == 0) {
            throw (ParseFailureStatus)
              new ParseFailureStatus(
              ).initCause(null)
            ;
          }
          final int rowCount = this.getRowCount();
          final int columnCount = this.dvList.size();
          final int bound = this.globals.groupSizeBound;
          this.gsOt =
            new ResidueOtTable(
              rowCount,
              1,
              BlobOtCell.RECEIVER,
              4 * this.globals.subtableSize.longValue() * this.globals.rightBits,
              BigInteger.valueOf(32),
              this.globals.useFilesForLargeMemory,
              this.globals.tmpdir
            )
          ;
          this.guOt =
            new BlobOtTable(
              rowCount,
              1,
              BlobOtCell.RECEIVER,
              4 * this.globals.subtableSize.longValue() * 5,
              1,
              this.globals.useFilesForLargeMemory,
              this.globals.tmpdir
            )
          ;
          this.gwOt =
            new ResidueOtTable(
              rowCount,
              columnCount,
              BlobOtCell.RECEIVER,
              4 * this.globals.subtableSize.longValue(),
              this.globals.modulus,
              this.globals.useFilesForLargeMemory,
              this.globals.tmpdir
            )
          ;
          this.gsOtBBuffer = null;
          this.gsOtBBufferIndex = 0;
          this.gsOtColumnIndex = 0;
          this.gsOtRowIndex = 0;
          this.gsOtUBuffer = null;
          this.gsOtUBufferIndex = 0;
          this.guOtBBuffer = null;
          this.guOtBBufferIndex = 0;
          this.guOtColumnIndex = 0;
          this.guOtRowIndex = 0;
          this.guOtUBuffer = null;
          this.guOtUBufferIndex = 0;
          this.gwOtBBuffer = null;
          this.gwOtBBufferIndex = 0;
          this.gwOtColumnIndex = 0;
          this.gwOtRowIndex = 0;
          this.gwOtUBuffer = null;
          this.gwOtUBufferIndex = 0;
          this.rowIndex = 0;
          this.columnIndex = 0;
          this.vectorIndex = 0;
          setState(BackServerRawState.G_A_M_YI_0);
        }
      } break;
      case G_A_F_0: if (true) {
        final int remoteParty = this.globals.partyCount - 1;
        final String host = this.globals.config.get(
          "raw_connect_host_" + remoteParty
        );
        final String port = this.globals.config.get(
          "raw_connect_port_" + remoteParty
        );
        this.logConnection(
          "connecting to " + host + ":" + port + "..."
        );
        final PoolEntry<ChannelFuture> myChannel =
          this.globals.rawChannels[remoteParty].acquire()
        ;
        this.gfShare =
          new GfShare(
            this.getRowCount(),
            this.globals.modulus
          )
        ;
        myChannel.object.addListener(
          new ChannelFutureListener() {
            @Override
            public final void operationComplete(
              final ChannelFuture future
            ) throws
              Exception
            {
              if (!future.isSuccess()) {
                throw (Exception)future.cause();
              }
              BackServerRawHandler.this.logConnection(
                "connected to " + host + ":" + port
              );
              final ChannelPipeline pipeline =
                myChannel.object.channel().pipeline()
              ;
              final BackServerRawHandler handler =
                pipeline.get(BackServerRawHandler.class)
              ;
              handler.analysis = BackServerRawHandler.this.analysis;
              handler.loanlinkbool = BackServerRawHandler.this.loanlinkbool;
              handler.dvList = BackServerRawHandler.this.dvList;
              handler.gfShare = BackServerRawHandler.this.gfShare;
              handler.ivList = BackServerRawHandler.this.ivList;
              handler.myChannel = myChannel;
              handler.queryId = BackServerRawHandler.this.queryId;
              pipeline.fireUserEventTriggered(
                BackServerRawHandler.WAKE_UP
              );
            }
          }
        );
        this.sqlChannel = null;
        this.sqlResult = null;
        setState(BackServerRawState.G_A_F_1);
      } break;
      case G_A_F_1: if (true) {
        if (this.sqlChannel == null) {
          this.getSqlChannel(ctx);
          return;
        }
        if (this.sqlResult == null) {
          final String query =
            new QueryBuilder(
              this.analysis,
              this.loanlinkbool,
              this.globals.domainNames,
              this.dvList,
              this.ivList,
              this.globals.localParty,
              this.remoteParty,
              this.globals.config.get("mysql_table"),
              JdbcSubprotocol.fromString(
                this.globals.config.get("jdbc_subprotocol")
              )
            ).buildQuery()
          ;
          this.getSqlResultSet(ctx, query);
          return;
        }
        final ByteBuf out = ctx.alloc().buffer();
        final int j = this.sqlResult.findColumn("freq");
        this.sqlResult.next();
        for (int i = 0; i != this.gfShare.rowCount; ++i) {
          RawUtil.writeBigInteger(
            out,
            this.gfShare.summands.get(i).add(
              this.sqlResult.getBigDecimal(j).toBigInteger()
            )
          );
          this.sqlResult.next();
        }
        this.writeAndFlush(ctx, out);
        this.sqlResult.close();
        this.sqlStatement.close();
        this.sqlChannel.release();
        setState(BackServerRawState.G_A_0);
        this.finishQueryLogEntry(in.readableBytes());
        this.cleanup();
      } break;
      case G_A_M_YI_0: if (true) {
        for (; this.gsOtRowIndex != this.gsOt.cells.length; ++this.gsOtRowIndex) {
          for (; this.gsOtColumnIndex != this.gsOt.cells[this.gsOtRowIndex].length; ++this.gsOtColumnIndex) {
            for (; this.gsOtBBufferIndex != this.gsOt.cells[this.gsOtRowIndex][this.gsOtColumnIndex].getBBufferCount(); ++this.gsOtBBufferIndex) {
              if (this.gsOtBBuffer == null) {
                this.gsOtBBuffer = this.gsOt.cells[this.gsOtRowIndex][this.gsOtColumnIndex].getBBuffer(this.gsOtBBufferIndex);
              }
              if (in.readableBytes() < this.gsOtBBuffer.remaining()) {
                this.gsOtBBuffer.limit(this.gsOtBBuffer.position() + in.readableBytes());
                in.readBytes(this.gsOtBBuffer);
                this.gsOtBBuffer.limit(this.gsOtBBuffer.capacity());
                return;
              }
              in.readBytes(this.gsOtBBuffer);
              this.gsOtBBuffer = null;
            }
            for (; this.gsOtUBufferIndex != this.gsOt.cells[this.gsOtRowIndex][this.gsOtColumnIndex].getUBufferCount(); ++this.gsOtUBufferIndex) {
              if (this.gsOtUBuffer == null) {
                this.gsOtUBuffer = this.gsOt.cells[this.gsOtRowIndex][this.gsOtColumnIndex].getUBuffer(this.gsOtUBufferIndex);
              }
              if (in.readableBytes() < this.gsOtUBuffer.remaining()) {
                this.gsOtUBuffer.limit(this.gsOtUBuffer.position() + in.readableBytes());
                in.readBytes(this.gsOtUBuffer);
                this.gsOtUBuffer.limit(this.gsOtUBuffer.capacity());
                return;
              }
              in.readBytes(this.gsOtUBuffer);
              this.gsOtUBuffer = null;
            }
            this.gsOtBBufferIndex = 0;
            this.gsOtUBufferIndex = 0;
          }
          this.gsOtColumnIndex = 0;
        }
        for (; this.guOtRowIndex != this.guOt.cells.length; ++this.guOtRowIndex) {
          for (; this.guOtColumnIndex != this.guOt.cells[this.guOtRowIndex].length; ++this.guOtColumnIndex) {
            for (; this.guOtBBufferIndex != this.guOt.cells[this.guOtRowIndex][this.guOtColumnIndex].getBBufferCount(); ++this.guOtBBufferIndex) {
              if (this.guOtBBuffer == null) {
                this.guOtBBuffer = this.guOt.cells[this.guOtRowIndex][this.guOtColumnIndex].getBBuffer(this.guOtBBufferIndex);
              }
              if (in.readableBytes() < this.guOtBBuffer.remaining()) {
                this.guOtBBuffer.limit(this.guOtBBuffer.position() + in.readableBytes());
                in.readBytes(this.guOtBBuffer);
                this.guOtBBuffer.limit(this.guOtBBuffer.capacity());
                return;
              }
              in.readBytes(this.guOtBBuffer);
              this.guOtBBuffer = null;
            }
            for (; this.guOtUBufferIndex != this.guOt.cells[this.guOtRowIndex][this.guOtColumnIndex].getUBufferCount(); ++this.guOtUBufferIndex) {
              if (this.guOtUBuffer == null) {
                this.guOtUBuffer = this.guOt.cells[this.guOtRowIndex][this.guOtColumnIndex].getUBuffer(this.guOtUBufferIndex);
              }
              if (in.readableBytes() < this.guOtUBuffer.remaining()) {
                this.guOtUBuffer.limit(this.guOtUBuffer.position() + in.readableBytes());
                in.readBytes(this.guOtUBuffer);
                this.guOtUBuffer.limit(this.guOtUBuffer.capacity());
                return;
              }
              in.readBytes(this.guOtUBuffer);
              this.guOtUBuffer = null;
            }
            this.guOtBBufferIndex = 0;
            this.guOtUBufferIndex = 0;
          }
          this.guOtColumnIndex = 0;
        }
        for (; this.gwOtRowIndex != this.gwOt.cells.length; ++this.gwOtRowIndex) {
          for (; this.gwOtColumnIndex != this.gwOt.cells[this.gwOtRowIndex].length; ++this.gwOtColumnIndex) {
            for (; this.gwOtBBufferIndex != this.gwOt.cells[this.gwOtRowIndex][this.gwOtColumnIndex].getBBufferCount(); ++this.gwOtBBufferIndex) {
              if (this.gwOtBBuffer == null) {
                this.gwOtBBuffer = this.gwOt.cells[this.gwOtRowIndex][this.gwOtColumnIndex].getBBuffer(this.gwOtBBufferIndex);
              }
              if (in.readableBytes() < this.gwOtBBuffer.remaining()) {
                this.gwOtBBuffer.limit(this.gwOtBBuffer.position() + in.readableBytes());
                in.readBytes(this.gwOtBBuffer);
                this.gwOtBBuffer.limit(this.gwOtBBuffer.capacity());
                return;
              }
              in.readBytes(this.gwOtBBuffer);
              this.gwOtBBuffer = null;
            }
            for (; this.gwOtUBufferIndex != this.gwOt.cells[this.gwOtRowIndex][this.gwOtColumnIndex].getUBufferCount(); ++this.gwOtUBufferIndex) {
              if (this.gwOtUBuffer == null) {
                this.gwOtUBuffer = this.gwOt.cells[this.gwOtRowIndex][this.gwOtColumnIndex].getUBuffer(this.gwOtUBufferIndex);
              }
              if (in.readableBytes() < this.gwOtUBuffer.remaining()) {
                this.gwOtUBuffer.limit(this.gwOtUBuffer.position() + in.readableBytes());
                in.readBytes(this.gwOtUBuffer);
                this.gwOtUBuffer.limit(this.gwOtUBuffer.capacity());
                return;
              }
              in.readBytes(this.gwOtUBuffer);
              this.gwOtUBuffer = null;
            }
            this.gwOtBBufferIndex = 0;
            this.gwOtUBufferIndex = 0;
          }
          this.gwOtColumnIndex = 0;
        }
        setState(BackServerRawState.G_A_M_YI_1);
        this.getGmShare();
        this.gmShare.gaPipeline = ctx.pipeline();
        this.gmShare.gsOt = this.gsOt;
        this.gmShare.guOt = this.guOt;
        this.gmShare.gwOt = this.gwOt;
        if (this.gmShare.countdown.decrementAndGet() == 0) {
          this.gmShare.giPipeline.fireUserEventTriggered(
            BackServerRawHandler.WAKE_UP
          );
        }
        return;
      } break;
      case G_A_M_YI_1: if (true) {
        if (in.isReadable()) {
          throw (ParseFailureStatus)
            new ParseFailureStatus(
            ).initCause(null)
          ;
        }
        final ByteBuf out = ctx.alloc().buffer();
        for (int i = 0; i != this.gwOt.cells.length; ++i) {
          for (int j = 0; j != this.gwOt.cells[i].length; ++j) {
            RawUtil.writeBigInteger(
              out,
              this.gwOt.cells[i][j].getUValue(0)
            );
          }
        }
        this.writeAndFlush(ctx, out);
        if (this.globals.localParty == this.globals.partyCount - 1) {
          setState(BackServerRawState.W_A_0);
        } else {
          setState(BackServerRawState.G_A_0);
        }
        this.finishQueryLogEntry(in.readableBytes());
        this.cleanup();
      } break;
      case G_I_0: if (true) {
        if (!in.isReadable(4)) {
          return;
        }
        final int type = in.readInt();
        if (type != MessageType.QUERY) {
          throw (ParseFailureStatus)
            new ParseFailureStatus(
            ).initCause(null)
          ;
        }
        this.startQueryLogEntry(4 + in.readableBytes());
        setState(BackServerRawState.G_I_1);
      } break;
      case G_I_1: if (true) {
        if (!in.isReadable(16)) {
          return;
        }
        final byte[] x = new byte[16];
        in.readBytes(x);
        this.queryId = ByteBuffer.wrap(x);
        setState(BackServerRawState.G_I_2);
      } break;
      case G_I_2: if (true) {
        if (!this.readQuery(in)) {
          return;
        }
        if (this.analysis.equals("freq")) {
          throw (ParseFailureStatus)
            new ParseFailureStatus(
            ).initCause(null)
          ;
        } else {
          this.sqlChannel = null;
          this.sqlResult = null;
          setState(BackServerRawState.G_I_M_YI_0);
        }
      } break;
      case G_I_M_YI_0: if (true) {
        if (this.sqlChannel == null) {
          this.getSqlChannel(ctx);
          return;
        }
        if (this.sqlResult == null) {
          final String query =
            new QueryBuilder(
              this.analysis,
              this.loanlinkbool,
              this.globals.domainNames,
              this.dvList,
              this.ivList,
              this.globals.localParty,
              this.remoteParty,
              this.globals.config.get("mysql_table"),
              JdbcSubprotocol.fromString(
                this.globals.config.get("jdbc_subprotocol")
              )
            ).buildQuery()
          ;
          this.getSqlResultSet(ctx, query);
          return;
        }
        setState(BackServerRawState.G_I_M_YI_1);
        this.getGmShare();
        this.gmShare.giPipeline = ctx.pipeline();
        if (this.gmShare.countdown.decrementAndGet() != 0) {
          return;
        }
      } break;
      case G_I_M_YI_1: if (true) {
        final long subtableSize =
          this.globals.subtableSize.longValue()
        ;
        final int rightBits =
          this.globals.rightBits
        ;
        final int rowCount =
          this.gmShare.gsOt.cells.length
        ;
        this.cuckooTable = new CuckooTable[rowCount];
        this.selbits = new BigBitArray[rowCount];
        this.sqlResult.next();
        for (int row = 0; row != rowCount; ++row) {
          this.cuckooTable[row] =
            new CuckooTable(
              this.globals.subtableBits,
              this.globals.xBits,
              this.globals.useFilesForLargeMemory,
              this.globals.tmpdir
            )
          ;
          this.cuckooTable[row].buildBob(
            this.sqlResult,
            100
          );
          this.selbits[row] =
            new BigBitArray(
              4 * subtableSize * rightBits,
              this.globals.useFilesForLargeMemory,
              this.globals.tmpdir
            )
          ;
          for (int i = 0; i != 4; ++i) {
            for (long j = 0; j != subtableSize; ++j) {
              final int xR =
                this.cuckooTable[row].subtables[i].getValue(j)
              ;
              for (int k = 0; k != rightBits; ++k) {
                final int b =
                  this.gmShare.gsOt.cells[row][0].getBValue(
                    (i * subtableSize + j) * rightBits + k
                  )
                ;
                final int y =
                  (xR >> k) & 1
                ;
                this.selbits[row].setValue(
                  (i * subtableSize + j) * rightBits + k,
                  b ^ y
                );
              }
            }
          }
          final int bufferCount =
            this.selbits[row].getBufferCount()
          ;
          for (int i = 0; i != bufferCount; ++i) {
            this.writeAndFlush(
              ctx,
              Unpooled.wrappedBuffer(
                this.selbits[row].getBuffer(i)
              )
            );
          }
        }
        this.sendsec = new BigByteArray[rowCount];
        for (int row = 0; row != rowCount; ++row) {
          this.sendsec[row] =
            new BigByteArray(
              4 * subtableSize * rightBits * 2,
              this.globals.useFilesForLargeMemory,
              this.globals.tmpdir
            )
          ;
        }
        this.sendsecBuffer = null;
        this.sendsecBufferIndex = 0;
        this.sendsecRowIndex = 0;
        setState(BackServerRawState.G_I_M_YI_2);
      } break;
      case G_I_M_YI_2: if (true) {
        for (; this.sendsecRowIndex != this.sendsec.length; ++this.sendsecRowIndex) {
          for (; this.sendsecBufferIndex != this.sendsec[this.sendsecRowIndex].getBufferCount(); ++this.sendsecBufferIndex) {
            if (this.sendsecBuffer == null) {
              this.sendsecBuffer = this.sendsec[this.sendsecRowIndex].getBuffer(this.sendsecBufferIndex);
            }
            if (in.readableBytes() < this.sendsecBuffer.remaining()) {
              this.sendsecBuffer.limit(this.sendsecBuffer.position() + in.readableBytes());
              in.readBytes(this.sendsecBuffer);
              this.sendsecBuffer.limit(this.sendsecBuffer.capacity());
              return;
            }
            in.readBytes(this.sendsecBuffer);
            this.sendsecBuffer = null;
          }
          this.sendsecBufferIndex = 0;
        }
        final long subtableSize =
          this.globals.subtableSize.longValue()
        ;
        final int rightBits =
          this.globals.rightBits
        ;
        final int rowCount =
          this.gmShare.gsOt.cells.length
        ;
        this.C_U = new BigByteArray[rowCount];
        for (int row = 0; row != rowCount; ++row) {
          this.C_U[row] =
            new BigByteArray(
              4 * subtableSize,
              this.globals.useFilesForLargeMemory,
              this.globals.tmpdir
            )
          ;
          for (int i = 0; i != 4; ++i) {
            for (long j = 0; j != subtableSize; ++j) {
              final int xR =
                this.cuckooTable[row].subtables[i].getValue(j)
              ;
              int sum = 0;
              for (int k = 0; k != rightBits; ++k) {
                final int y =
                  (xR >> k) & 1
                ;
                final int val =
                  this.sendsec[row].getValue(
                    ((i * subtableSize + j) * rightBits + k) * 2 + y
                  )
                ;
                final int s =
                  this.gmShare.gsOt.cells[row][0].getUValue(
                    (i * subtableSize + j) * rightBits + k
                  ).intValue()
                ;
                sum = (sum + val - s + 32) % 32;
              }
              this.C_U[row].setValue(
                i * subtableSize + j,
                (byte)sum
              );
            }
          }
        }
        this.cselbits = new BigBitArray[rowCount];
        for (int row = 0; row != rowCount; ++row) {
          this.cselbits[row] =
            new BigBitArray(
              4 * subtableSize * 5,
              this.globals.useFilesForLargeMemory,
              this.globals.tmpdir
            )
          ;
          for (int i = 0; i != 4; ++i) {
            for (long j = 0; j != subtableSize; ++j) {
              final int c =
                this.C_U[row].getValue(
                  i * subtableSize + j
                )
              ;
              for (int k = 0; k != 5; ++k) {
                final int y =
                  (c >> k) & 1
                ;
                final int v =
                  this.gmShare.guOt.cells[row][0].getBValue(
                    (i * subtableSize + j) * 5 + k
                  )
                ;
                this.cselbits[row].setValue(
                  (i * subtableSize + j) * 5 + k,
                  v ^ y
                );
              }
            }
          }
          final int bufferCount =
            this.cselbits[row].getBufferCount()
          ;
          for (int i = 0; i != bufferCount; ++i) {
            this.writeAndFlush(
              ctx,
              Unpooled.wrappedBuffer(
                this.cselbits[row].getBuffer(i)
              )
            );
          }
        }
        this.csendsec = new BigByteArray[rowCount];
        for (int row = 0; row != rowCount; ++row) {
          this.csendsec[row] =
            new BigByteArray(
              4 * subtableSize * 5 * 2,
              this.globals.useFilesForLargeMemory,
              this.globals.tmpdir
            )
          ;
        }
        this.csendsecBuffer = null;
        this.csendsecBufferIndex = 0;
        this.csendsecRowIndex = 0;
        setState(BackServerRawState.G_I_M_YI_3);
      } break;
      case G_I_M_YI_3: if (true) {
        for (; this.csendsecRowIndex != this.csendsec.length; ++this.csendsecRowIndex) {
          for (; this.csendsecBufferIndex != this.csendsec[this.csendsecRowIndex].getBufferCount(); ++this.csendsecBufferIndex) {
            if (this.csendsecBuffer == null) {
              this.csendsecBuffer = this.csendsec[this.csendsecRowIndex].getBuffer(this.csendsecBufferIndex);
            }
            if (in.readableBytes() < this.csendsecBuffer.remaining()) {
              this.csendsecBuffer.limit(this.csendsecBuffer.position() + in.readableBytes());
              in.readBytes(this.csendsecBuffer);
              this.csendsecBuffer.limit(this.csendsecBuffer.capacity());
              return;
            }
            in.readBytes(this.csendsecBuffer);
            this.csendsecBuffer = null;
          }
          this.csendsecBufferIndex = 0;
        }
        final long subtableSize =
          this.globals.subtableSize.longValue()
        ;
        final int rowCount =
          this.gmShare.gsOt.cells.length
        ;
        final int wColumnCount =
          this.gmShare.gwOt.cells[0].length
        ;
        final BigByteArray insec =
          new BigByteArray(
            4 * subtableSize * 5,
            this.globals.useFilesForLargeMemory,
            this.globals.tmpdir
          )
        ;
        final byte[] u = new byte[1];
        this.PSIShare_U = new BigBitArray[rowCount];
        for (int row = 0; row != rowCount; ++row) {
          for (int i = 0; i != 4; ++i) {
            for (long j = 0; j != subtableSize; ++j) {
              final int c =
                this.C_U[row].getValue(
                  i * subtableSize + j
                )
              ;
              for (int k = 0; k != 5; ++k) {
                this.gmShare.guOt.cells[row][0].getUValue(
                  (i * subtableSize + j) * 5 + k,
                  u,
                  0
                );
                final int y =
                  (c >> k) & 1
                ;
                final byte val =
                  this.csendsec[row].getValue(
                    ((i * subtableSize + j) * 5 + k) * 2 + y
                  )
                ;
                insec.setValue(
                  (i * subtableSize + j) * 5 + k,
                  (byte)(val ^ u[0])
                );
              }
            }
          }
          this.PSIShare_U[row] =
            new BigBitArray(
              4 * subtableSize,
              this.globals.useFilesForLargeMemory,
              this.globals.tmpdir
            )
          ;
          evaluateFullEqualityCircuits(
            insec,
            this.PSIShare_U[row],
            4 * subtableSize
          );
        }
        insec.close();
        this.yselbits = new BigBitArray[rowCount][];
        for (int row = 0; row != rowCount; ++row) {
          this.yselbits[row] = new BigBitArray[wColumnCount];
          for (int col = 0; col != wColumnCount; ++col) {
            this.yselbits[row][col] =
              new BigBitArray(
                4 * subtableSize,
                this.globals.useFilesForLargeMemory,
                this.globals.tmpdir
              )
            ;
            for (int i = 0; i != 4; ++i) {
              for (long j = 0; j != subtableSize; ++j) {
                final int q =
                  this.gmShare.gwOt.cells[row][col].getBValue(
                    i * subtableSize + j
                  )
                ;
                final int p =
                  this.PSIShare_U[row].getValue(
                    i * subtableSize + j
                  )
                ;
                this.yselbits[row][col].setValue(
                  i * subtableSize + j,
                  q ^ p
                );
              }
            }
            final int bufferCount =
              this.yselbits[row][col].getBufferCount()
            ;
            for (int i = 0; i != bufferCount; ++i) {
              this.writeAndFlush(
                ctx,
                Unpooled.wrappedBuffer(
                  this.yselbits[row][col].getBuffer(i)
                )
              );
            }
          }
        }
        this.maskedv = new BigResidueArray[rowCount][];
        for (int row = 0; row != rowCount; ++row) {
          this.maskedv[row] = new BigResidueArray[wColumnCount];
          for (int col = 0; col != wColumnCount; ++col) {
            this.maskedv[row][col] =
              new BigResidueArray(
                4 * subtableSize * 2,
                this.globals.modulus,
                this.globals.useFilesForLargeMemory,
                this.globals.tmpdir
              )
            ;
          }
        }
        this.maskedvBuffer = null;
        this.maskedvBufferIndex = 0;
        this.maskedvColumnIndex = 0;
        this.maskedvRowIndex = 0;
        setState(BackServerRawState.G_I_M_YI_4);
      } break;
      case G_I_M_YI_4: if (true) {
        for (; this.maskedvRowIndex != this.maskedv.length; ++this.maskedvRowIndex) {
          for (; this.maskedvColumnIndex != this.maskedv[this.maskedvRowIndex].length; ++this.maskedvColumnIndex) {
            for (; this.maskedvBufferIndex != this.maskedv[this.maskedvRowIndex][this.maskedvColumnIndex].getBufferCount(); ++this.maskedvBufferIndex) {
              if (this.maskedvBuffer == null) {
                this.maskedvBuffer = this.maskedv[this.maskedvRowIndex][this.maskedvColumnIndex].getBuffer(this.maskedvBufferIndex);
              }
              if (in.readableBytes() < this.maskedvBuffer.remaining()) {
                this.maskedvBuffer.limit(this.maskedvBuffer.position() + in.readableBytes());
                in.readBytes(this.maskedvBuffer);
                this.maskedvBuffer.limit(this.maskedvBuffer.capacity());
                return;
              }
              in.readBytes(this.maskedvBuffer);
              this.maskedvBuffer = null;
            }
            this.maskedvBufferIndex = 0;
          }
          this.maskedvColumnIndex = 0;
        }
        final long subtableSize =
          this.globals.subtableSize.longValue()
        ;
        final int rowCount =
          this.gmShare.gwOt.cells.length
        ;
        final int wColumnCount =
          this.gmShare.gwOt.cells[0].length
        ;
        for (int row = 0; row != rowCount; ++row) {
          for (int col = 0; col != wColumnCount; ++col) {
            BigInteger out_U = BigInteger.ZERO;
            for (int i = 0; i != 4; ++i) {
              for (long j = 0; j != subtableSize; ++j) {
                final int p =
                  this.PSIShare_U[row].getValue(
                    i * subtableSize + j
                  )
                ;
                final BigInteger v =
                  this.maskedv[row][col].getValue(
                    (i * subtableSize + j) * 2 + p
                  )
                ;
                final BigInteger w =
                  this.gmShare.gwOt.cells[row][col].getUValue(
                    i * subtableSize + j
                  )
                ;
                out_U = out_U.add(v.subtract(w));
              }
            }
            this.gmShare.gwOt.cells[row][col].setUValue(
              0,
              out_U.mod(this.globals.modulus)
            );
          }
        }
        this.gmShare.gaPipeline.fireUserEventTriggered(
          BackServerRawHandler.WAKE_UP
        );
        setState(BackServerRawState.G_I_0);
        this.finishQueryLogEntry(in.readableBytes());
        this.cleanup();
      } break;
      case G_W_0: if (true) {
        expect(!in.isReadable());
        this.startQueryLogEntry(in.readableBytes());
        if (this.analysis.equals("freq")) {
          if (this.ivList.size() == 0) {
            throw (InternalErrorStatus)
              new InternalErrorStatus(
              ).initCause(null)
            ;
          }
          final ByteBuf out = ctx.alloc().buffer();
          this.writeQuery(out);
          for (int i = 0; i != this.gfShare.rowCount; ++i) {
            RawUtil.writeBigInteger(out, this.gfShare.summands.get(i));
          }
          this.writeAndFlush(ctx, out);
          this.finishQueryLogEntry(in.readableBytes());
          final PoolEntry<ChannelFuture> pe = this.myChannel;
          this.cleanup();
          setState(BackServerRawState.G_W_0);
          pe.release();
          return;
        } else {
          throw (ParseFailureStatus)
            new ParseFailureStatus(
            ).initCause(null)
          ;
        }
      } break;
      case W_0: if (true) {
        if (!this.readRemoteParty(in)) {
          return;
        }
        if (this.remoteParty == 0) {
          setState(BackServerRawState.W_A_0);
        } else if (this.remoteParty == 1) {
          setState(BackServerRawState.G_I_0);
        } else if (this.remoteParty != this.globals.partyCount - 1) {
          setState(BackServerRawState.W_G_0);
        } else {
          throw (ParseFailureStatus)
            new ParseFailureStatus(
            ).initCause(null)
          ;
        }
      } break;
      case W_A_0: if (true) {
        if (!in.isReadable(4)) {
          return;
        }
        final int type = in.readInt();
        if (type != MessageType.QUERY) {
          throw (ParseFailureStatus)
            new ParseFailureStatus(
            ).initCause(null)
          ;
        }
        this.startQueryLogEntry(4 + in.readableBytes());
        setState(BackServerRawState.W_A_1);
      } break;
      case W_A_1: if (true) {
        if (!in.isReadable(16)) {
          return;
        }
        final byte[] x = new byte[16];
        in.readBytes(x);
        this.queryId = ByteBuffer.wrap(x);
        setState(BackServerRawState.W_A_2);
      } break;
      case W_A_2: if (true) {
        if (!this.readQuery(in)) {
          return;
        }
        if (this.analysis.equals("freq")) {
          this.sqlChannel = null;
          this.sqlResult = null;
          setState(BackServerRawState.W_A_F_0);
        } else {
          final int rowCount = this.getRowCount();
          final int columnCount = this.dvList.size();
          final int bound = this.globals.groupSizeBound;
          this.gsOt =
            new ResidueOtTable(
              rowCount,
              1,
              BlobOtCell.RECEIVER,
              4 * this.globals.subtableSize.longValue() * this.globals.rightBits,
              BigInteger.valueOf(32),
              this.globals.useFilesForLargeMemory,
              this.globals.tmpdir
            )
          ;
          this.guOt =
            new BlobOtTable(
              rowCount,
              1,
              BlobOtCell.RECEIVER,
              4 * this.globals.subtableSize.longValue() * 5,
              1,
              this.globals.useFilesForLargeMemory,
              this.globals.tmpdir
            )
          ;
          this.gwOt =
            new ResidueOtTable(
              rowCount,
              columnCount,
              BlobOtCell.RECEIVER,
              4 * this.globals.subtableSize.longValue(),
              this.globals.modulus,
              this.globals.useFilesForLargeMemory,
              this.globals.tmpdir
            )
          ;
          this.gsOtBBuffer = null;
          this.gsOtBBufferIndex = 0;
          this.gsOtColumnIndex = 0;
          this.gsOtRowIndex = 0;
          this.gsOtUBuffer = null;
          this.gsOtUBufferIndex = 0;
          this.guOtBBuffer = null;
          this.guOtBBufferIndex = 0;
          this.guOtColumnIndex = 0;
          this.guOtRowIndex = 0;
          this.guOtUBuffer = null;
          this.guOtUBufferIndex = 0;
          this.gwOtBBuffer = null;
          this.gwOtBBufferIndex = 0;
          this.gwOtColumnIndex = 0;
          this.gwOtRowIndex = 0;
          this.gwOtUBuffer = null;
          this.gwOtUBufferIndex = 0;
          this.rowIndex = 0;
          this.columnIndex = 0;
          this.vectorIndex = 0;
          setState(BackServerRawState.G_A_M_YI_0);
        }
      } break;
      case W_A_F_0: if (true) {
        if (this.sqlChannel == null) {
          this.getSqlChannel(ctx);
          return;
        }
        if (this.sqlResult == null) {
          final String query =
            new QueryBuilder(
              this.analysis,
              this.loanlinkbool,
              this.globals.domainNames,
              this.dvList,
              this.ivList,
              this.globals.localParty,
              this.remoteParty,
              this.globals.config.get("mysql_table"),
              JdbcSubprotocol.fromString(
                this.globals.config.get("jdbc_subprotocol")
              )
            ).buildQuery()
          ;
          this.getSqlResultSet(ctx, query);
          return;
        }
        this.getWfShare();
        this.wfShare.waPipeline = ctx.pipeline();
        final List<BigInteger> column =
          new ArrayList<BigInteger>(this.wfShare.rowCount)
        ;
        final int j = this.sqlResult.findColumn("freq");
        this.sqlResult.next();
        for (int i = 0; i != this.wfShare.rowCount; ++i) {
          column.add(this.sqlResult.getBigDecimal(j).toBigInteger());
          this.sqlResult.next();
        }
        final int taskIndex = this.globals.localParty - 2;
        this.wfShare.summands.results.set(taskIndex, column);
        this.wfShare.summands.countdown.decrementAndGet();
        this.sqlResult.close();
        this.sqlStatement.close();
        this.sqlChannel.release();
        setState(BackServerRawState.W_A_F_1);
      } break;
      case W_A_F_1: if (true) {
        if (this.wfShare.summands.countdown.get() != 0) {
          return;
        }
        final ByteBuf out = ctx.alloc().buffer();
        for (int i = 0; i != this.wfShare.rowCount; ++i) {
          BigInteger sum = BigInteger.ZERO;
          for (int j = 0; j != this.wfShare.taskCount; ++j) {
            sum = sum.add(this.wfShare.summands.results.get(j).get(i));
          }
          RawUtil.writeBigInteger(out, sum);
        }
        this.writeAndFlush(ctx, out);
        setState(BackServerRawState.W_A_0);
        this.finishQueryLogEntry(in.readableBytes());
        this.cleanup();
      } break;
      case W_G_0: if (true) {
        if (!in.isReadable(4)) {
          return;
        }
        final int type = in.readInt();
        if (type != MessageType.QUERY) {
          throw (ParseFailureStatus)
            new ParseFailureStatus(
            ).initCause(null)
          ;
        }
        this.startQueryLogEntry(4 + in.readableBytes());
        setState(BackServerRawState.W_G_1);
      } break;
      case W_G_1: if (true) {
        if (!in.isReadable(16)) {
          return;
        }
        final byte[] x = new byte[16];
        in.readBytes(x);
        this.queryId = ByteBuffer.wrap(x);
        setState(BackServerRawState.W_G_2);
      } break;
      case W_G_2: if (true) {
        if (!this.readQuery(in)) {
          return;
        }
        if (this.analysis.equals("freq")) {
          setState(BackServerRawState.W_G_F_0);
        } else {
          throw (ParseFailureStatus)
            new ParseFailureStatus(
            ).initCause(null)
          ;
        }
      } break;
      case W_G_F_0: if (true) {
        this.getWfShare();
        this.wgColumn =
          new ArrayList<BigInteger>(this.wfShare.rowCount)
        ;
        this.rowIndex = 0;
        setState(BackServerRawState.W_G_F_1);
      } break;
      case W_G_F_1: if (true) {
        for (; this.rowIndex != this.wfShare.rowCount; ++this.rowIndex) {
          final BigInteger x = RawUtil.readBigInteger(in);
          if (x == null) {
            return;
          }
          this.wgColumn.add(this.globals.modulus.subtract(x));
        }
        final int taskIndex = this.remoteParty - 2;
        this.wfShare.summands.results.set(taskIndex, this.wgColumn);
        if (this.wfShare.summands.countdown.decrementAndGet() == 0) {
          this.wfShare.waPipeline.fireUserEventTriggered(
            BackServerRawHandler.WAKE_UP
          );
        }
        setState(BackServerRawState.W_G_0);
        this.finishQueryLogEntry(in.readableBytes());
        this.cleanup();
      } break;
      default: if (true) {
        throw (MissingCaseStatus)
          new MissingCaseStatus(
          ).initCause(null)
        ;
      } break;
    }
  }
}

private void logConnection(
  final CharSequence s
) {
  final String id = Integer.toHexString(this.hashCode());
  globals.log(id + ": " + s);
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

private static void makeFullEqualityCircuits(
  final BigBitArray outflip,
  final BigByteArray stufftosend,
  final long m
) {
  final int[] seclens =
    new int[] {
      3, 4, 4, 6, 4, 2, 3, 2, 1
    }
  ;
  final int[][] wiring =
    new int[][] {
      new int[] {0, 0},
      new int[] {1, 1},
      new int[] {2, 2},
      new int[] {3, 3},
      new int[] {4, 4},
      new int[] {0, 1},
      new int[] {2, 3},
      new int[] {6, 4},
      new int[] {5, 7},
    }
  ;
  final int[][] equalitytable = new int[9][];
  for (int i = 0; i != 9; ++i) {
    equalitytable[i] = new int[2];
  }
  final int[] zsec = new int[4];
  final int[] osec = new int[4];
  final byte[] randbuf = new byte[13000000];
  int offset = 0;
  for (long i = 0; i != m; ++i) {
    if (i % (randbuf.length / 13) == 0) {
      Rand.bytes(
        Rand.defaultSrc(),
        ByteBuffer.wrap(randbuf),
        (int)Math.min(randbuf.length / 13, m - i) * 13
      );
      offset = 0;
    }
    outflip.setValue(i, randbuf[offset] & 1);
    offset += 1;
    equalitytable[8][0] = outflip.getValue(i) ^ 0;
    equalitytable[8][1] = outflip.getValue(i) ^ 1;
    for (int j = 8; j != 4; --j) {
      zsec[0] = equalitytable[j][0];
      zsec[1] = equalitytable[j][0];
      zsec[2] = equalitytable[j][0];
      zsec[3] = equalitytable[j][1];
      encodegate(
        seclens[j],
        zsec,
        osec,
        randbuf[offset + 0],
        randbuf[offset + 1],
        randbuf[offset + 2]
      );
      offset += 3;
      equalitytable[wiring[j][0]][0] = osec[0];
      equalitytable[wiring[j][0]][1] = osec[1];
      equalitytable[wiring[j][1]][0] = osec[2];
      equalitytable[wiring[j][1]][1] = osec[3];
    }
    for (int j = 0; j != 5; ++j) {
      stufftosend.setValue(
        (i * 5 + j) * 2 + 0,
        (byte)equalitytable[j][0]
      );
      stufftosend.setValue(
        (i * 5 + j) * 2 + 1,
        (byte)equalitytable[j][1]
      );
    }
  }
}

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

private final boolean readQuery(
  final ByteBuf in
) {
  if (!in.isReadable(4)) {
    return false;
  }
  in.markReaderIndex();
  final int n = in.readInt();
  if (n < 0) {
    throw (ParseFailureStatus)
      new ParseFailureStatus(
      ).initCause(null)
    ;
  }
  if (!in.isReadable(n)) {
    in.resetReaderIndex();
    return false;
  }
  final int n0 = in.readerIndex();
  this.analysis = RawUtil.readString(in);
  if (this.analysis == null) {
    throw (ParseFailureStatus)
      new ParseFailureStatus(
      ).initCause(null)
    ;
  }
  this.loanlinkbool = (in.readInt() != 0);
  final int ivListSize = in.readInt();
  if (ivListSize < 0) {
    throw (ParseFailureStatus)
      new ParseFailureStatus(
      ).initCause(null)
    ;
  }
  this.ivList = new ArrayList<String>(ivListSize);
  for (int i = 0; i != ivListSize; ++i) {
    final String iv = RawUtil.readString(in);
    if (iv == null) {
      throw (ParseFailureStatus)
        new ParseFailureStatus(
        ).initCause(null)
      ;
    }
    this.ivList.add(iv);
  }
  this.ivList = Collections.unmodifiableList(this.ivList);
  final int dvListSize = in.readInt();
  if (dvListSize < 0) {
    throw (ParseFailureStatus)
      new ParseFailureStatus(
      ).initCause(null)
    ;
  }
  this.dvList = new ArrayList<String>(dvListSize);
  for (int i = 0; i != dvListSize; ++i) {
    final String dv = RawUtil.readString(in);
    if (dv == null) {
      throw (ParseFailureStatus)
        new ParseFailureStatus(
        ).initCause(null)
      ;
    }
    this.dvList.add(dv);
  }
  this.dvList = Collections.unmodifiableList(this.dvList);
  final int n1 = in.readerIndex();
  if (n1 - n0 != n) {
    throw (ParseFailureStatus)
      new ParseFailureStatus(
      ).initCause(null)
    ;
  }
  return true;
}

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

public final boolean readRemoteParty(
  final ByteBuf in
) {
  if (!in.isReadable(8)) {
    return false;
  }
  final int type = in.readInt();
  if (type != MessageType.ID) {
    throw (ParseFailureStatus)
      new ParseFailureStatus(
      ).initCause(null)
    ;
  }
  this.remoteParty = in.readInt();
  if (this.remoteParty < 0) {
    throw (ParseFailureStatus)
      new ParseFailureStatus(
      ).initCause(null)
    ;
  }
  if (this.remoteParty >= this.globals.localParty) {
    throw (ParseFailureStatus)
      new ParseFailureStatus(
      ).initCause(null)
    ;
  }
  return true;
}

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

private final void startQueryLogEntry(
  final int readableBytes
) {
  this.queryBytesReceived = readableBytes - this.bytesReceived;
  this.queryBytesSent = -this.bytesSent;
  this.queryNanoseconds = -System.nanoTime();
}

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

@Override
public final void userEventTriggered(
  final ChannelHandlerContext ctx,
  final Object evt
) throws
  Exception
{
  if (evt == BackServerRawHandler.WAKE_UP) {
    final ByteBuf in = ctx.alloc().buffer();
    this.handle(ctx, in);
    in.release();
    return;
  }
  if (BuildConfig.ENABLE_INTERNAL_ERROR_CHECKS) {
    throw (MissingCaseStatus)
      new MissingCaseStatus(
      ).initCause(null)
    ;
  }
}

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

private final ChannelFuture writeAndFlush(
  final ChannelHandlerContext ctx,
  final ByteBuf out
) {
  this.bytesSent += out.readableBytes();
  return ctx.writeAndFlush(out);
}

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

private final void writeQuery(
  final ByteBuf out
) {
  out.writeInt(MessageType.QUERY);
  out.writeBytes(this.queryId.array());
  final int i = out.writerIndex();
  out.writeZero(4);
  RawUtil.writeString(out, this.analysis);
  out.writeInt((this.loanlinkbool) ? 1 : 0);
  out.writeInt(this.ivList.size());
  for (final String iv : this.ivList) {
    RawUtil.writeString(out, iv);
  }
  out.writeInt(this.dvList.size());
  for (final String dv : this.dvList) {
    RawUtil.writeString(out, dv);
  }
  final int j = out.writerIndex();
  out.writerIndex(i);
  out.writeInt(j - i - 4);
  out.writerIndex(j);
}

}
