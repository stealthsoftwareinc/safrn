#
# For the copyright information for this file, please search up the
# directory tree for the first README.md file.
#

Name: sstc-safrn
Version: %{PACKAGE_VERSION_RPM_V}
Release: %{PACKAGE_VERSION_RPM_R}%{?dist}
Summary: A secure multiparty computation system for performing statistical analyses on private databases
License: Proprietary
Source0: safrn-%{PACKAGE_VERSION}.tar.gz

BuildRequires: autoconf
BuildRequires: automake
BuildRequires: bc
BuildRequires: coreutils
BuildRequires: gawk
BuildRequires: gcc
BuildRequires: gcc-c++
BuildRequires: gmp
BuildRequires: gmp-devel
BuildRequires: java-1.8.0-openjdk
BuildRequires: java-1.8.0-openjdk-devel
BuildRequires: libtool
BuildRequires: make
BuildRequires: maven
BuildRequires: nettle
BuildRequires: nettle-devel
BuildRequires: sed
BuildRequires: tar

Requires(post): glibc
Requires(postun): glibc
Requires: gmp
Requires: nettle

%description
A secure multiparty computation system for performing statistical
analyses on private databases.

%package uberjar-oracle12201

Summary: Uber jars

Requires: java-1.8.0-openjdk

%description uberjar-oracle12201
Uber jars

%prep
%setup -n safrn-%{PACKAGE_VERSION}

%build

./configure \
  --bindir='%{_bindir}' \
  --datadir='%{_datadir}' \
  --includedir='%{_includedir}' \
  --infodir='%{_infodir}' \
  --libdir='%{_libdir}' \
  --libexecdir='%{_libexecdir}' \
  --prefix='%{_prefix}' \
  --sbindir='%{_sbindir}' \
  CFLAGS='%{optflags}' \
  CPPFLAGS='-DNDEBUG' \
  LDFLAGS='%{__global_ldflags}' \
;

make maven-prep
mvn -P oracle package

mv \
  target/safrn-%{PACKAGE_VERSION}-back.jar \
  safrn_uberjar_oracle12201_back_server.jar \
;

mv \
  target/safrn-%{PACKAGE_VERSION}-front.jar \
  safrn_uberjar_oracle12201_front_server.jar \
;

cat >safrn_uberjar_oracle12201_back_server <<EOF
#! /bin/sh -
java -jar %{_datadir}/java/safrn/safrn_uberjar_oracle12201_back_server.jar "\$@"
EOF

chmod +x safrn_uberjar_oracle12201_back_server

cat >safrn_uberjar_oracle12201_front_server <<EOF
#! /bin/sh -
java -jar %{_datadir}/java/safrn/safrn_uberjar_oracle12201_front_server.jar "\$@"
EOF

chmod +x safrn_uberjar_oracle12201_front_server

%install
mkdir -p \
  %{buildroot}/%{_bindir} \
  %{buildroot}/%{_datadir}/java/safrn \
;
cp \
  safrn_uberjar_oracle12201_back_server \
  %{buildroot}/%{_bindir} \
;
cp \
  safrn_uberjar_oracle12201_front_server \
  %{buildroot}/%{_bindir} \
;
cp \
  safrn_uberjar_oracle12201_back_server.jar \
  %{buildroot}/%{_datadir}/java/safrn \
;
cp \
  safrn_uberjar_oracle12201_front_server.jar \
  %{buildroot}/%{_datadir}/java/safrn \
;

%files
%defattr(-, root, root, -)

%files uberjar-oracle12201
%defattr(-, root, root, -)
%{_bindir}/safrn_uberjar_oracle12201_*
%{_datadir}/java/safrn/safrn_uberjar_oracle12201_*

%post
/sbin/ldconfig

%postun
/sbin/ldconfig
