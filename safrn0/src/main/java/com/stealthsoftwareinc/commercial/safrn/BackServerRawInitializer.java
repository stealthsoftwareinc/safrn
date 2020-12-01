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
 * <code>com.stealthsoftwareinc.commercial.safrn.BackServerRawInitializer</code>
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
import com.stealthsoftwareinc.commercial.safrn.BackServerRawHandler;
import com.stealthsoftwareinc.sst.NullPointerStatus;
import io.netty.channel.ChannelInitializer;
import io.netty.channel.socket.SocketChannel;

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

public final class BackServerRawInitializer
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

private final int remoteParty;

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

public BackServerRawInitializer(
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
  this.globals = globals;
  this.remoteParty = remoteParty;
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
    "BackServerRawHandler",
    new BackServerRawHandler(
      this.globals,
      this.remoteParty
    )
  );
}

}