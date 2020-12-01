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
AC_DEFUN([DEFINE_ALL], [[{

#
# The block that contains this comment is the expansion of the
# DEFINE_ALL macro.
#]dnl
m4_ifdef(
  [DEFINE_ALL_HAS_BEEN_CALLED],
  [gatbps_fatal([
    DEFINE_ALL has already been called
  ])],
  [m4_define([DEFINE_ALL_HAS_BEEN_CALLED])])[]dnl
m4_if(
  m4_eval([$# != 0]),
  [1],
  [gatbps_fatal([
    DEFINE_ALL requires exactly 0 arguments
    ($# ]m4_if([$#], [1], [[was]], [[were]])[ given)
  ])])[]dnl
[

]dnl begin_prerequisites
[

]AC_REQUIRE([DEFINE_docbookdir])[
]AC_REQUIRE([DEFINE_javadir])[
]AC_REQUIRE([DEFINE_javadocdir])[
]AC_REQUIRE([DEFINE_plaintextdir])[
]AC_REQUIRE([DEFINE_xmldir])[

]dnl end_prerequisites
[

]AC_REQUIRE([DEFINE_ALL_WGET_URLS])[
]AC_REQUIRE([DEFINE_ALL_GITBUNDLE_URLS])[
]AC_REQUIRE([DEFINE_ALL_GITBUNDLE_COMMITS])[

]dnl begin_prerequisites
[

]dnl end_prerequisites
[

]dnl begin_prerequisites
[

]AC_REQUIRE([DEFINE_AR])[
]AC_REQUIRE([DEFINE_AWK])[
]AC_REQUIRE([DEFINE_CC])[
]AC_REQUIRE([DEFINE_CXX])[
]AC_REQUIRE([DEFINE_DOCKER])[
]AC_REQUIRE([DEFINE_DOXYGEN])[
]AC_REQUIRE([DEFINE_GIT])[
]AC_REQUIRE([DEFINE_GREP])[
]AC_REQUIRE([DEFINE_GZIP])[
]AC_REQUIRE([DEFINE_JAR])[
]AC_REQUIRE([DEFINE_JAVA])[
]AC_REQUIRE([DEFINE_JAVAC])[
]AC_REQUIRE([DEFINE_JAVADOC])[
]AC_REQUIRE([DEFINE_JDEPS])[
]AC_REQUIRE([DEFINE_LN_S])[
]AC_REQUIRE([DEFINE_M4])[
]AC_REQUIRE([DEFINE_MAKEINFO])[
]AC_REQUIRE([DEFINE_MKDIR_P])[
]AC_REQUIRE([DEFINE_OPENSSL])[
]AC_REQUIRE([DEFINE_RANLIB])[
]AC_REQUIRE([DEFINE_SED])[
]AC_REQUIRE([DEFINE_TAR])[
]AC_REQUIRE([DEFINE_WGET])[
]AC_REQUIRE([DEFINE_XZ])[

]dnl end_prerequisites
[

]dnl begin_prerequisites
[

]AC_REQUIRE([DEFINE_CLASSPATH])[
]AC_REQUIRE([DEFINE_JAVACFLAGS])[

]dnl end_prerequisites
[

]AC_REQUIRE([DEFINE_AT])[
]AC_REQUIRE([DEFINE_CLASSPATH_SEPARATOR])[
]AC_REQUIRE([DEFINE_DOCKER_BUILD_FLAGS])[
]AC_REQUIRE([DEFINE_EXEEXT])[
]AC_REQUIRE([DEFINE_SAFRN_ENABLE_INTERNAL_ERROR_CHECKS])[
]AC_REQUIRE([DEFINE_SAFRN_ENABLE_INVALID_ARGUMENT_CHECKS])[
]AC_REQUIRE([DEFINE_SAFRN_ENABLE_UNDEFINED_BEHAVIOR_CHECKS])[
]AC_REQUIRE([DEFINE_SOFT_INCLUDE])[

:;}]])[]dnl
