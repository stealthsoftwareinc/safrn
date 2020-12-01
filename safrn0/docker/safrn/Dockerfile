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

FROM alpine:3.6
RUN 'apk' \
  '--no-cache' \
  'add' \
  'mariadb-client-libs' \
  'openjdk8-jre' \
;
ADD [ \
  "overlay.tar.xz", \
  "/" \
]
