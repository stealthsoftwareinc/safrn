#
# This file is from the SAFRN package.
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

all:

## begin_variables

GATBPS = 'gatbps'

GATBPSFLAGS =

## end_variables

## begin_rules

.PHONY: FORCE
.PHONY: all

.SUFFIXES:
.SUFFIXES: .texi

.texi:
	$(GATBPS) \
  '--append-path=doc/texi' \
  '--no-unicode' \
  '--plain' \
  $(GATBPSFLAGS) \
  '--' \
  $@ \
;

COPYING: FORCE

FORCE:

NEWS: FORCE

all: COPYING
all: FORCE
all: NEWS

## end_rules
