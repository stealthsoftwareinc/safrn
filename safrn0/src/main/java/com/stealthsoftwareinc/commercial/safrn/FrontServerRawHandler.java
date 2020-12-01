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
 * <code>com.stealthsoftwareinc.commercial.safrn.FrontServerRawHandler</code>
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
import com.stealthsoftwareinc.commercial.safrn.BuildConfig;
import com.stealthsoftwareinc.commercial.safrn.FrontServerGlobals;
import com.stealthsoftwareinc.commercial.safrn.FrontServerQueryStart;
import com.stealthsoftwareinc.commercial.safrn.FrontServerRawState;
import com.stealthsoftwareinc.commercial.safrn.ResidueOtTable;
import com.stealthsoftwareinc.commercial.safrn.MessageType;
import com.stealthsoftwareinc.commercial.safrn.RawUtil;
import com.stealthsoftwareinc.sst.InternalErrorStatus;
import com.stealthsoftwareinc.sst.LimitExceededStatus;
import com.stealthsoftwareinc.sst.Memory;
import com.stealthsoftwareinc.sst.MissingCaseStatus;
import com.stealthsoftwareinc.sst.NotImplementedStatus;
import com.stealthsoftwareinc.sst.NullPointerStatus;
import com.stealthsoftwareinc.sst.ParseFailureStatus;
import com.stealthsoftwareinc.sst.PoolEntry;
import io.netty.buffer.ByteBuf;
import io.netty.buffer.Unpooled;
import io.netty.channel.ChannelFuture;
import io.netty.channel.ChannelFutureListener;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.ChannelInboundHandlerAdapter;
import io.netty.util.ReferenceCountUtil;
import java.io.PrintStream;
import java.lang.Exception;
import java.lang.Object;
import java.lang.System;
import java.lang.Throwable;
import java.math.BigInteger;
import java.nio.ByteBuffer;
import java.time.LocalTime;

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

public final class FrontServerRawHandler
extends ChannelInboundHandlerAdapter
{

/**
 * A running count of the number of bytes received.
 * <p>
 * This variable is initialized to zero and is incremented by
 * <code>in.readableBytes()</code>
 * each time the
 * <code>{@link com.stealthsoftwareinc.commercial.safrn.FrontServerRawHandler.channelRead(ChannelHandlerContext, Object)}</code>
 * method is called, keeping a running count of the number of bytes
 * received over the lifetime of this handler.
 * To determine the number of bytes received between two points in the
 * <code>{@link com.stealthsoftwareinc.commercial.safrn.FrontServerRawHandler.handle(ChannelHandlerContext, ByteBuf)}</code>
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

private int columnIndex;

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

private final FrontServerGlobals globals;

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

public FrontServerQueryStart queryStart;

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

private FrontServerRawState state;

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

public FrontServerRawHandler(
  final FrontServerGlobals globals
) {
  if (globals == null) {
    throw (NullPointerStatus)
      new NullPointerStatus(
        "globals is a null reference"
      ).initCause(null)
    ;
  }
  this.bytesReceived = 0;
  this.bytesSent = 0;
  this.columnIndex = 0;
  this.globals = globals;
  this.ignoreEverything = false;
  this.queryBytesReceived = 0;
  this.queryBytesSent = 0;
  this.queryNanoseconds = 0;
  this.queryStart = null;
  this.rowIndex = 0;
  this.sponge1 = Unpooled.buffer();
  this.sponge2 = Unpooled.buffer();
  setState(FrontServerRawState.A_0);
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
      (
        (queryStart == null) ?
          "?"
        :
          String.valueOf(queryStart.remoteParty)
      ) +
      ": " +
      message.toString()
    );
  }
}

private void setState(
  final FrontServerRawState newState
) {
  state = newState;
  log(state.name());
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
  if (this.queryStart != null) {
    if (this.queryStart.gsOt != null) {
      try {
        this.queryStart.gsOt.close();
      } catch (final Exception e) {
      }
    }
    if (this.queryStart.guOt != null) {
      try {
        this.queryStart.guOt.close();
      } catch (final Exception e) {
      }
    }
    if (this.queryStart.gwOt != null) {
      try {
        this.queryStart.gwOt.close();
      } catch (final Exception e) {
      }
    }
    if (this.queryStart.isOts != null) {
      for (int i = 0; i != this.queryStart.isOts.length; ++i) {
        if (this.queryStart.isOts[i] != null) {
          try {
            this.queryStart.isOts[i].close();
          } catch (final Exception e) {
          }
        }
      }
    }
    if (this.queryStart.iuOts != null) {
      for (int i = 0; i != this.queryStart.iuOts.length; ++i) {
        if (this.queryStart.iuOts[i] != null) {
          try {
            this.queryStart.iuOts[i].close();
          } catch (final Exception e) {
          }
        }
      }
    }
    if (this.queryStart.iwOts != null) {
      for (int i = 0; i != this.queryStart.iwOts.length; ++i) {
        if (this.queryStart.iwOts[i] != null) {
          try {
            this.queryStart.iwOts[i].close();
          } catch (final Exception e) {
          }
        }
      }
    }
    this.queryStart = null;
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
  globals.log(
    "queryId = " + Memory.toHexString(this.queryStart.queryId) + ", " +
    "localParty = 0, " +
    "remoteParty = " + this.queryStart.remoteParty + ", " +
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
  globals.log(LocalTime.now()+Constants.DELIMETER+Constants.LATENCY+Constants.DELIMETER+this.getClass().getSimpleName()+Constants.DELIMETER+"handle()"
  +Constants.DELIMETER+this.state.name() +Constants.DELIMETER+"Id()="+Thread.currentThread().getId());
  for (;;) {
	  globals.log(LocalTime.now()+Constants.DELIMETER+Constants.LATENCY+Constants.DELIMETER+this.getClass().getSimpleName()+Constants.DELIMETER+"inside for loop"+
  Constants.DELIMETER+this.state.name() +Constants.DELIMETER+"Id()="+Thread.currentThread().getId()
				+Constants.DELIMETER+"DUMMY1"+Constants.DELIMETER+"DUMMY2"+Constants.DELIMETER+"tsFrontServerRawHandler="+System.currentTimeMillis()); 
    switch (this.state) {
      case A_0: if (true) {
        if (in.isReadable()) {
          throw (ParseFailureStatus)
            new ParseFailureStatus(
            ).initCause(null)
          ;
        }
        final ByteBuf out = ctx.alloc().buffer();
        out.writeInt(MessageType.ID);
        out.writeInt(0);
        this.writeAndFlush(ctx, out);
        if (this.queryStart.remoteParty == 1) {
          setState(FrontServerRawState.A_I_0);
        } else {
          setState(FrontServerRawState.A_G_0);
        }
      } break;
      case A_I_0: if (true) {
        if (in.isReadable()) {
          throw (ParseFailureStatus)
            new ParseFailureStatus(
            ).initCause(null)
          ;
        }
        this.startQueryLogEntry(in.readableBytes());
        final ByteBuf out = ctx.alloc().buffer();
        this.writeQuery(out);
        this.writeAndFlush(ctx, out);
        if (this.queryStart.analysis.equals("freq")) {
          if (this.queryStart.ivList.size() != 0) {
            throw (InternalErrorStatus)
              new InternalErrorStatus(
              ).initCause(null)
            ;
          }
          setState(FrontServerRawState.A_I_F_0);
        } else {
          if (this.queryStart.ivList.size() == 0) {
            this.columnIndex = 0;
            setState(FrontServerRawState.A_I_M_NI_0);
          } else {
            setState(FrontServerRawState.A_I_M_YI_0);
          }
        }
      } break;
      case A_I_F_0: if (true) {
        final BigInteger summand = RawUtil.readBigInteger(in);
        if (summand == null) {
          return;
        }
        this.queryStart.queryDone.summands.get(0).set(0, summand);
        this.queryStart.httpPipeline.fireUserEventTriggered(this.queryStart.queryDone);
        final PoolEntry<ChannelFuture> pe = this.queryStart.rawChannel;
        setState(FrontServerRawState.A_I_0);
        this.finishQueryLogEntry(in.readableBytes());
        this.cleanup();
        pe.release();
        return;
      } break;
      case A_I_M_NI_0: if (true) {
        final int columnCount = this.queryStart.queryDone.summands.get(0).size();
        for (; this.columnIndex != columnCount; ++this.columnIndex) {
          final BigInteger summand = RawUtil.readBigInteger(in);
          if (summand == null) {
            return;
          }
          this.queryStart.queryDone.summands.get(0).set(this.columnIndex, summand);
        }
        this.queryStart.httpPipeline.fireUserEventTriggered(this.queryStart.queryDone);
        final PoolEntry<ChannelFuture> pe = this.queryStart.rawChannel;
        setState(FrontServerRawState.A_I_0);
        this.finishQueryLogEntry(in.readableBytes());
        this.cleanup();
        pe.release();
        return;
      } break;
      case A_I_M_YI_0: if (true) {
        final ResidueOtTable[] isOts = this.queryStart.isOts;
        for (int p = 0; p != isOts.length; ++p) {
          for (int i = 0; i != isOts[p].cells.length; ++i) {
            for (int j = 0; j != isOts[p].cells[i].length; ++j) {
              {
                final int bufferCount =
                  isOts[p].cells[i][j].getSBufferCount()
                ;
                for (int k = 0; k != bufferCount; ++k) {
                  this.writeAndFlush(
                    ctx,
                    Unpooled.wrappedBuffer(
                      isOts[p].cells[i][j].getSBuffer(k)
                    )
                  );
                }
              }
            }
          }
        }
        final BlobOtTable[] iuOts = this.queryStart.iuOts;
        for (int p = 0; p != iuOts.length; ++p) {
          for (int i = 0; i != iuOts[p].cells.length; ++i) {
            for (int j = 0; j != iuOts[p].cells[i].length; ++j) {
              {
                final int bufferCount =
                  iuOts[p].cells[i][j].getSBufferCount()
                ;
                for (int k = 0; k != bufferCount; ++k) {
                  this.writeAndFlush(
                    ctx,
                    Unpooled.wrappedBuffer(
                      iuOts[p].cells[i][j].getSBuffer(k)
                    )
                  );
                }
              }
            }
          }
        }
        final ResidueOtTable[] iwOts = this.queryStart.iwOts;
        for (int p = 0; p != iwOts.length; ++p) {
          for (int i = 0; i != iwOts[p].cells.length; ++i) {
            for (int j = 0; j != iwOts[p].cells[i].length; ++j) {
              {
                final int bufferCount =
                  iwOts[p].cells[i][j].getSBufferCount()
                ;
                for (int k = 0; k != bufferCount; ++k) {
                  this.writeAndFlush(
                    ctx,
                    Unpooled.wrappedBuffer(
                      iwOts[p].cells[i][j].getSBuffer(k)
                    )
                  );
                }
              }
            }
          }
        }
        this.rowIndex = 0;
        this.columnIndex = 0;
        setState(FrontServerRawState.A_I_M_YI_1);
      } break;
      case A_I_M_YI_1: if (true) {
        final int rowCount = this.queryStart.queryDone.summands.size();
        final int columnCount = this.queryStart.queryDone.summands.get(0).size();
        for (; this.rowIndex != rowCount; ++this.rowIndex) {
          for (; this.columnIndex != columnCount; ++this.columnIndex) {
            final BigInteger summand = RawUtil.readBigInteger(in);
            if (summand == null) {
              return;
            }
            this.queryStart.queryDone.summands.get(this.rowIndex).set(this.columnIndex, summand);
          }
          this.columnIndex = 0;
        }
        this.queryStart.httpPipeline.fireUserEventTriggered(this.queryStart.queryDone);
        final PoolEntry<ChannelFuture> pe = this.queryStart.rawChannel;
        setState(FrontServerRawState.A_I_0);
        this.finishQueryLogEntry(in.readableBytes());
        this.cleanup();
        pe.release();
        return;
      } break;
      case A_G_0: if (true) {
        if (in.isReadable()) {
          throw (ParseFailureStatus)
            new ParseFailureStatus(
            ).initCause(null)
          ;
        }
        if (this.queryStart.ivList.size() == 0) {
          throw (InternalErrorStatus)
            new InternalErrorStatus(
            ).initCause(null)
          ;
        }
        this.startQueryLogEntry(in.readableBytes());
        final ByteBuf out = ctx.alloc().buffer();
        this.writeQuery(out);
        this.writeAndFlush(ctx, out);
        if (this.queryStart.analysis.equals("freq")) {
          this.rowIndex = 0;
          setState(FrontServerRawState.A_G_F_0);
        } else {
          setState(FrontServerRawState.A_G_M_YI_0);
        }
      } break;
      case A_G_F_0: if (true) {
        final int rows = this.queryStart.queryDone.summands.size();
        for (; this.rowIndex != rows; ++this.rowIndex) {
          final BigInteger summand = RawUtil.readBigInteger(in);
          if (summand == null) {
            return;
          }
          this.queryStart.queryDone.summands.get(this.rowIndex).set(0, summand);
        }
        this.queryStart.httpPipeline.fireUserEventTriggered(this.queryStart.queryDone);
        final PoolEntry<ChannelFuture> pe = this.queryStart.rawChannel;
        setState(FrontServerRawState.A_G_0);
        this.finishQueryLogEntry(in.readableBytes());
        this.cleanup();
        pe.release();
        return;
      } break;
      case A_G_M_YI_0: if (true) {
        final ResidueOtTable gsOt = this.queryStart.gsOt;
        for (int i = 0; i != gsOt.cells.length; ++i) {
          for (int j = 0; j != gsOt.cells[i].length; ++j) {
            {
              final int bufferCount =
                gsOt.cells[i][j].getBBufferCount()
              ;
              for (int k = 0; k != bufferCount; ++k) {
                this.writeAndFlush(
                  ctx,
                  Unpooled.wrappedBuffer(
                    gsOt.cells[i][j].getBBuffer(k)
                  )
                );
              }
            }
            {
              final int bufferCount =
                gsOt.cells[i][j].getUBufferCount()
              ;
              for (int k = 0; k != bufferCount; ++k) {
                this.writeAndFlush(
                  ctx,
                  Unpooled.wrappedBuffer(
                    gsOt.cells[i][j].getUBuffer(k)
                  )
                );
              }
            }
          }
        }
        final BlobOtTable guOt = this.queryStart.guOt;
        for (int i = 0; i != guOt.cells.length; ++i) {
          for (int j = 0; j != guOt.cells[i].length; ++j) {
            {
              final int bufferCount =
                guOt.cells[i][j].getBBufferCount()
              ;
              for (int k = 0; k != bufferCount; ++k) {
                this.writeAndFlush(
                  ctx,
                  Unpooled.wrappedBuffer(
                    guOt.cells[i][j].getBBuffer(k)
                  )
                );
              }
            }
            {
              final int bufferCount =
                guOt.cells[i][j].getUBufferCount()
              ;
              for (int k = 0; k != bufferCount; ++k) {
                this.writeAndFlush(
                  ctx,
                  Unpooled.wrappedBuffer(
                    guOt.cells[i][j].getUBuffer(k)
                  )
                );
              }
            }
          }
        }
        final ResidueOtTable gwOt = this.queryStart.gwOt;
        for (int i = 0; i != gwOt.cells.length; ++i) {
          for (int j = 0; j != gwOt.cells[i].length; ++j) {
            {
              final int bufferCount =
                gwOt.cells[i][j].getBBufferCount()
              ;
              for (int k = 0; k != bufferCount; ++k) {
                this.writeAndFlush(
                  ctx,
                  Unpooled.wrappedBuffer(
                    gwOt.cells[i][j].getBBuffer(k)
                  )
                );
              }
            }
            {
              final int bufferCount =
                gwOt.cells[i][j].getUBufferCount()
              ;
              for (int k = 0; k != bufferCount; ++k) {
                this.writeAndFlush(
                  ctx,
                  Unpooled.wrappedBuffer(
                    gwOt.cells[i][j].getUBuffer(k)
                  )
                );
              }
            }
          }
        }
        this.rowIndex = 0;
        this.columnIndex = 0;
        setState(FrontServerRawState.A_G_M_YI_1);
      } break;
      case A_G_M_YI_1: if (true) {
        final int rowCount = this.queryStart.queryDone.summands.size();
        final int columnCount = this.queryStart.queryDone.summands.get(0).size();
        for (; this.rowIndex != rowCount; ++this.rowIndex) {
          for (; this.columnIndex != columnCount; ++this.columnIndex) {
            final BigInteger summand = RawUtil.readBigInteger(in);
            if (summand == null) {
              return;
            }
            this.queryStart.queryDone.summands.get(this.rowIndex).set(this.columnIndex, summand);
          }
          this.columnIndex = 0;
        }
        this.queryStart.httpPipeline.fireUserEventTriggered(this.queryStart.queryDone);
        final PoolEntry<ChannelFuture> pe = this.queryStart.rawChannel;
        setState(FrontServerRawState.A_G_0);
        this.finishQueryLogEntry(in.readableBytes());
        this.cleanup();
        pe.release();
        return;
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
  if (evt instanceof FrontServerQueryStart) {
    this.queryStart = (FrontServerQueryStart)evt;
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

private final void writeAndFlush(
  final ChannelHandlerContext ctx,
  final ByteBuf out
) {
  this.bytesSent += out.readableBytes();
  ctx.writeAndFlush(out);
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
  out.writeBytes(this.queryStart.queryId);
  final int i = out.writerIndex();
  out.writeZero(4);
  RawUtil.writeString(out, this.queryStart.analysis);
  out.writeInt((this.queryStart.loanlinkbool) ? 1 : 0);
  out.writeInt(this.queryStart.ivList.size());
  for (final FrontServerIv iv : this.queryStart.ivList) {
    RawUtil.writeString(out, iv.name);
  }
  out.writeInt(this.queryStart.dvList.size());
  for (final FrontServerDv dv : this.queryStart.dvList) {
    RawUtil.writeString(out, dv.name);
  }
  final int j = out.writerIndex();
  out.writerIndex(i);
  out.writeInt(j - i - 4);
  out.writerIndex(j);
}

}
