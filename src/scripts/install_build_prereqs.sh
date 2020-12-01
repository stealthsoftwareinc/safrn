#!/bin/bash
set -e

if which apt > /dev/null
then
	sudo apt-get -y install cmake make g++ gcc libssl-dev openssl libevent-dev unixodbc odbcinst unixodbc-dev git curl odbc-postgresql wget automake autoconf libtool texinfo

elif which yum > /dev/null
then
	sudo yum -y install epel-release
	sudo yum -y install git cmake make gcc-c++ gcc openssl openssl-devel unixODBC unixODBC-devel libevent-devel wget jq automake autoconf libtool texinfo

fi
