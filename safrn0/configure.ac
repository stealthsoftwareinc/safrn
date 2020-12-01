dnl
dnl For the copyright information for this file, please search up the
dnl directory tree for the first README.md file.
dnl

[

]AC_INIT(
  [[SAFRN]],
  m4_esyscmd_s([
    'sh' '-' 'build-aux/VERSION.sh'
  ])m4_if(m4_sysval, [0], [], [m4_fatal([VERSION.sh failed])]),
  [[]],
  [[safrn]],
  [[]])[

]m4_include([build-aux/prelude.ac])[

]gatbps_check_macros[ || 'exit' "${?}"

]GATBPS_DEFINE_VERSION(
  [AC_],
  [PACKAGE_VERSION])[

]GATBPS_DEFINE_DATE(
  [AC_],
  [PACKAGE_DATE],
  m4_esyscmd_s([
    'sh' '-' 'build-aux/DATE.sh'
  ])m4_if(m4_sysval, [0], [], [m4_fatal([DATE.sh failed])]))[

]AC_COPYRIGHT(
[[Some parts of this file are from the SAFRN package.

The following copyright notice is generally applicable:

     Copyright (C)
        Stealth Software Technologies Commercial, Inc.

The full copyright information depends on the distribution
of the package. For more information, see the COPYING file.
However, depending on the context in which you are viewing
this file, the COPYING file may not be available.
]])[

]AC_CONFIG_AUX_DIR([build-aux])[
]AC_CONFIG_MACRO_DIR([m4])[
]AC_CONFIG_MACRO_DIRS([m4])[
]AC_CONFIG_SRCDIR([doc/safrn.texi])[

]AM_INIT_AUTOMAKE([
  -Wall
  foreign
  nostdinc
  subdir-objects
  tar-ustar
])[

]DEFINE_ALL[

]LT_INIT([win32-dll])[

]dnl begin_includes
[

]m4_include([build-aux/autogen.ac])[
]m4_include([docker/safrn.ac])[
]m4_include([rpm/sstc-safrn/centos-7/x86_64/rpm.ac])[
]m4_include([src/shadow/com/stealthsoftwareinc/commercial/safrn/BuildConfig.java.ac])[
]m4_include([src/shadow/com/stealthsoftwareinc/commercial/safrn/BuildConfig.java.im.ac])[

]dnl end_includes
[
]

GATBPS_CONFIG_FILE([build-aux/INPUT_FILTER.sh])
GATBPS_CONFIG_FILE([build-aux/PACKAGE_STRING.df])
GATBPS_CONFIG_FILE([doc/texi/PACKAGEDATE.texi], [doc/texi/PACKAGEDATE.texi.in], [maintainer-clean])
GATBPS_CONFIG_FILE([doc/texi/PACKAGEVERSION.texi], [doc/texi/PACKAGEVERSION.texi.in], [maintainer-clean])
GATBPS_CONFIG_FILE([pom.xml])

GATBPS_CONFIG_FILE_SUBST
GATBPS_CONFIG_LATER_SUBST
GATBPS_CP_SUBST
GATBPS_DOCKER_BUILD_SUBST
GATBPS_GIT_ARCHIVE_SUBST
GATBPS_GIT_BUNDLE_SUBST
GATBPS_GIT_CLONE_SUBST
GATBPS_JAVA_CLASS_SUBST
GATBPS_JAVA_SUBST
GATBPS_M4_SUBST
GATBPS_TAR_SUBST
GATBPS_WGET_SUBST
GATBPS_XZ_SUBST

AC_CONFIG_FILES([Makefile])
AC_CONFIG_HEADERS([config.h])

AC_OUTPUT[

]
