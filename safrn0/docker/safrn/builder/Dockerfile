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

RUN apk --no-cache add \
  alpine-sdk \
  autoconf \
  automake \
  file \
  g++ \
  gcc \
  git \
  jq \
  libtool \
  lzip \
  make \
  mariadb-dev \
  openjdk8 \
  rpm \
  tar \
  texinfo \
  xz \
;

COPY wficmp.gitbundle /x/

RUN : \
  && git clone /x/wficmp.gitbundle /x/wficmp \
  && cd /x/wficmp \
  && autoreconf -fi \
  && ./configure \
    --build=x86_64-unknown-linux \
    --host=x86_64-unknown-linux \
    --prefix=/usr \
  && make \
  && make install \
  && make DESTDIR=/x/overlay install \
;

COPY tgicmp.gitbundle /x/

RUN : \
  && git clone /x/tgicmp.gitbundle /x/tgicmp \
  && cd /x/tgicmp \
  && autoreconf -fi \
  && ./configure \
    --build=x86_64-unknown-linux \
    --host=x86_64-unknown-linux \
    --prefix=/usr \
  && make \
  && make install \
  && make DESTDIR=/x/overlay install \
;

COPY sst.gitbundle /x/

RUN : \
  && git clone /x/sst.gitbundle /x/sst \
  && cd /x/sst \
  && ./autogen \
  && CPPFLAGS= \
  && CPPFLAGS="$CPPFLAGS -I/usr/lib/jvm/java-1.8-openjdk/include" \
  && CPPFLAGS="$CPPFLAGS -I/usr/lib/jvm/java-1.8-openjdk/include/linux" \
  && ./configure \
    --build=x86_64-unknown-linux \
    --disable-invalid-argument-checks \
    --disable-static \
    --disable-undefined-behavior-checks \
    --host=x86_64-unknown-linux \
    --prefix=/usr \
    CPPFLAGS="$CPPFLAGS" \
    JAR=/usr/lib/jvm/java-1.8-openjdk/bin/jar \
    JAVAC=/usr/lib/jvm/java-1.8-openjdk/bin/javac \
    JDEPS=/usr/lib/jvm/java-1.8-openjdk/bin/jdeps \
  && make install-java-jardeps \
  && make DESTDIR=/x/overlay install-java-jardeps \
  && make \
  && make java \
  && make install \
  && make install-java \
  && make DESTDIR=/x/overlay install \
  && make DESTDIR=/x/overlay install-java \
;

COPY [ \
  "safrn.gitbundle", \
  "/x/safrn.gitbundle" \
]
RUN 'mkdir' \
  '/x/safrn' \
;
RUN 'git' \
  'clone' \
  '/x/safrn.gitbundle' \
  '/x/safrn/safrn' \
;
RUN 'cd' '/x/safrn/'* && { \
  ./autogen; \
}
RUN 'cd' '/x/safrn/'* && { \
  './configure' \
    '--build=x86_64-unknown-linux' \
    '--disable-invalid-argument-checks' \
    '--disable-static' \
    '--disable-undefined-behavior-checks' \
    '--host=x86_64-unknown-linux' \
    '--prefix=/usr' \
    'CPPFLAGS=-I/usr/lib/jvm/java-1.8-openjdk/include -I/usr/lib/jvm/java-1.8-openjdk/include/linux' \
    'JAR=/usr/lib/jvm/java-1.8-openjdk/bin/jar' \
    'JAVAC=/usr/lib/jvm/java-1.8-openjdk/bin/javac' \
    'JDEPS=/usr/lib/jvm/java-1.8-openjdk/bin/jdeps' \
  ; \
}
RUN : \
  && cd /x/safrn/* \
  && make install-java-jardeps \
  && make DESTDIR=/x/overlay install-java-jardeps \
;
RUN 'cd' '/x/safrn/'* && { \
  'make' \
    'all' \
  ; \
}
RUN 'cd' '/x/safrn/'* && { \
  'make' \
    'install' \
  ; \
}
RUN 'cd' '/x/safrn/'* && { \
  'make' \
    'DESTDIR=/x/overlay' \
    'install' \
  ; \
}
CMD [ \
  "/bin/sh" \
]
