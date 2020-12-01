#
# For the copyright information for this file, please search up the
# directory tree for the first README.md file.
#

FROM centos:7

RUN yum -y install \
  autoconf \
  automake \
  bc \
  coreutils \
  gawk \
  gcc \
  gcc-c++ \
  gettext-devel \
  gmp \
  gmp-devel \
  java-1.8.0-openjdk \
  java-1.8.0-openjdk-devel \
  libtool \
  make \
  maven \
  nettle \
  nettle-devel \
  perl-devel \
  redhat-rpm-config \
  rpm-build \
  sed \
  tar \
  zlib-devel \
;

COPY git-2.20.1.tar.gz /x/

RUN : \
  && cd /x \
  && tar xf git-2.20.1.tar.gz \
  && cd git-2.20.1 \
  && make configure \
  && ./configure \
  && make \
  && make install \
;

COPY texinfo-6.3.tar.xz /x/

RUN : \
  && cd /x \
  && tar xf texinfo-6.3.tar.xz \
  && cd texinfo-6.3 \
  && ./configure \
  && make \
  && make install \
;

COPY sst.gitbundle /x/

RUN : \
  && cd /x \
  && git clone sst.gitbundle sst \
  && cd sst \
  && install/on-centos \
;

ARG ARG_PACKAGE_VERSION
ARG ARG_PACKAGE_VERSION_RPM_R
ARG ARG_PACKAGE_VERSION_RPM_V

COPY safrn-*.tar.* /root/rpmbuild/SOURCES/
COPY rpm.spec /root/rpmbuild/SPECS/

RUN rpmbuild -ba \
  -D "PACKAGE_VERSION $ARG_PACKAGE_VERSION" \
  -D "PACKAGE_VERSION_RPM_R $ARG_PACKAGE_VERSION_RPM_R" \
  -D "PACKAGE_VERSION_RPM_V $ARG_PACKAGE_VERSION_RPM_V" \
  /root/rpmbuild/SPECS/rpm.spec \
;

CMD sh
