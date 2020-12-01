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
 * <code>com.stealthsoftwareinc.commercial.safrn.FrontServer</code>
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
import com.stealthsoftwareinc.commercial.safrn.FrontServerGlobals;
import com.stealthsoftwareinc.sst.Args;
import com.stealthsoftwareinc.sst.InvalidOptionNameStatus;
import com.stealthsoftwareinc.sst.InvalidOptionValueStatus;
import com.stealthsoftwareinc.sst.Json;
import com.stealthsoftwareinc.sst.MissingOptionValueStatus;
import com.stealthsoftwareinc.sst.NullInputStream;
import com.stealthsoftwareinc.sst.NullOutputStream;
import com.stealthsoftwareinc.sst.OptionPair;
import io.netty.bootstrap.Bootstrap;
import io.netty.bootstrap.ServerBootstrap;
import io.netty.channel.Channel;
import io.netty.channel.ChannelOption;
import io.netty.channel.nio.NioEventLoopGroup;
import io.netty.channel.socket.nio.NioServerSocketChannel;
import io.netty.channel.socket.nio.NioSocketChannel;
import io.netty.util.NetUtil;
import java.io.*;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.PrintStream;
import java.lang.Exception;
import java.lang.Integer;
import java.lang.String;
import java.lang.System;
import java.net.InetSocketAddress;
import java.net.URI;
import java.net.URLDecoder;
import java.nio.*;
import java.nio.charset.*;
import java.nio.file.*;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Scanner;
import java.util.concurrent.*;
import java.util.concurrent.Callable;

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

public final class FrontServer
implements
  Callable<Integer>
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

private final String[] args;

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

private final PrintStream stderr;

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

private final InputStream stdin;

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

private final PrintStream stdout;

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

public FrontServer(
  final InputStream stdin,
  final PrintStream stdout,
  final PrintStream stderr,
  final String... args
) {
  this.stdin = stdin;
  this.stdout = stdout;
  this.stderr = stderr;
  this.args = args;
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
public final Integer call(
) throws
  Exception
{
  return
    FrontServer.main(
      this.stdin,
      this.stdout,
      this.stderr,
      this.args
    )
  ;
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

public static final int main(
  final InputStream stdin,
  final PrintStream stdout,
  final PrintStream stderr,
  final String... args
) throws
  Exception
{
  if (stdin == null) {
    final int s;
    final NullInputStream stdin2 = new NullInputStream();
    try {
      s =
        FrontServer.main(
          stdin2,
          stdout,
          stderr,
          args
        )
      ;
    } finally {
      try {
        stdin2.close();
      } catch (final Exception e) {
      }
    }
    return s;
  }
  if (stdout == null) {
    final int s;
    final NullOutputStream stdout2 = new NullOutputStream();
    try {
      final PrintStream stdout3 = new PrintStream(stdout2);
      try {
        s =
          FrontServer.main(
            stdin,
            stdout3,
            stderr,
            args
          )
        ;
        stdout3.close();
      } catch (final Exception e1) {
        try {
          stdout3.close();
        } catch (final Exception e2) {
        }
        throw e1;
      }
      stdout2.close();
    } catch (final Exception e1) {
      try {
        stdout2.close();
      } catch (final Exception e2) {
      }
      throw e1;
    }
    return s;
  }
  if (stderr == null) {
    final int s;
    final NullOutputStream stderr2 = new NullOutputStream();
    try {
      final PrintStream stderr3 = new PrintStream(stderr2);
      try {
        s =
          FrontServer.main(
            stdin,
            stdout,
            stderr3,
            args
          )
        ;
        stderr3.close();
      } catch (final Exception e1) {
        try {
          stderr3.close();
        } catch (final Exception e2) {
        }
        throw e1;
      }
      stderr2.close();
    } catch (final Exception e1) {
      try {
        stderr2.close();
      } catch (final Exception e2) {
      }
      throw e1;
    }
    return s;
  }
  if (Args.needNull(args)) {
    final int s;
    final String[] args2 = Args.addNull(args);
    s =
      FrontServer.main(
        stdin,
        stdout,
        stderr,
        args2
      )
    ;
    return s;
  }
  final Config config = new Config();
  {
    int argi = 0;
    while (Args.isOption(args[argi])) {
      final String fullName = Args.cropOption(args[argi]);
      final String snippedName = Args.snipOption(fullName);
      if (Args.isOption(args[argi], new String[] {
        "--config"
      })) {
        final OptionPair opp = Args.requireOptionValue(args, argi);
        if (opp.value == null) {
          stderr.println(
            "safrn_front_server!" +
            " " +
            fullName +
            " requires a value"
          );
          throw (MissingOptionValueStatus)
            new MissingOptionValueStatus(
              fullName
            ).initCause(null)
          ;
        }
        config.addProperties(opp.value);
        argi = opp.argi;
        continue;
      }
      if (config.isValid(snippedName)) {
        final OptionPair opp = Args.requireOptionValue(args, argi);
        if (opp.value == null) {
          stderr.println(
            "safrn_front_server!" +
            " " +
            fullName +
            " requires a value"
          );
          throw (MissingOptionValueStatus)
            new MissingOptionValueStatus(
              fullName
            ).initCause(null)
          ;
        }
        if (!config.isValid(snippedName, opp.value)) {
          stderr.println(
            "safrn_front_server!" +
            " invalid " +
            fullName +
            " value: \"" +
            Json.encode(opp.value) +
            "\""
          );
          throw (InvalidOptionValueStatus)
            new InvalidOptionValueStatus(
              fullName +
              "=" +
              opp.value
            ).initCause(null)
          ;
        }
        config.add(snippedName, opp.value);
        argi = opp.argi;
        continue;
      }
      stderr.println(
        "safrn_front_server!" +
        " invalid option name: \"" +
        Json.encode(fullName) +
        "\""
      );
      throw (InvalidOptionNameStatus)
        new InvalidOptionNameStatus(
          fullName
        ).initCause(null)
      ;
    }
  }
  config.makeImmutable();
  final NioEventLoopGroup eventLoopGroup = new NioEventLoopGroup();
  try {
    final Bootstrap rawConnector = new Bootstrap();
    final FrontServerGlobals globals =
      new FrontServerGlobals(
        config,
        stdout,
        rawConnector
      )
    ;
    rawConnector.group(eventLoopGroup);
    rawConnector.channel(NioSocketChannel.class);
    rawConnector.handler(
      new FrontServerRawInitializer(
        globals
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
    final ServerBootstrap httpServer = new ServerBootstrap();
    httpServer.group(eventLoopGroup);
    httpServer.channel(NioServerSocketChannel.class);
    httpServer.childHandler(
      new FrontServerHttpInitializer(
        globals
      )
    );
    httpServer.option(
      ChannelOption.SO_BACKLOG,
      Math.min(NetUtil.SOMAXCONN, 128)
    );
    httpServer.option(
      ChannelOption.SO_REUSEADDR,
      true
    );
    httpServer.childOption(
      ChannelOption.SO_KEEPALIVE,
      true
    );
    httpServer.childOption(
      ChannelOption.TCP_NODELAY,
      true
    );
    final Channel httpListener =
      httpServer.bind(
        config.get("http_listen_host"),
        Integer.parseInt(config.get("http_listen_port"))
      ).syncUninterruptibly().channel()
    ;
    try {
      stdout.println("Server started on " + config.get("http_listen_host") + ":" + config.get("http_listen_port"));
      stdout.println("Press <ENTER> to stop the server");
      stdin.read();
      stdout.println("exiting...");
    } finally {
      try {
        httpListener.close().syncUninterruptibly();
      } catch (final Exception e) {
      }
    }
  } finally {
    try {
      eventLoopGroup.shutdownGracefully().syncUninterruptibly();
    } catch (final Exception e) {
    }
  }
  return 0;
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

public static final void main(
  final String... args
) throws
  Exception
{
  final InputStream stdin = System.in;
  final PrintStream stdout = System.out;
  final PrintStream stderr = System.err;
  System.exit(FrontServer.main(stdin, stdout, stderr, args));
}

}
