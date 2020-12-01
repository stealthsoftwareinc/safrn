dnl
dnl This file is from the SAFRN package.
dnl
dnl The following copyright notice is generally applicable:
dnl
dnl      Copyright (C)
dnl         Stealth Software Technologies Commercial, Inc.
dnl
dnl The full copyright information depends on the distribution
dnl of the package. For more information, see the COPYING file.
dnl However, depending on the context in which you are viewing
dnl this file, the COPYING file may not be available.
dnl
AC_DEFUN([DEFINE_SAFRN_ENABLE_UNDEFINED_BEHAVIOR_CHECKS], [[{

#
# The block that contains this comment is the expansion of the
# DEFINE_SAFRN_ENABLE_UNDEFINED_BEHAVIOR_CHECKS macro.
#]dnl
m4_ifdef(
  [DEFINE_SAFRN_ENABLE_UNDEFINED_BEHAVIOR_CHECKS_HAS_BEEN_CALLED],
  [gatbps_fatal([
    DEFINE_SAFRN_ENABLE_UNDEFINED_BEHAVIOR_CHECKS has already been called
  ])],
  [m4_define([DEFINE_SAFRN_ENABLE_UNDEFINED_BEHAVIOR_CHECKS_HAS_BEEN_CALLED])])[]dnl
m4_if(
  m4_eval([$# != 0]),
  [1],
  [gatbps_fatal([
    DEFINE_SAFRN_ENABLE_UNDEFINED_BEHAVIOR_CHECKS requires exactly 0
    arguments
    ($# ]m4_if([$#], [1], [[was]], [[were]])[ given)
  ])])[]dnl
[

]AC_REQUIRE([DEFINE_SAFRN_ENABLE_INVALID_ARGUMENT_CHECKS])[

]GATBPS_ARG_ENABLE_BOOL(
  [
    permission to enable undefined behavior checks
  ],
  [SAFRN_ENABLE_UNDEFINED_BEHAVIOR_CHECKS],
  [undefined-behavior-checks],
  [yes],
  [
    enable undefined behavior checks
  ],
  [
    disable undefined behavior checks
  ])[

case "$][{SAFRN_ENABLE_INVALID_ARGUMENT_CHECKS}" in
  '0')
    case "$][{SAFRN_ENABLE_UNDEFINED_BEHAVIOR_CHECKS}" in
      '1')
        ]GATBPS_MSG_ERROR([
          undefined behavior checks must be disabled when invalid
          argument checks are disabled
        ])[
      ;;
    esac
  ;;
esac

:;}]])[]dnl
