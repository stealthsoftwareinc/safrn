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
 * <code>com.stealthsoftwareinc.commercial.safrn.Config</code>
 * Java class.
 *
 * @since SAFRN&nbsp;0.1.0 (2017-11-08)
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

package com.stealthsoftwareinc.commercial.safrn;

/* begin_imports */

import com.stealthsoftwareinc.commercial.safrn.ConfigBase;

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

public final class Config
extends ConfigBase<Config>
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

public static final Config NULL_CONFIG;

static {
  NULL_CONFIG = new Config().makeImmutable();
}

/**
 *
 * @throws java.lang.Throwable
 * (or a subclass thereof)
 * if and only if the operation failed
 *
 * @since SAFRN&nbsp;0.1.0 (2017-11-08)
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

public Config(
) {
  super(PARTITIONS, ALIASES);
}

/**
 *
 * @throws java.lang.Throwable
 * (or a subclass thereof)
 * if and only if the operation failed
 *
 * @since SAFRN&nbsp;0.1.0 (2017-11-08)
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

private Config(
  final Config src,
  final boolean copy,
  final boolean empty
) {
  super(src, copy, empty);
}

/**
 * Gets an immutable configuration with no explicit entries.
 *
 * @return
 * an immutable configuration with no explicit entries
 *
 * @throws java.lang.Throwable
 * (or a subclass thereof)
 * if and only if the operation failed
 *
 * @accesses
 * nothing
 *
 * @modifies
 * nothing
 *
 * @wellbehaved
 * always
 *
 * @available
 * always
 *
 * @since SAFRN&nbsp;0.1.0 (2017-11-08)
 *
 * @inheritancenotes
 * none
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

@Override
protected final Config getNull(
) {
  return Config.NULL_CONFIG;
}

/**
 * Gets this configuration.
 *
 * @return
 * this configuration
 *
 * @throws java.lang.Throwable
 * (or a subclass thereof)
 * if and only if the operation failed
 *
 * @accesses
 * nothing
 *
 * @modifies
 * nothing
 *
 * @wellbehaved
 * always
 *
 * @available
 * always
 *
 * @since SAFRN&nbsp;0.1.0 (2017-11-08)
 *
 * @inheritancenotes
 * none
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

@Override
protected final Config getThis(
) {
  return this;
}

/**
 *
 * @throws java.lang.Throwable
 * (or a subclass thereof)
 * if and only if the operation failed
 *
 * @available
 * always
 *
 * @since SAFRN&nbsp;0.1.0 (2017-11-08)
 *
 * @inheritancenotes
 * none
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

@Override
public final Config toMutable(
  final boolean empty
) {
  return new Config(this, true, empty);
}

/**
 * Gets this configuration if it is an unmodifiable view, or a new
 * unmodifiable view of this configuration if not.
 *
 * @return
 * this configuration if it is an unmodifiable view, or a new
 * unmodifiable view of this configuration if not
 *
 * @throws java.lang.Throwable
 * (or a subclass thereof)
 * if and only if the operation failed
 *
 * @accesses
 * nothing
 *
 * @modifies
 * nothing
 *
 * @wellbehaved
 * always
 *
 * @available
 * always
 *
 * @since SAFRN&nbsp;0.1.0 (2017-11-08)
 *
 * @inheritancenotes
 * none
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

@Override
public final Config toView(
) {
  if (this.isView()) {
    return this;
  }
  return new Config(this, false, false);
}

}
