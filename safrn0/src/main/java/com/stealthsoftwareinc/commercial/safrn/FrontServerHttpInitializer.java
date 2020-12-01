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
 * <code>com.stealthsoftwareinc.commercial.safrn.FrontServerHttpInitializer</code>
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

import com.stealthsoftwareinc.commercial.safrn.FrontServerGlobals;
import com.stealthsoftwareinc.commercial.safrn.FrontServerHttpHandler;
import com.stealthsoftwareinc.sst.NullPointerStatus;
import io.netty.channel.ChannelInitializer;
import io.netty.channel.socket.SocketChannel;
import io.netty.handler.codec.http.HttpServerCodec;
import java.lang.Integer;

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

public final class FrontServerHttpInitializer
extends ChannelInitializer<SocketChannel>
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

public FrontServerHttpInitializer(
  final FrontServerGlobals globals
) {
  if (globals == null) {
    throw (NullPointerStatus)
      new NullPointerStatus(
        "globals is a null reference"
      ).initCause(null)
    ;
  }
  this.globals = globals;
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
protected final void initChannel(
  final SocketChannel channel
) {
  channel.pipeline().addLast(
    "HttpServerCodec",
    new HttpServerCodec(
    )
  );
  channel.pipeline().addLast(
    "FrontServerHttpHandler",
    new FrontServerHttpHandler(
      this.globals
    )
  );
}

}
