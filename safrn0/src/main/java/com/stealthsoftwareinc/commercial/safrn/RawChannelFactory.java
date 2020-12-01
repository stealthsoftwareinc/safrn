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
 * <code>com.stealthsoftwareinc.commercial.safrn.RawChannelFactory</code>
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

import com.stealthsoftwareinc.sst.Factory;
import com.stealthsoftwareinc.sst.InvalidArgumentStatus;
import com.stealthsoftwareinc.sst.NullPointerStatus;
import io.netty.bootstrap.Bootstrap;
import io.netty.channel.ChannelFuture;
import java.lang.String;

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

public class RawChannelFactory
implements
  Factory<ChannelFuture>
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

private final Bootstrap connector;

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

private final String host;

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

private final int port;

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

public RawChannelFactory(
  final Bootstrap connector,
  final String host,
  final int port
) {
  if (connector == null) {
    throw (NullPointerStatus)
      new NullPointerStatus(
        "connector is a null reference"
      ).initCause(null)
    ;
  }
  if (host == null) {
    throw (NullPointerStatus)
      new NullPointerStatus(
        "host is a null reference"
      ).initCause(null)
    ;
  }
  if (port < 1 || port > 65535) {
    throw (InvalidArgumentStatus)
      new InvalidArgumentStatus(
      ).initCause(null)
    ;
  }
  this.connector = connector;
  this.host = host;
  this.port = port;
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
public ChannelFuture get(
) {
  return this.connector.connect(this.host, this.port);
}

}
