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
 * <code>com.stealthsoftwareinc.commercial.safrn.SqlChannelFactory</code>
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
import com.stealthsoftwareinc.sst.NullPointerStatus;
import io.netty.util.concurrent.EventExecutor;
import io.netty.util.concurrent.Future;
import io.netty.util.concurrent.Promise;
import java.lang.Runnable;
import java.sql.Connection;
import java.sql.DriverManager;

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

public class SqlChannelFactory
implements
  Factory<Future<Connection>>
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

private final EventExecutor executor;

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

private final String jdbcUrl;

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

public SqlChannelFactory(
  final String jdbcUrl,
  final EventExecutor executor
) {
  if (jdbcUrl == null) {
    throw (NullPointerStatus)
      new NullPointerStatus(
        "jdbcUrl is a null reference"
      ).initCause(null)
    ;
  }
  if (executor == null) {
    throw (NullPointerStatus)
      new NullPointerStatus(
        "executor is a null reference"
      ).initCause(null)
    ;
  }
  this.jdbcUrl = jdbcUrl;
  this.executor = executor;
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
public Future<Connection> get(
) {
  final Promise<Connection> promise = this.executor.newPromise();
  this.executor.execute(
    new Runnable() {
      @Override
      public final void run(
      ) {
        try {
          promise.setSuccess(
            DriverManager.getConnection(
              SqlChannelFactory.this.jdbcUrl
            )
          );
        } catch (final Exception e) {
          promise.setFailure(e);
        }
      }
    }
  );
  return promise;
}

}
