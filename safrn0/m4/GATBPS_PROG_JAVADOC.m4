dnl
dnl This file was generated by GATBPS 0.1.0-5459+g499c8df8, which was
dnl released on 2020-08-20. Before changing it, make sure
dnl you're doing the right thing. Depending on how GATBPS
dnl is being used, your changes may be automatically lost.
dnl A short description of this file follows.
dnl
dnl Special file: GATBPS_PROG_JAVADOC.m4
dnl
dnl For more information, see the GATBPS manual.
dnl
#serial 20200820
AC_DEFUN([GATBPS_PROG_JAVADOC_check_macros], [dnl
m4_ifndef(
  [gatbps_fatal],
  [dnl
m4_errprintn(
m4_location[: error: ]dnl
[gatbps_fatal ]dnl
[is not defined]dnl
)[]dnl
m4_fatal(
[did you forget ]dnl
[gatbps_fatal.m4?]dnl
)[]dnl
])[]dnl
m4_ifndef(
  [gatbps_fatal_check_macros],
  [dnl
m4_errprintn(
m4_location[: error: ]dnl
[gatbps_fatal_check_macros ]dnl
[is not defined]dnl
)[]dnl
m4_fatal(
[this means that there is a bug in GATBPS]dnl
)[]dnl
])[]dnl
gatbps_fatal_check_macros[]dnl
]m4_define(
  [gatbps_check_macros],
  m4_ifndef(
    [gatbps_check_macros],
    [[[# gatbps_check_macros]dnl
]],
    [m4_defn([gatbps_check_macros])])dnl
[GATBPS_PROG_JAVADOC_check_macros[]dnl
]))[]dnl
AC_DEFUN([GATBPS_PROG_JAVADOC], [[{

#
# The block that contains this comment is the expansion of the
# GATBPS_PROG_JAVADOC macro.
#]dnl
m4_ifdef(
  [GATBPS_PROG_JAVADOC_HAS_BEEN_CALLED],
  [gatbps_fatal([
    GATBPS_PROG_JAVADOC has already been called
  ])],
  [m4_define([GATBPS_PROG_JAVADOC_HAS_BEEN_CALLED])])[]dnl
m4_if(
  m4_eval([$# != 0]),
  [1],
  [gatbps_fatal([
    GATBPS_PROG_JAVADOC requires exactly 0 arguments
    ($# ]m4_if([$#], [1], [[was]], [[were]])[ given)
  ])])[]dnl
[

]AC_CHECK_PROGS(
  [JAVADOC],
  [javadoc])[

]AC_ARG_VAR(
  [JAVADOC],
  [
    the command to use to run javadoc
  ])[

:;}]])[]dnl
dnl
dnl The authors of this file have waived all copyright and
dnl related or neighboring rights to the extent permitted by
dnl law as described by the CC0 1.0 Universal Public Domain
dnl Dedication. You should have received a copy of the full
dnl dedication along with this file, typically as a file
dnl named <CC0-1.0.txt>. If not, it may be available at
dnl <https://creativecommons.org/publicdomain/zero/1.0/>.
dnl
