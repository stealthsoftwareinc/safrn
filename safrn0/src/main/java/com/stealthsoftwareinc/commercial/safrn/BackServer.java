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
 * <code>com.stealthsoftwareinc.commercial.safrn.BackServer</code>
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
import com.stealthsoftwareinc.commercial.safrn.Config;
import com.stealthsoftwareinc.sst.Args;
import com.stealthsoftwareinc.sst.InvalidOptionNameStatus;
import com.stealthsoftwareinc.sst.InvalidOptionValueStatus;
import com.stealthsoftwareinc.sst.Json;
import com.stealthsoftwareinc.sst.MissingCaseStatus;
import com.stealthsoftwareinc.sst.MissingOptionValueStatus;
import com.stealthsoftwareinc.sst.NullInputStream;
import com.stealthsoftwareinc.sst.NullOutputStream;
import com.stealthsoftwareinc.sst.OptionPair;
import com.stealthsoftwareinc.sst.Uris;
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

public final class BackServer
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

public BackServer(
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
    BackServer.main(
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
        BackServer.main(
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
          BackServer.main(
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
          BackServer.main(
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
      BackServer.main(
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
            "safrn_back_server!" +
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
            "safrn_back_server!" +
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
            "safrn_back_server!" +
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
        "safrn_back_server!" +
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
    final String jdbcUrl;
    switch (config.get("jdbc_subprotocol")) {
      case "mysql": if (true) {
        final String host =
          (config.isExplicit("mysql_host")) ?
            config.get("mysql_host")
          :
            config.get("jdbc_host")
        ;
        final String port =
          (config.isExplicit("mysql_port")) ?
            config.get("mysql_port")
          : (config.isExplicit("jdbc_port")) ?
            config.get("jdbc_port")
          :
            String.valueOf(ConfigBase.DEFAULT_MYSQL_PORT)
        ;
        final String database =
          (config.isExplicit("mysql_database")) ?
            config.get("mysql_database")
          :
            config.get("jdbc_database")
        ;
        final String username =
          (config.isExplicit("mysql_username")) ?
            config.get("mysql_username")
          :
            config.get("jdbc_username")
        ;
        final String password =
          (config.isExplicit("mysql_password")) ?
            config.get("mysql_password")
          :
            config.get("jdbc_password")
        ;
        jdbcUrl =
          "jdbc:mysql:" +
          "//" + host +
          ":" + port +
          "/" + Uris.encode(database).toString() +
          "?user=" + Uris.encode(username).toString() +
          "&password=" + Uris.encode(password).toString() +
          "&serverTimezone=UTC"
        ;
      } break;
      case "postgresql": if (true) {
        final String host =
          (config.isExplicit("postgresql_host")) ?
            config.get("postgresql_host")
          :
            config.get("jdbc_host")
        ;
        final String port =
          (config.isExplicit("postgresql_port")) ?
            config.get("postgresql_port")
          : (config.isExplicit("jdbc_port")) ?
            config.get("jdbc_port")
          :
            String.valueOf(ConfigBase.DEFAULT_POSTGRESQL_PORT)
        ;
        final String database =
          (config.isExplicit("postgresql_database")) ?
            config.get("postgresql_database")
          :
            config.get("jdbc_database")
        ;
        final String username =
          (config.isExplicit("postgresql_username")) ?
            config.get("postgresql_username")
          :
            config.get("jdbc_username")
        ;
        final String password =
          (config.isExplicit("postgresql_password")) ?
            config.get("postgresql_password")
          :
            config.get("jdbc_password")
        ;
        jdbcUrl =
          "jdbc:postgresql:" +
          "//" + host +
          ":" + port +
          "/" + Uris.encode(database).toString() +
          "?user=" + Uris.encode(username).toString() +
          "&password=" + Uris.encode(password).toString() +
          "&ssl=true"
        ;
      } break;
      case "oracle": if (true) {
        final String host =
          (config.isExplicit("oracle_host")) ?
            config.get("oracle_host")
          :
            config.get("jdbc_host")
        ;
        final String port =
          (config.isExplicit("oracle_port")) ?
            config.get("oracle_port")
          : (config.isExplicit("jdbc_port")) ?
            config.get("jdbc_port")
          :
            String.valueOf(ConfigBase.DEFAULT_ORACLE_PORT)
        ;
        final String serviceName = config.get("oracle_servicename");
        final String username =
          (config.isExplicit("oracle_username")) ?
            config.get("oracle_username")
          :
            config.get("jdbc_username")
        ;
        final String password =
          (config.isExplicit("oracle_password")) ?
            config.get("oracle_password")
          :
            config.get("jdbc_password")
        ;
        jdbcUrl =
            "jdbc:oracle:thin" +
            ":" + username +
            "/" + password +
            "@//" + host +
            ":" + port +
            "/" + serviceName
        ;
      } break;
      default: if (true) {
        throw (MissingCaseStatus)
          new MissingCaseStatus(
          ).initCause(null)
        ;
      } break;
    }
    final BackServerGlobals globals =
      new BackServerGlobals(
        config,
        stdout,
        eventLoopGroup,
        jdbcUrl,
        eventLoopGroup.next()
      )
    ;
    final ServerBootstrap rawServer = new ServerBootstrap();
    rawServer.group(eventLoopGroup);
    rawServer.channel(NioServerSocketChannel.class);
    rawServer.childHandler(
      new BackServerRawInitializer(
        globals,
        -1
      )
    );
    rawServer.option(
      ChannelOption.SO_BACKLOG,
      Math.min(NetUtil.SOMAXCONN, 128)
    );
    rawServer.option(
      ChannelOption.SO_REUSEADDR,
      true
    );
    rawServer.childOption(
      ChannelOption.SO_KEEPALIVE,
      true
    );
    rawServer.childOption(
      ChannelOption.TCP_NODELAY,
      true
    );
    final Channel rawListener =
      rawServer.bind(
        config.get("raw_listen_host"),
        Integer.parseInt(config.get("raw_listen_port"))
      ).syncUninterruptibly().channel()
    ;
    try {
      stdout.println("Server started on " + config.get("raw_listen_host") + ":" + config.get("raw_listen_port"));
      stdout.println("Press <ENTER> to stop the server");
      stdin.read();
      stdout.println("exiting...");
    } finally {
      try {
        rawListener.close().syncUninterruptibly();
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
  System.exit(BackServer.main(stdin, stdout, stderr, args));
}

}
