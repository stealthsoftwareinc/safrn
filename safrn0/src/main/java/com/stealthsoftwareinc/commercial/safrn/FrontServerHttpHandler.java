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
 * <code>com.stealthsoftwareinc.commercial.safrn.FrontServerHttpHandler</code>
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
import com.stealthsoftwareinc.commercial.safrn.BlobOtTable;
import com.stealthsoftwareinc.commercial.safrn.BuildConfig;
import com.stealthsoftwareinc.commercial.safrn.FrontServerGlobals;
import com.stealthsoftwareinc.commercial.safrn.FrontServerHttpQueryResponse;
import com.stealthsoftwareinc.commercial.safrn.FrontServerHttpResponse;
import com.stealthsoftwareinc.commercial.safrn.FrontServerQueryDone;
import com.stealthsoftwareinc.commercial.safrn.FrontServerQueryStart;
import com.stealthsoftwareinc.commercial.safrn.ResidueOtTable;
import com.stealthsoftwareinc.sst.Json;
import com.stealthsoftwareinc.sst.MissingCaseStatus;
import com.stealthsoftwareinc.sst.NullPointerStatus;
import com.stealthsoftwareinc.sst.PoolEntry;
import com.stealthsoftwareinc.sst.Uris;
import io.netty.buffer.ByteBuf;
import io.netty.buffer.Unpooled;
import io.netty.channel.ChannelFuture;
import io.netty.channel.ChannelFutureListener;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.ChannelInboundHandlerAdapter;
import io.netty.channel.ChannelPipeline;
import io.netty.handler.codec.http.DefaultFullHttpResponse;
import io.netty.handler.codec.http.FullHttpResponse;
import io.netty.handler.codec.http.HttpContent;
import io.netty.handler.codec.http.HttpHeaderNames;
import io.netty.handler.codec.http.HttpObject;
import io.netty.handler.codec.http.HttpRequest;
import io.netty.handler.codec.http.HttpResponse;
import io.netty.handler.codec.http.HttpResponseStatus;
import io.netty.handler.codec.http.HttpUtil;
import io.netty.handler.codec.http.HttpVersion;
import io.netty.handler.codec.http.LastHttpContent;
import io.netty.util.ReferenceCountUtil;
import java.io.PrintWriter;
import java.lang.Exception;
import java.lang.Integer;
import java.lang.Object;
import java.lang.Process;
import java.lang.ProcessBuilder;
import java.lang.String;
import java.lang.StringBuilder;
import java.lang.Throwable;
import java.math.BigInteger;
import java.net.URI;
import java.net.URISyntaxException;
import java.nio.CharBuffer;
import java.nio.charset.Charset;
import java.nio.charset.CharsetEncoder;
import java.nio.charset.CodingErrorAction;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.security.SecureRandom;
import java.time.LocalTime;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Random;
import java.util.Set;

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

public final class FrontServerHttpHandler
extends ChannelInboundHandlerAdapter
{
	

private long tsAPIStart; //curl/api start
private long tsAPIEnd; //curt/api end

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

private boolean doneReading;

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
 * @since SAFRN&nbsp;0.1.0 (2017-11-08)
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

private boolean ignoreContents;

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

private boolean keepAlive;

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

private HttpRequest request;

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

private String requestPath;

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

private String requestQuery;

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

private final StringBuilder responseBuilder;

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

private final CharsetEncoder responseEncoder;

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

private final ArrayList<FrontServerHttpResponse> responses;

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

private int responsesSent;

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

public FrontServerHttpHandler(
  final FrontServerGlobals globals
) {
  if (globals == null) {
    throw (NullPointerStatus)
      new NullPointerStatus(
        "globals is a null reference"
      ).initCause(null)
    ;
  }
  this.doneReading = false;
  this.globals = globals;
  this.ignoreContents = false;
  this.keepAlive = true;
  this.request = null;
  this.requestPath = null;
  this.requestQuery = null;
  this.responseBuilder = new StringBuilder();
  this.responseEncoder = Charset.forName("UTF-8").newEncoder();
  this.responseEncoder.onMalformedInput(CodingErrorAction.REPORT);
  this.responseEncoder.onUnmappableCharacter(CodingErrorAction.REPORT);
  this.responses = new ArrayList<FrontServerHttpResponse>();
  this.responsesSent = 0;
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
  final HttpObject object = (HttpObject)msg;
  try {
    this.handle(ctx, object);
  } catch (final Exception e1) {
    try {
      ReferenceCountUtil.release(object);
    } catch (final Exception e2) {
    }
    throw e1;
  }
  ReferenceCountUtil.release(object);
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
public final void channelReadComplete(
  final ChannelHandlerContext ctx
) throws
  Exception
{
  this.doneReading = true;
  if (this.keepAlive) {
    if (this.responsesSent == this.responses.size()) {
      this.close(ctx);
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

private final void close(
  final ChannelHandlerContext ctx
) {
  final ByteBuf empty = Unpooled.EMPTY_BUFFER;
  final ChannelFuture future = ctx.writeAndFlush(empty);
  future.addListener(ChannelFutureListener.CLOSE);
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


private final void handle(
  final ChannelHandlerContext ctx,
  final HttpObject object
) throws
  Exception
{
  if (!this.keepAlive) {
    return;
  }
  
  tsAPIStart = System.currentTimeMillis();
  globals.log(LocalTime.now()+Constants.DELIMETER+Constants.LATENCY+Constants.DELIMETER+this.getClass().getSimpleName()+Constants.DELIMETER+
		  "tsAPIStart = "+tsAPIStart);
  if (object instanceof HttpRequest) {
    this.request = (HttpRequest)object;
    this.ignoreContents = false;
    final URI requestUri;
    try {
      requestUri = new URI(this.request.uri());
    } catch (final URISyntaxException e) {
      this.handleMalformedUri(ctx, e);
      return;
    }
    this.requestPath = requestUri.getPath();
    this.requestQuery = requestUri.getRawQuery();
  }
  if (object instanceof LastHttpContent) {
    this.keepAlive = HttpUtil.isKeepAlive(this.request);
  }
  if (this.ignoreContents) {
    return;
  }
  if (this.requestPath.equals("/query")) {
    this.handleQuery(ctx, object);
    return;
  }
  this.handlePathNotFound(ctx);
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

private final void handleMalformedUri(
  final ChannelHandlerContext ctx,
  final URISyntaxException e
) throws
  Exception
{
  final DefaultFullHttpResponse fullResponse =
    new DefaultFullHttpResponse(
      this.request.protocolVersion(),
      HttpResponseStatus.BAD_REQUEST,
      ctx.alloc().buffer(),
      true
    )
  ;
  this.responseBuilder.setLength(0);
  this.responseBuilder.append(
    fullResponse.status().code()
  );
  this.responseBuilder.append(
    " "
  );
  this.responseBuilder.append(
    fullResponse.status().reasonPhrase()
  );
  this.responseBuilder.append(
    "\r\n"
  );
  fullResponse.content().writeBytes(
    this.responseEncoder.encode(
      CharBuffer.wrap(this.responseBuilder)
    )
  );
  HttpUtil.setKeepAlive(
    fullResponse,
    this.keepAlive
  );
  fullResponse.headers().set(
    HttpHeaderNames.ACCESS_CONTROL_ALLOW_ORIGIN,
    "*"
  );
  fullResponse.headers().set(
    HttpHeaderNames.CONTENT_LENGTH,
    fullResponse.content().readableBytes()
  );
  fullResponse.headers().set(
    HttpHeaderNames.CONTENT_TYPE,
    "text/plain; charset=UTF-8"
  );
  final FrontServerHttpResponse response =
    new FrontServerHttpResponse(
      fullResponse
    )
  ;
  response.done = true;
  this.responses.add(response);
  this.sendResponses(ctx);
  this.ignoreContents = true;
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

private final void handlePathNotFound(
  final ChannelHandlerContext ctx
) throws
  Exception
{
  final DefaultFullHttpResponse fullResponse =
    new DefaultFullHttpResponse(
      this.request.protocolVersion(),
      HttpResponseStatus.NOT_FOUND,
      ctx.alloc().buffer(),
      true
    )
  ;
  this.responseBuilder.setLength(0);
  this.responseBuilder.append(
    fullResponse.status().code()
  );
  this.responseBuilder.append(
    " "
  );
  this.responseBuilder.append(
    fullResponse.status().reasonPhrase()
  );
  this.responseBuilder.append(
    "\r\n"
  );
  fullResponse.content().writeBytes(
    this.responseEncoder.encode(
      CharBuffer.wrap(this.responseBuilder)
    )
  );
  HttpUtil.setKeepAlive(
    fullResponse,
    this.keepAlive
  );
  fullResponse.headers().set(
    HttpHeaderNames.ACCESS_CONTROL_ALLOW_ORIGIN,
    "*"
  );
  fullResponse.headers().set(
    HttpHeaderNames.CONTENT_LENGTH,
    fullResponse.content().readableBytes()
  );
  fullResponse.headers().set(
    HttpHeaderNames.CONTENT_TYPE,
    "text/plain; charset=UTF-8"
  );
  final FrontServerHttpResponse response =
    new FrontServerHttpResponse(
      fullResponse
    )
  ;
  response.done = true;
  this.responses.add(response);
  this.sendResponses(ctx);
  this.ignoreContents = true;
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

private final void handleQuery(
  final ChannelHandlerContext ctx
) throws
  Exception
{
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

private final void handleQuery(
  final ChannelHandlerContext ctx,
  final HttpContent content
) throws
  Exception
{
  if (content instanceof LastHttpContent) {
    final DefaultFullHttpResponse fullResponse =
      new DefaultFullHttpResponse(
        this.request.protocolVersion(),
        HttpResponseStatus.OK,
        ctx.alloc().buffer(),
        true
      )
    ;
    this.responseBuilder.setLength(0);
    {
      //timing info
      final long tsFrontServerStart = System.currentTimeMillis();
      globals.log(LocalTime.now()+Constants.DELIMETER+Constants.LATENCY+Constants.DELIMETER+this.getClass().getSimpleName()+Constants.DELIMETER
    		  +"tsFrontServerStart="+tsFrontServerStart);
      //final long start_sec = start / 1000;
      //final long start_millis = start % 1000;
      //URI info
      final String q = this.requestQuery;
      //debug info
      String errorMsg = "";

      try {
        globals.log("Received request " + this.request.method().toString() + " " + q);

        // Parse URL Parameters.
        String[] seg = (q == null ? "" : q).split("&", -1); //split out each &
        String[] pair;
        String[] cols;
        StringBuilder dr = new StringBuilder();

        final List<String> colnames = new ArrayList<String>();
        final List<String> validdvs = new ArrayList<String>();
        final List<String> validivs = new ArrayList<String>();
        for (int i = 0; i != this.globals.dvList.size(); ++i) {
          colnames.add(this.globals.dvList.get(i).name);
          validdvs.add(this.globals.dvList.get(i).name);
        }
        for (int i = 0; i != this.globals.ivList.size(); ++i) {
          colnames.add(this.globals.ivList.get(i).name);
          validivs.add(this.globals.ivList.get(i).name);
        }

        boolean success = true;

        final String analysis;
        final List<Integer> iv;
        final List<Integer> dv;
        boolean loanlinkbool = false;
        int countonlymeanint = 0;
        {
          String myAnalysis = "";
          List<Integer> myIv = new ArrayList<Integer>();
          List<Integer> myDv = new ArrayList<Integer>();
          boolean optfound[] = new boolean[opt.length];
          boolean lhsvalid;
          for (int i = 0; i < seg.length; i++) {
            if (seg[i].contentEquals("")) {
              errorMsg += "ERROR: Missing parameter? \\n";
              success = false;
              break;
            }
            pair = seg[i].split("=", 2); //for each &, if it's not empty split out the =
            pair[0] = Uris.decode(pair[0]).toString();
            if (pair.length != 2) {
              errorMsg += "ERROR: Missing = after " + seg[i] + "\\n";
              success = false;
              break;
            }
            lhsvalid = false;
            int j;
            for (j = 0; j < opt.length; j++) { //find the option it matches and mark it
              if (pair[0].equals(opt[j])) {
                if (optfound[j] == false) {
                  optfound[j] = true;
                  lhsvalid = true;
                }
                break;
              }
            }
            if (lhsvalid == false) {
              errorMsg += "ERROR: Invalid or duplicated option: " + pair[0] + "\\n";
              success = false;
              break;
            }
            switch (j) {
              // analysis
              case 0: if (true) {
                pair[1] = Uris.decode(pair[1]).toString();
                if (pair[1].equals("mean") || pair[1].equals("freq")) {
                  myAnalysis = pair[1];
                } else {
                  errorMsg += "ERROR: Must use mean or freq for analysis \\n";
                  success = false;
                }
              } break;
              // iv, grab the column index (not name) into the iv list
              case 1: if (true) {
                cols = pair[1].split("\\+", -1);
                for (int k = 0; k < cols.length; k++) {
                  cols[k] = Uris.decode(cols[k]).toString();
                  if (myIv.contains(colnames.indexOf(cols[k]))) {
                    errorMsg += "ERROR: Duplicated independent variable " + cols[k] + " \\n";
                    success = false;
                    break;
                  }
                  if (validivs.contains(cols[k])) {
                    myIv.add(colnames.indexOf(cols[k]));
                  } else {
                    errorMsg += "ERROR: Invalid independent variable " + cols[k] + " \\n";
                    success = false;
                    break;
                  }
                }
              } break;
              // dv, grab the column index (not name) into the dv list
              case 2: if (true) {
                cols = pair[1].split("\\+", -1);
                for (int k = 0; k < cols.length; k++) {
                  cols[k] = Uris.decode(cols[k]).toString();
                  if (myDv.contains(cols[k])) {
                    errorMsg += "ERROR: Duplicated dependent variable " + cols[k] + " \\n";
                    success = false;
                    break;
                  }
                  if (validdvs.contains(cols[k])) {
                    myDv.add(colnames.indexOf(cols[k]));
                  } else {
                    errorMsg += "ERROR: Invalid dependent variable " + cols[k] + " \\n";
                    success = false;
                    break;
                  }
                }
              } break;
              // loanlink, set it to be 0 or 1
              case 3: if (true) {
                loanlinkbool = pair[1].equals("1");
              } break;
              // countonlymean, set it to be 0 or 1
              case 4: if (true) {
                if (pair[1].equals("1")) {
                  countonlymeanint = 1;
                } else if (pair[1].equals("2")) {
                  countonlymeanint = 2;
                }
              } break;
              default: if (true) {
              } break;
            }
            if (success == false)
              break;
          }
          analysis = myAnalysis;
          iv = Collections.unmodifiableList(myIv);
          dv = Collections.unmodifiableList(myDv);
        }
        if (success == true) {
          if (!analysis.equals("freq") && !analysis.equals("mean")) {
            errorMsg += "ERROR: Invalid analysis " + analysis + " \\n";
            success = false;
          }
          if (analysis.equals("freq") && !dv.isEmpty()) {
            errorMsg += "ERROR: Cannot have dv for frequency analysis \\n";
            success = false;
          }
          if (analysis.equals("mean") && dv.isEmpty()) {
            errorMsg += "ERROR: Need dv for mean analysis \\n";
            success = false;
          }
        }
        final Set<Integer> remoteParties;
        {
          final Set<Integer> myPeers = new HashSet<Integer>();
          if (analysis.equals("freq")) {
            if (iv.isEmpty()) {
              // Talk to only the income party.
              myPeers.add(1);
            } else {
              // Talk to everyone except the income party.
              for (int i = 2; i != this.globals.partyCount; ++i) {
                myPeers.add(i);
              }
            }
          } else {
            if (iv.isEmpty()) {
              // Talk to only the income party.
              myPeers.add(1);
            } else {
              // Talk to everyone.
              for (int i = 1; i != this.globals.partyCount; ++i) {
                myPeers.add(i);
              }
            }
          }
          remoteParties = Collections.unmodifiableSet(myPeers);
        }
        if (success) {
          final List<FrontServerIv> queryIvList =
            new ArrayList<FrontServerIv>()
          ;
          for (final int i : iv) {
            for (int j = 0; j != this.globals.ivList.size(); ++j) {
              final FrontServerIv x = this.globals.ivList.get(j);
              if (x.name.equals(colnames.get(i))) {
                queryIvList.add(x);
              }
            }
          }
          final List<FrontServerDv> queryDvList =
            new ArrayList<FrontServerDv>()
          ;
          for (final int i : dv) {
            for (int j = 0; j != this.globals.dvList.size(); ++j) {
              final FrontServerDv x = this.globals.dvList.get(j);
              if (x.name.equals(colnames.get(i))) {
                queryDvList.add(x);
              }
            }
          }
          final FrontServerHttpQueryResponse response =
            new FrontServerHttpQueryResponse(
              new DefaultFullHttpResponse(
                this.request.protocolVersion(),
                HttpResponseStatus.OK,
                ctx.alloc().buffer(),
                true
              ),
              remoteParties.size(),
              analysis,
              queryIvList,
              queryDvList,
              countonlymeanint,
              analysis.equals("freq") || queryIvList.size() == 0
            )
          ;
          HttpUtil.setKeepAlive(
            response.response,
            this.keepAlive
          );
          this.responses.add(response);
          final Random random = new SecureRandom();
          final byte[] queryId = new byte[16];
          random.nextBytes(queryId);
          final ResidueOtTable[] sOts;
          final BlobOtTable[] uOts;
          final ResidueOtTable[] wOts;
          if (analysis.equals("freq") || queryIvList.size() == 0) {
            sOts = null;
            uOts = null;
            wOts = null;
          } else {
            final int nGroups = this.globals.partyCount - 2;
            final int rn = response.sums.size();
            final int cn = response.sums.get(0).size();
            {
              sOts = new ResidueOtTable[nGroups];
              uOts = new BlobOtTable[nGroups];
              wOts = new ResidueOtTable[nGroups];
              for (int p = 0; p != nGroups; ++p) {
                sOts[p] =
                  new ResidueOtTable(
                    rn,
                    1,
                    BlobOtCell.DEALER,
                    4 * this.globals.subtableSize.longValue() * this.globals.rightBits,
                    BigInteger.valueOf(32),
                    this.globals.useFilesForLargeMemory,
                    this.globals.tmpdir
                  )
                ;
                uOts[p] =
                  new BlobOtTable(
                    rn,
                    1,
                    BlobOtCell.DEALER,
                    4 * this.globals.subtableSize.longValue() * 5,
                    1,
                    this.globals.useFilesForLargeMemory,
                    this.globals.tmpdir
                  )
                ;
                wOts[p] =
                  new ResidueOtTable(
                    rn,
                    cn,
                    BlobOtCell.DEALER,
                    4 * this.globals.subtableSize.longValue(),
                    this.globals.modulus,
                    this.globals.useFilesForLargeMemory,
                    this.globals.tmpdir
                  )
                ;
                sOts[p].randomize();
                uOts[p].randomize();
                wOts[p].randomize();
              }
            }
          }
          for (final Integer remoteParty : remoteParties) {
            final String host = this.globals.config.get(
              "raw_connect_host_" + remoteParty
            );
            final String port = this.globals.config.get(
              "raw_connect_port_" + remoteParty
            );
            this.logConnection(
              "connecting to " + host + ":" + port + "..."
            );
            final PoolEntry<ChannelFuture> channel =
              this.globals.rawChannels[remoteParty].acquire()
            ;
            final FrontServerQueryStart queryStart =
              new FrontServerQueryStart(
                analysis,
                queryIvList,
                queryDvList,
                queryId,
                loanlinkbool,
                countonlymeanint,
                remoteParty,
                new FrontServerQueryDone(
                  response.sums.size(),
                  response.sums.get(0).size(),
                  this.responses.size() - 1
                ),
                channel,
                ctx.pipeline(),
                this.responses.size() - 1,
                (remoteParty == 1) ? sOts : null,
                (
                  (remoteParty == 1) ?
                    null
                  : (sOts == null) ?
                    null
                  :
                    sOts[remoteParty - 2]
                ),
                (remoteParty == 1) ? uOts : null,
                (
                  (remoteParty == 1) ?
                    null
                  : (uOts == null) ?
                    null
                  :
                    uOts[remoteParty - 2]
                ),
                (remoteParty == 1) ? wOts : null,
                (
                  (remoteParty == 1) ?
                    null
                  : (wOts == null) ?
                    null
                  :
                    wOts[remoteParty - 2]
                )
              )
            ;
            channel.object.addListener(
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
                  FrontServerHttpHandler.this.logConnection(
                    "connected to " + host + ":" + port
                  );
                  final ChannelPipeline pipeline =
                    channel.object.channel().pipeline();
                  ;
                  pipeline.fireUserEventTriggered(queryStart);
                }
              }
            );
          }
          final long tsFrontServerEnd = System.currentTimeMillis();
          globals.log(LocalTime.now()+Constants.DELIMETER+Constants.LATENCY+Constants.DELIMETER+this.getClass().getSimpleName()+Constants.DELIMETER+"tsFrontServerEnd="+tsFrontServerEnd);
          globals.log(LocalTime.now()+Constants.DELIMETER+Constants.LATENCY+Constants.DELIMETER
        		  +"FRONTSERVER LATENCY = "+(tsFrontServerEnd-tsFrontServerStart));
          return;
        }
        // build response
        StringBuilder response = this.responseBuilder;
        {
          response.append("{\r\n  \"success\":false,\r\n  \"error\":\"").append(errorMsg).append("\"\r\n}");
        }
        globals.log(response);
      } catch (Exception e) {
        System.out.println(
            e.toString() + "\nDetails: " + e.getMessage() + "\nFrom line: " + e.getStackTrace()[0].getLineNumber());
        return;
      }
    }
    fullResponse.content().writeBytes(
      this.responseEncoder.encode(
        CharBuffer.wrap(this.responseBuilder)
      )
    );
    HttpUtil.setKeepAlive(
      fullResponse,
      this.keepAlive
    );
    fullResponse.headers().set(
      HttpHeaderNames.ACCESS_CONTROL_ALLOW_ORIGIN,
      "*"
    );
    fullResponse.headers().set(
      HttpHeaderNames.CONTENT_LENGTH,
      fullResponse.content().readableBytes()
    );
    final FrontServerHttpResponse response =
      new FrontServerHttpResponse(
        fullResponse
      )
    ;
    response.done = true;
    this.responses.add(response);
    this.sendResponses(ctx);
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

private final void handleQuery(
  final ChannelHandlerContext ctx,
  final HttpObject object
) throws
  Exception
{
  if (object instanceof HttpRequest) {
    this.handleQuery(ctx);
  } else {
    this.handleQuery(ctx, (HttpContent)object);
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
 * @since SAFRN&nbsp;0.1.0 (2017-11-08)
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

private final void sendResponses(
  final ChannelHandlerContext ctx
) {
  while (this.responsesSent != this.responses.size()) {
    final FrontServerHttpResponse response =
      this.responses.get(this.responsesSent)
    ;
    if (!response.done) {
      return;
    }
    this.responses.set(this.responsesSent, null);
    ctx.writeAndFlush(response.response);
    ++this.responsesSent;
  }
  if (this.keepAlive) {
    if (this.doneReading) {
      this.close(ctx);
    }
  } else {
    this.close(ctx);
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

private final void userEventTriggered(
  final ChannelHandlerContext ctx,
  final FrontServerQueryDone evt
) throws
  Exception
{
  final FrontServerHttpQueryResponse response =
    (FrontServerHttpQueryResponse)this.responses.get(evt.index)
  ;
  for (int i = 0; i != response.sums.size(); ++i) {
    final List<BigInteger> row = response.sums.get(i);
    final List<BigInteger> summands = evt.summands.get(i);
    for (int j = 0; j != row.size(); ++j) {
      row.set(j, row.get(j).add(summands.get(j)));
    }
  }
  --response.countdown;
  if (response.countdown == 0) {
    this.responseBuilder.setLength(0);
    this.responseBuilder.append(
      "{\r\n"
    );
    this.responseBuilder.append(
      "  \"success\": \"true\",\r\n"
    );
    this.responseBuilder.append(
      "  \"analysis\": \""
    );
    Json.encode(
      this.responseBuilder,
      response.analysis
    );
    this.responseBuilder.append(
      "\",\r\n"
    );
    if (response.ivList.size() != 0) {
      this.responseBuilder.append(
        "  \"iv\": [\r\n"
      );
      for (int i = 0; i != response.ivList.size(); ++i) {
        this.responseBuilder.append(
          "    \""
        );
        Json.encode(
          this.responseBuilder,
          response.ivList.get(i).name
        );
        this.responseBuilder.append(
          "\""
        );
        if (i != response.ivList.size() - 1) {
          this.responseBuilder.append(
            ","
          );
        }
        this.responseBuilder.append(
          "\r\n"
        );
      }
      this.responseBuilder.append(
        "  ],\r\n"
      );
    }
    if (response.dvList.size() != 0) {
      this.responseBuilder.append(
        "  \"dv\": [\r\n"
      );
      for (int i = 0; i != response.dvList.size(); ++i) {
        this.responseBuilder.append(
          "    \""
        );
        Json.encode(
          this.responseBuilder,
          response.dvList.get(i).name
        );
        this.responseBuilder.append(
          "\""
        );
        if (i != response.dvList.size() - 1) {
          this.responseBuilder.append(
            ","
          );
        }
        this.responseBuilder.append(
          "\r\n"
        );
      }
      this.responseBuilder.append(
        "  ],\r\n"
      );
    }
    this.responseBuilder.append(
      "  \"data\": [\r\n"
    );
    for (int i = 0; i != response.sums.size(); ++i) {
      final List<BigInteger> row = response.sums.get(i);
      for (int j = 0; j != row.size(); ++j) {
        row.set(j, row.get(j).mod(this.globals.modulus));
      }
    }
    if (response.ivList.size() == 0) {
      final List<BigInteger> row = response.sums.get(0);
      final BigInteger count = row.get(row.size() - 1);
      this.responseBuilder.append(
        "    "
      );
      if (response.analysis.equals("freq")) {
        this.responseBuilder.append(
          count.toString()
        );
      } else {
        for (int i = 0; i != response.dvList.size(); ++i) {
          if (count.doubleValue() == 0) {
            this.responseBuilder.append(
              "null"
            );
          } else {
            this.responseBuilder.append(
              row.get(i).doubleValue() / count.doubleValue()
            );
          }
          if (i != response.dvList.size() - 1) {
            this.responseBuilder.append(
              ", "
            );
          }
        }
      }
      this.responseBuilder.append(
        "\r\n"
      );
    } else {
      int[] domainIndexes = new int[response.ivList.size()];
      for (int i = 0; i != response.sums.size(); ++i) {
        final List<BigInteger> row = response.sums.get(i);
        this.responseBuilder.append(
          "    ["
        );
        for (int j = 0; j != response.ivList.size(); ++j) {
          final FrontServerIv iv = response.ivList.get(j);
          if (domainIndexes[j] == iv.domain.size()) {
            this.responseBuilder.append(
              "\"*\", "
            );
          } else if (iv.quote) {
            this.responseBuilder.append(
              "\""
            );
            Json.encode(
              this.responseBuilder,
              iv.domain.get(domainIndexes[j])
            );
            this.responseBuilder.append(
              "\", "
            );
          } else {
            this.responseBuilder.append(
              iv.domain.get(domainIndexes[j])
            );
            this.responseBuilder.append(
              ", "
            );
          }
        }
        if (response.analysis.equals("freq")) {
          final BigInteger count = row.get(row.size() - 1);
          this.responseBuilder.append(
            count.toString()
          );
        } else {
          final BigInteger count =
            row.get(0).mod(BigInteger.ONE.shiftLeft(32))
          ;
          for (int j = 0; j != response.dvList.size(); ++j) {
            row.set(j, row.get(j).shiftRight(32));
            if (count.doubleValue() == 0) {
              if (response.countonlymeanint == 1) {
                this.responseBuilder.append(
                  "0"
                );
              } else if (response.countonlymeanint == 0) {
                this.responseBuilder.append(
                  "null"
                );
              } else if (response.countonlymeanint == 2) {
                this.responseBuilder.append(
                  "[ null, 0 ]"
                );
              }
            } else {
              if (response.countonlymeanint == 1) {
                this.responseBuilder.append(
                  count.doubleValue()
                );
              } else if (response.countonlymeanint == 0) {
                this.responseBuilder.append(
                  row.get(j).doubleValue() / count.doubleValue()
                );
              } else if (response.countonlymeanint == 2) {
                this.responseBuilder.append(
                  "["
                );
                this.responseBuilder.append(
                  row.get(j).doubleValue() / count.doubleValue()
                );
                this.responseBuilder.append(
                  " , "
                );
                this.responseBuilder.append(
                  count.doubleValue()
                );
                this.responseBuilder.append(
                  "]"
                );
              }
            }
            if (j != response.dvList.size() - 1) {
              this.responseBuilder.append(
                ", "
              );
            }
          }
        }
        this.responseBuilder.append(
          "]"
        );
        if (i != response.sums.size() - 1) {
          this.responseBuilder.append(
            ","
          );
        }
        this.responseBuilder.append(
          "\r\n"
        );
        for (int j = response.ivList.size() - 1; j >= 0; --j) {
          final FrontServerIv iv = response.ivList.get(j);
          if (domainIndexes[j] < iv.domain.size()) {
            ++domainIndexes[j];
            break;
          }
          domainIndexes[j] = 0;
        }
      }
    }
    this.responseBuilder.append(
      "  ]\r\n"
    );
    this.responseBuilder.append(
      "}\r\n"
    );
    response.response.content().writeBytes(
      this.responseEncoder.encode(
        CharBuffer.wrap(this.responseBuilder)
      )
    );
    response.response.headers().set(
      HttpHeaderNames.ACCESS_CONTROL_ALLOW_ORIGIN,
      "*"
    );
    response.response.headers().set(
      HttpHeaderNames.CONTENT_LENGTH,
      response.response.content().readableBytes()
    );
    response.response.headers().set(
      HttpHeaderNames.CONTENT_TYPE,
      "application/json; charset=UTF-8"
    );
    //tsFrontServerStart
    this.tsAPIEnd = System.currentTimeMillis();
    globals.log(LocalTime.now()+Constants.DELIMETER+Constants.LATENCY+Constants.DELIMETER+this.getClass().getSimpleName()+Constants.DELIMETER+"tsAPIEnd = "+tsAPIEnd);
    globals.log(LocalTime.now()+Constants.DELIMETER+Constants.LATENCY+Constants.DELIMETER+this.getClass().getSimpleName()+Constants.DELIMETER+"API Latency = "+(tsAPIEnd-this.tsAPIStart));
    globals.log(LocalTime.now()+Constants.DELIMETER+Constants.LATENCY+Constants.DELIMETER+Constants.DELIMETER+"END TO END API LATENCY = "+(tsAPIEnd-this.tsAPIStart));
    globals.log("FYI:\n returing string = "+this.responseBuilder);
    response.done = true;
    this.sendResponses(ctx);
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
public final void userEventTriggered(
  final ChannelHandlerContext ctx,
  final Object evt
) throws
  Exception
{
  if (evt instanceof FrontServerQueryDone) {
    this.userEventTriggered(
      ctx,
      (FrontServerQueryDone)evt
    );
    return;
  }
}

//----------------------------------------------------------------------

  private static final String opt[] = new String[] {
    "analysis",
    "iv",
    "dv",
    "loanlink",
    "countonlymean"
  }; //list of options

}
