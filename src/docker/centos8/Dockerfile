#
# Copyright 2020 Stealth Software Technologies, Inc.
#

FROM centos:8

RUN yum -y install epel-release yum-utils
RUN yum config-manager --set-enabled PowerTools

RUN yum -y install diffutils which
RUN yum -y install \
      make \
      cmake \
      wget \
      jq \
      autoconf \
      openssl \
      openssl-devel \
      m4 \
      libtool \
      automake \
      texinfo \
      gcc \
      gcc-c++ \
      json-devel \
      libevent \
      libevent-devel \
      nettle \
      nettle-devel \
      gsl \
      gsl-devel
