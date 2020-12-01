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
 * <code>com.stealthsoftwareinc.commercial.safrn.ConfigBase</code>
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

import com.stealthsoftwareinc.sst.ConfigMapper;
import com.stealthsoftwareinc.sst.ConfigPartition;
import com.stealthsoftwareinc.sst.ConfigPattern;
import com.stealthsoftwareinc.sst.ConfigPredicate;
import com.stealthsoftwareinc.sst.HostNamePredicate;
import com.stealthsoftwareinc.sst.JavaFloatValuePredicate;
import com.stealthsoftwareinc.sst.PortNumberPredicate;
import java.lang.Boolean;
import java.lang.Double;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;

/* end_imports */

/**
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

public abstract class ConfigBase<T extends ConfigBase<T>>
extends com.stealthsoftwareinc.sst.ConfigBase<T>
{

/**
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

protected static final Collection<ConfigMapper> ALIASES;

/**
 *
 * @inheritancenotes
 * You should not hide this variable.
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

public static final BigInteger DEFAULT_GROUP_SIZE_BOUND;

/**
 *
 * @inheritancenotes
 * You should not hide this variable.
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

public static final double DEFAULT_HASH_SLOP;

/**
 * The default host name for all network protocols.
 *
 * @inheritancenotes
 * You should not hide this variable.
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

public static final String DEFAULT_HOST;

/**
 * The default port number for the HTTP network protocol.
 *
 * @inheritancenotes
 * You should not hide this variable.
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

public static final int DEFAULT_HTTP_PORT;

/**
 *
 * @inheritancenotes
 * You should not hide this variable.
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

public static final BigInteger DEFAULT_INCOME_SIZE_BOUND;

/**
 *
 * @inheritancenotes
 * You should not hide this variable.
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

public static final String DEFAULT_JDBC_DATABASE;

/**
 *
 * @inheritancenotes
 * You should not hide this variable.
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

public static final String DEFAULT_JDBC_PASSWORD;

/**
 *
 * @inheritancenotes
 * You should not hide this variable.
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

public static final String DEFAULT_JDBC_SUBPROTOCOL;

/**
 *
 * @inheritancenotes
 * You should not hide this variable.
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

public static final String DEFAULT_JDBC_USERNAME;

/**
 *
 */

public static final String DEFAULT_LOG_CONNECTIONS;

/**
 *
 * @inheritancenotes
 * You should not hide this variable.
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

public static final BigInteger DEFAULT_MODULUS;

/**
 *
 * @inheritancenotes
 * You should not hide this variable.
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

public static final int DEFAULT_MYSQL_PORT;

/**
 *
 * @inheritancenotes
 * You should not hide this variable.
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

public static final int DEFAULT_ORACLE_PORT;

/**
 *
 * @inheritancenotes
 * You should not hide this variable.
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

public static final String DEFAULT_ORACLE_SERVICENAME;

/**
 *
 * @inheritancenotes
 * You should not hide this variable.
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

public static final int DEFAULT_PARTY_COUNT;

/**
 *
 * @inheritancenotes
 * You should not hide this variable.
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

public static final String DEFAULT_PERFORMANCE_LOG_FILE;

/**
 *
 * @inheritancenotes
 * You should not hide this variable.
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

public static final int DEFAULT_POSTGRESQL_PORT;

/**
 * The default port number for the raw network protocol.
 *
 * @inheritancenotes
 * You should not hide this variable.
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

public static final int DEFAULT_RAW_PORT;

/**
 *
 * @inheritancenotes
 * You should not hide this variable.
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

public static final String DEFAULT_TMPDIR;

/**
 *
 * @inheritancenotes
 * You should not hide this variable.
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

public static final boolean DEFAULT_USE_FILES_FOR_LARGE_MEMORY;

/**
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

protected static final Collection<ConfigPartition> PARTITIONS;

static {
  {
    final ArrayList<ConfigMapper> aliases;
    aliases = new ArrayList<ConfigMapper>();
    ALIASES = Collections.unmodifiableList(aliases);
  }
  DEFAULT_GROUP_SIZE_BOUND = new BigInteger("1000");
  DEFAULT_HASH_SLOP = 2.1;
  DEFAULT_HOST = "127.0.0.1";
  DEFAULT_HTTP_PORT = 8080;
  DEFAULT_INCOME_SIZE_BOUND = new BigInteger("1000");
  DEFAULT_JDBC_DATABASE = "safrn";
  DEFAULT_JDBC_PASSWORD = "root";
  DEFAULT_JDBC_SUBPROTOCOL = "mysql";
  DEFAULT_JDBC_USERNAME = "root";
  DEFAULT_LOG_CONNECTIONS = "";
  DEFAULT_MODULUS = new BigInteger("1073741789");
  DEFAULT_MYSQL_PORT = 3306;
  DEFAULT_ORACLE_PORT = 1521;
  DEFAULT_ORACLE_SERVICENAME = "ORCLCDB.localdomain";
  DEFAULT_PARTY_COUNT = 5;
  DEFAULT_PERFORMANCE_LOG_FILE = "";
  DEFAULT_POSTGRESQL_PORT = 5432;
  DEFAULT_RAW_PORT = 10701;
  DEFAULT_TMPDIR = "";
  DEFAULT_USE_FILES_FOR_LARGE_MEMORY = false;
  {
    final ArrayList<ConfigPartition> partitions;
    partitions = new ArrayList<ConfigPartition>();
    partitions.add(
      new ConfigPartition(
        new ConfigPattern(
          "group_size_bound"
        ),
        new ConfigPattern(
          "[1-9][0-9]*"
        ),
        ConfigBase.DEFAULT_GROUP_SIZE_BOUND.toString()
      )
    );
    partitions.add(
      new ConfigPartition(
        new ConfigPattern(
          "hash_slop"
        ),
        new JavaFloatValuePredicate(),
        Double.toString(ConfigBase.DEFAULT_HASH_SLOP)
      )
    );
    partitions.add(
      new ConfigPartition(
        new ConfigPattern(
          "http_listen_host"
        ),
        new HostNamePredicate(),
        ConfigBase.DEFAULT_HOST
      )
    );
    partitions.add(
      new ConfigPartition(
        new ConfigPattern(
          "http_listen_port"
        ),
        new PortNumberPredicate(false),
        Integer.toString(ConfigBase.DEFAULT_HTTP_PORT)
      )
    );
    partitions.add(
      new ConfigPartition(
        new ConfigPattern(
          "income_size_bound"
        ),
        new ConfigPattern(
          "[1-9][0-9]*"
        ),
        ConfigBase.DEFAULT_INCOME_SIZE_BOUND.toString()
      )
    );
    partitions.add(
      new ConfigPartition(
        new ConfigPattern(
          "jdbc_database"
        ),
        ConfigBase.DEFAULT_JDBC_DATABASE
      )
    );
    partitions.add(
      new ConfigPartition(
        new ConfigPattern(
          "jdbc_host"
        ),
        new HostNamePredicate(),
        ConfigBase.DEFAULT_HOST
      )
    );
    partitions.add(
      new ConfigPartition(
        new ConfigPattern(
          "jdbc_password"
        ),
        ConfigBase.DEFAULT_JDBC_PASSWORD
      )
    );
    partitions.add(
      new ConfigPartition(
        new ConfigPattern(
          "jdbc_port"
        ),
        new PortNumberPredicate(false),
        "12345"
      )
    );
    partitions.add(
      new ConfigPartition(
        new ConfigPattern(
          "jdbc_subprotocol"
        ),
        new ConfigPredicate() {
          @Override
          public final boolean test(
            final CharSequence input
          ) {
            if (input == null) {
              return this.test("");
            }
            switch (input.toString()) {
              case "mysql": return true;
              case "postgresql": return true;
              case "oracle": return true;
            }
            return false;
          }
        },
        ConfigBase.DEFAULT_JDBC_SUBPROTOCOL
      )
    );
    partitions.add(
      new ConfigPartition(
        new ConfigPattern(
          "jdbc_username"
        ),
        ConfigBase.DEFAULT_JDBC_USERNAME
      )
    );
    partitions.add(
      new ConfigPartition(
        new ConfigPattern(
          "local_party"
        ),
        new ConfigPattern(
          "0|[1-9][0-9]*"
        ),
        Integer.toString(0)
      )
    );
    partitions.add(
      new ConfigPartition(
        new ConfigPattern(
          "log_connections"
        ),
        ConfigBase.DEFAULT_LOG_CONNECTIONS
      )
    );
    partitions.add(
      new ConfigPartition(
        new ConfigPattern(
          "modulus"
        ),
        new ConfigPattern(
          "[1-9][0-9]*"
        ),
        ConfigBase.DEFAULT_MODULUS.toString()
      )
    );
    partitions.add(
      new ConfigPartition(
        new ConfigPattern(
          "mysql_database"
        )
      )
    );
    partitions.add(
      new ConfigPartition(
        new ConfigPattern(
          "mysql_host"
        ),
        new HostNamePredicate(),
        ConfigBase.DEFAULT_HOST
      )
    );
    partitions.add(
      new ConfigPartition(
        new ConfigPattern(
          "mysql_password"
        )
      )
    );
    partitions.add(
      new ConfigPartition(
        new ConfigPattern(
          "mysql_port"
        ),
        new PortNumberPredicate(false),
        String.valueOf(ConfigBase.DEFAULT_MYSQL_PORT)
      )
    );
    partitions.add(
      new ConfigPartition(
        new ConfigPattern(
          "mysql_table"
        ),
        "data"
      )
    );
    partitions.add(
      new ConfigPartition(
        new ConfigPattern(
          "mysql_username"
        )
      )
    );
    partitions.add(
      new ConfigPartition(
        new ConfigPattern(
          "oracle_host"
        ),
        new HostNamePredicate(),
        ConfigBase.DEFAULT_HOST
      )
    );
    partitions.add(
      new ConfigPartition(
        new ConfigPattern(
          "oracle_password"
        )
      )
    );
    partitions.add(
      new ConfigPartition(
        new ConfigPattern(
          "oracle_port"
        ),
        new PortNumberPredicate(false),
        String.valueOf(ConfigBase.DEFAULT_ORACLE_PORT)
      )
    );
    partitions.add(
      new ConfigPartition(
        new ConfigPattern(
          "oracle_username"
        )
      )
    );
    partitions.add(
      new ConfigPartition(
        new ConfigPattern(
          "oracle_servicename"
        ),
        ConfigBase.DEFAULT_ORACLE_SERVICENAME
      )
    );
    partitions.add(
      new ConfigPartition(
        new ConfigPattern(
          "party_count"
        ),
        new ConfigPattern(
          "[5-9]|[1-9][0-9]+"
        ),
        Integer.toString(ConfigBase.DEFAULT_PARTY_COUNT)
      )
    );
    partitions.add(
      new ConfigPartition(
        new ConfigPattern(
          "performance_log_file"
        ),
        ConfigBase.DEFAULT_PERFORMANCE_LOG_FILE
      )
    );
    partitions.add(
      new ConfigPartition(
        new ConfigPattern(
          "postgresql_database"
        )
      )
    );
    partitions.add(
      new ConfigPartition(
        new ConfigPattern(
          "postgresql_host"
        ),
        new HostNamePredicate(),
        ConfigBase.DEFAULT_HOST
      )
    );
    partitions.add(
      new ConfigPartition(
        new ConfigPattern(
          "postgresql_password"
        )
      )
    );
    partitions.add(
      new ConfigPartition(
        new ConfigPattern(
          "postgresql_port"
        ),
        new PortNumberPredicate(false),
        String.valueOf(ConfigBase.DEFAULT_POSTGRESQL_PORT)
      )
    );
    partitions.add(
      new ConfigPartition(
        new ConfigPattern(
          "postgresql_username"
        )
      )
    );
    partitions.add(
      new ConfigPartition(
        new ConfigPattern(
          "raw_connect_host_[1-9][0-9]*"
        ),
        new HostNamePredicate(),
        ConfigBase.DEFAULT_HOST
      )
    );
    partitions.add(
      new ConfigPartition(
        new ConfigPattern(
          "raw_connect_port_[1-9][0-9]*"
        ),
        new PortNumberPredicate(false),
        Integer.toString(ConfigBase.DEFAULT_RAW_PORT)
      )
    );
    partitions.add(
      new ConfigPartition(
        new ConfigPattern(
          "raw_listen_host"
        ),
        new HostNamePredicate(),
        ConfigBase.DEFAULT_HOST
      )
    );
    partitions.add(
      new ConfigPartition(
        new ConfigPattern(
          "raw_listen_port"
        ),
        new PortNumberPredicate(false),
        Integer.toString(ConfigBase.DEFAULT_RAW_PORT)
      )
    );
    partitions.add(
      new ConfigPartition(
        new ConfigPattern(
          "tmpdir"
        ),
        ConfigBase.DEFAULT_TMPDIR
      )
    );
    partitions.add(
      new ConfigPartition(
        new ConfigPattern(
          "use_files_for_large_memory"
        ),
        Boolean.toString(ConfigBase.DEFAULT_USE_FILES_FOR_LARGE_MEMORY)
      )
    );
    partitions.add(
      new ConfigPartition(
        new ConfigPattern(
          "valid_dvs"
        ),
        "Income"
      )
    );
    partitions.add(
      new ConfigPartition(
        new ConfigPattern(
          "valid_keys"
        ),
        "CaseID"
      )
    );
    partitions.add(
      new ConfigPartition(
        new ConfigPattern(
          "valid_ivs"
        ),
        "Attrib_A,Group_X,Attrib_B"
      )
    );
    partitions.add(
      new ConfigPartition(
        new ConfigPattern(
          ".*_domain_quote"
        ),
        "true"
      )
    );
    partitions.add(
      new ConfigPartition(
        new ConfigPattern(
          ".*_domain_values"
        ),
        "1"
      )
    );
    PARTITIONS = Collections.unmodifiableList(partitions);
  }
}

/**
 *
 * @throws java.lang.Throwable
 * (or a subclass thereof)
 * if and only if the operation failed
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

protected ConfigBase(
) {
  super();
}

/**
 *
 * @throws java.lang.Throwable
 * (or a subclass thereof)
 * if and only if the operation failed
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

protected ConfigBase(
  final Iterable<ConfigPartition> partitions
) {
  super(partitions);
}

/**
 *
 * @throws java.lang.Throwable
 * (or a subclass thereof)
 * if and only if the operation failed
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

protected ConfigBase(
  final Iterable<ConfigPartition> partitions,
  final Iterable<? extends ConfigMapper> aliases
) {
  super(partitions, aliases);
}

/**
 *
 * @throws java.lang.Throwable
 * (or a subclass thereof)
 * if and only if the operation failed
 *
 * @inheritancenotes
 * If you are implementing an abstract subclass of this class, then you
 * should wrap this constructor with a protected constructor.
 * For example:
 *<!--code{.java}--><blockquote><pre>
 *public abstract class ConfigBase&lt;T extends ConfigBase&lt;T&gt;&gt;
 *extends com.stealthsoftwareinc.commercial.safrn.ConfigBase&lt;T&gt;
 *{
 *<b></b>
 *<b></b>// ...
 *<b></b>
 *protected ConfigBase(
 *  final T src,
 *  final boolean copy,
 *  final boolean empty
 *) {
 *  super(src, copy, empty);
 *}
 *<b></b>
 *<b></b>// ...
 *<b></b>
 *}
 *</pre></blockquote>
 * <p>
 * If you are implementing a non-abstract subclass of this class, then
 * your class should be final and you should wrap this constructor with
 * a private constructor.
 * For example:
 * </p>
 *<!--code{.java}--><blockquote><pre>
 *public final class Config
 *extends ConfigBase&lt;Config&gt;
 *{
 *<b></b>
 *<b></b>// ...
 *<b></b>
 *private Config(
 *  final Config src,
 *  final boolean copy,
 *  final boolean empty
 *) {
 *  super(src, copy, empty);
 *}
 *<b></b>
 *<b></b>// ...
 *<b></b>
 *}
 *</pre></blockquote>
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

protected ConfigBase(
  final T src,
  final boolean copy,
  final boolean empty
) {
  super(src, copy, empty);
}

}
