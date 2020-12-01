##
## For the copyright information for this file, please search up the
## directory tree for the first README.md file.
##

#
# Some parts of this file are from the SAFRN package.
#
# The following copyright notice is generally applicable:
#
#      Copyright (C)
#         Stealth Software Technologies Commercial, Inc.
#
# The full copyright information depends on the distribution
# of the package. For more information, see the COPYING file.
# However, depending on the context in which you are viewing
# this file, the COPYING file may not be available.
#

## begin_variables

ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = \
  -I include \
  -I $(srcdir)/include \
$(prevent_a_blank_line)

AM_MAKEINFOFLAGS = \
  -I doc/texi \
  -I $(srcdir)/doc/texi \
$(prevent_a_blank_line)

AM_MAKEINFOHTMLFLAGS = \
  $(AM_MAKEINFOFLAGS) \
  --css-include=texinfo.css \
  --no-headers \
  --no-split \
  --set-customization-variable TOP_NODE_UP=Top \
$(prevent_a_blank_line)

AM_TEXI2DVIFLAGS = \
  $(AM_MAKEINFOFLAGS) \
$(prevent_a_blank_line)

BUILT_SOURCES =

DISTCLEANFILES =

EXTRA_DIST =
EXTRA_DIST += DATE
EXTRA_DIST += README.md
EXTRA_DIST += VERSION
EXTRA_DIST += autogen
EXTRA_DIST += build-aux/DATE.sh
EXTRA_DIST += build-aux/VERSION.sh
EXTRA_DIST += build-aux/echo.sh
EXTRA_DIST += build-aux/gen-configure.sh
EXTRA_DIST += build-aux/sh-form.sh
EXTRA_DIST += src/bash/functions
EXTRA_DIST += src/bash/preludes
EXTRA_DIST += src/bash/scripts/gitlab-ci
EXTRA_DIST += src/install/from-dist
EXTRA_DIST += src/install/from-repo
EXTRA_DIST += src/install/on-x

GATBPS_JAVADOCFLAGS =
GATBPS_JAVADOCFLAGS += \
  '-tag' 'param' \
  '-tag' 'return' \
  '-tag' 'throws' \
  '-tag' 'accesses:a:Accesses:' \
  '-tag' 'modifies:a:Modifies:' \
  '-tag' 'wellbehaved:a:Well-Behaved:' \
  '-tag' 'available:a:Available:' \
  '-tag' 'see' \
  '-tag' 'since' \
  '-tag' 'inheritancenotes:a:Inheritance Notes:' \
  '-tag' 'examples:a:Examples:' \
  '-tag' 'notablechanges:a:Notable Changes:' \
$(prevent_a_blank_line)

MAINTAINERCLEANFILES =
MAINTAINERCLEANFILES += DATE
MAINTAINERCLEANFILES += VERSION

MOSTLYCLEANFILES =
MOSTLYCLEANFILES += $(java_src)

SUBDIRS =

SUFFIXES =

TESTS =

TEXI2DVI = \
  'texi2dvi' \
  $(AM_TEXI2DVIFLAGS) \
  $(TEXI2DVIFLAGS) \
$(prevent_a_blank_line)

TMPEXT = .tmp

XFAIL_TESTS =

bin_PROGRAMS =

bin_SCRIPTS =

check_PROGRAMS =

dist_java_DATA =

dist_pkgdata_DATA =

docbook_dep =
docbook_dep += $(doc_safrn_TEXINFOS)

docbook_dist_dep =
docbook_dist_dep += $(doc_safrn_TEXINFOS)
docbook_dist_dep += doc/safrn.texi

docbook_dist_dst =
docbook_dist_dst += doc/safrn-$(PACKAGE_VERSION)-docbook.xml

docbook_dist_src =
docbook_dist_src += doc/safrn-docbook.xml

docbook_dst =
docbook_dst += doc/safrn-docbook.xml

docbook_src =
docbook_src += doc/safrn.texi

doxygen_dep =

doxygen_dfv =

doxygen_dst =
doxygen_dst += doc/safrn-doxygen

doxygen_src =

dvi_dist_dep =
dvi_dist_dep += $(doc_safrn_TEXINFOS)
dvi_dist_dep += doc/safrn.texi

dvi_dist_dst =
dvi_dist_dst += doc/safrn-$(PACKAGE_VERSION).dvi

dvi_dist_src =
dvi_dist_src += doc/safrn.dvi

html_dist_dep =
html_dist_dep += $(doc_safrn_TEXINFOS)
html_dist_dep += doc/safrn.texi

html_dist_dst =
html_dist_dst += doc/safrn-$(PACKAGE_VERSION).html

html_dist_src =
html_dist_src += doc/safrn.html

include_HEADERS =

info_TEXINFOS =
info_TEXINFOS += doc/safrn.texi

jar_classpath_files =

java_dist_dst = lib/safrn-$(PACKAGE_VERSION).jar

javadoc_JAVADOCFLAGS =
javadoc_JAVADOCFLAGS += $(GATBPS_JAVADOCFLAGS)
javadoc_JAVADOCFLAGS += '-windowtitle' '$(PACKAGE_STRING) Javadoc Reference'

javadoc_dist_dep =
javadoc_dist_dep += $(javadoc_src)

javadoc_dist_dst =
javadoc_dist_dst += doc/safrn-$(PACKAGE_VERSION)-javadoc

javadoc_dist_src =
javadoc_dist_src += $(javadoc_dst)

javadoc_dst =
javadoc_dst += doc/safrn-javadoc

lib_LTLIBRARIES =

libexec_PROGRAMS =

libexec_SCRIPTS =

nodist_noinst_HEADERS =

nodist_pkginclude_HEADERS =

noinst_HEADERS =

noinst_LIBRARIES =

noinst_LTLIBRARIES =

noinst_PROGRAMS =

pdf_dist_dep =
pdf_dist_dep += $(doc_safrn_TEXINFOS)
pdf_dist_dep += doc/safrn.texi

pdf_dist_dst =
pdf_dist_dst += doc/safrn-$(PACKAGE_VERSION).pdf

pdf_dist_src =
pdf_dist_src += doc/safrn.pdf

pkgdata_DATA =

pkginclude_HEADERS =

pkglibexec_PROGRAMS =

pkglibexec_SCRIPTS =

pkgsysconfdir = $(sysconfdir)/$(PACKAGE_TARNAME)

plaintext_dep =
plaintext_dep += $(doc_safrn_TEXINFOS)

plaintext_dist_dep =
plaintext_dist_dep += $(doc_safrn_TEXINFOS)
plaintext_dist_dep += doc/safrn.texi

plaintext_dist_dst =
plaintext_dist_dst += doc/safrn-$(PACKAGE_VERSION).txt

plaintext_dist_src =
plaintext_dist_src += doc/safrn.txt

plaintext_dst =
plaintext_dst += doc/safrn.txt

plaintext_src =
plaintext_src += doc/safrn.texi

ps_dist_dep =
ps_dist_dep += $(doc_safrn_TEXINFOS)
ps_dist_dep += doc/safrn.texi

ps_dist_dst =
ps_dist_dst += doc/safrn-$(PACKAGE_VERSION).ps

ps_dist_src =
ps_dist_src += doc/safrn.ps

xml_dep =
xml_dep += $(doc_safrn_TEXINFOS)

xml_dist_dep =
xml_dist_dep += $(doc_safrn_TEXINFOS)
xml_dist_dep += doc/safrn.texi

xml_dist_dst =
xml_dist_dst += doc/safrn-$(PACKAGE_VERSION).xml

xml_dist_src =
xml_dist_src += doc/safrn.xml

xml_dst =
xml_dst += doc/safrn.xml

xml_src =
xml_src += doc/safrn.texi

## end_variables

## begin_variable_additions

## end_variable_additions

## begin_rules

$(distdir).tar.gz:
	$(MAKE) $(AM_MAKEFLAGS) dist-gzip

.PHONY: FORCE
.PHONY: maven_prep

DATE:
	sh build-aux/DATE.sh >DATE.tmp$(TMPEXT)
	mv DATE.tmp$(TMPEXT) DATE

FORCE:

VERSION:
	sh build-aux/VERSION.sh >VERSION.tmp$(TMPEXT)
	mv VERSION.tmp$(TMPEXT) VERSION

maven-prep: FORCE
maven-prep: src/main/java/com/stealthsoftwareinc/commercial/safrn/BuildConfig.java

## end_rules

if HAVE_JAVA
if HAVE_JAVAC
## begin_rules

all-local: java-main

install-data-local: install-java-main

uninstall-local: uninstall-java-main

## end_rules
endif
endif

## begin_includes

include $(srcdir)/build-aux/autogen.am
include $(srcdir)/docker/safrn.am
include $(srcdir)/rpm/sstc-safrn/centos-7/x86_64/rpm.am
include $(srcdir)/src/shadow/com/stealthsoftwareinc/commercial/safrn/BuildConfig.java.am
include $(srcdir)/src/shadow/com/stealthsoftwareinc/commercial/safrn/BuildConfig.java.im-am

## end_includes

## begin_includes

include $(srcdir)/build-aux/GATBPS_CONFIG_FILE_RULES.am
include $(srcdir)/build-aux/GATBPS_CONFIG_LATER_RULES.am
include $(srcdir)/build-aux/GATBPS_CP_RULES.am
include $(srcdir)/build-aux/GATBPS_DOCKER_BUILD_RULES.am
include $(srcdir)/build-aux/GATBPS_GIT_ARCHIVE_RULES.am
include $(srcdir)/build-aux/GATBPS_GIT_BUNDLE_RULES.am
include $(srcdir)/build-aux/GATBPS_GIT_CLONE_RULES.am
include $(srcdir)/build-aux/GATBPS_JAVA_CLASS_RULES.am
include $(srcdir)/build-aux/GATBPS_JAVA_RULES.am
include $(srcdir)/build-aux/GATBPS_M4_RULES.am
include $(srcdir)/build-aux/GATBPS_RECIPE_MARKER.am
include $(srcdir)/build-aux/GATBPS_TAR_RULES.am
include $(srcdir)/build-aux/GATBPS_VPATH_SEARCH.am
include $(srcdir)/build-aux/GATBPS_V_NOP.am
include $(srcdir)/build-aux/GATBPS_V_PAD.am
include $(srcdir)/build-aux/GATBPS_WGET_RULES.am
include $(srcdir)/build-aux/GATBPS_XZ_RULES.am
include $(srcdir)/build-aux/cp.am
include $(srcdir)/build-aux/docbook-dist.am
include $(srcdir)/build-aux/docbook.am
include $(srcdir)/build-aux/docker.am
include $(srcdir)/build-aux/doxygen.am
include $(srcdir)/build-aux/dvi-dist.am
include $(srcdir)/build-aux/gatbps.am
include $(srcdir)/build-aux/git.am
include $(srcdir)/build-aux/gzip.am
include $(srcdir)/build-aux/html-dist.am
include $(srcdir)/build-aux/java-dist.am
include $(srcdir)/build-aux/java.am
include $(srcdir)/build-aux/javadoc-dist.am
include $(srcdir)/build-aux/javadoc.am
include $(srcdir)/build-aux/m4.am
include $(srcdir)/build-aux/pdf-dist.am
include $(srcdir)/build-aux/plaintext-dist.am
include $(srcdir)/build-aux/plaintext.am
include $(srcdir)/build-aux/ps-dist.am
include $(srcdir)/build-aux/tar.am
include $(srcdir)/build-aux/wget.am
include $(srcdir)/build-aux/xml-dist.am
include $(srcdir)/build-aux/xml.am
include $(srcdir)/build-aux/xz.am

## end_includes
