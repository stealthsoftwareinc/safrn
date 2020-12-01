# SAFRN1 Core
This is the secure data analysis core of SAFRN.
Code here uses secure MultiParty Computation to compute the following analyses, with more on their way.

 - Linear Regression
   - Ordinary Least Square (OLS)
   - Fit Intercept (Optional, by query)
   - Per-coefficient standard error
   - root MSE
   - R-Square
   - F-statistic's p-value (F-statistic intentionally withheld)
   - Per-coefficient t-statistic's p-value (t-statistic intentionally withheld)
 - Low order moments
   - count
   - mean
   - variance
   - skew

Please see the [design document](../doc/wiki/design.md) and other documents from the SAFRN Wiki.

## Quick Start Guide

 1. Ensure all [dependencies](#dependencies) are installed. For library dependencies the ``-devel`` packages are also necessary.
 2. Unzip the SAFRN package (``tar -xzf safrn.tar.gz``)
 3. Change into the ``safrn/core`` directory (``cd safrn/core``)
 4. Build SAFRN (``make build``)
    - Optional: run the unit test suite (``make test``)
 5. Repeat steps 1-4 for as many test devices as you would like
    - Note: SAFRN can be configured to use multiple ports on a single host.
 6. Edit the ``peers.json`` file to reflect the IP and Port numbers of each test device.
 7. Run SAFRN on each test device.
    - For a single host/multiple ports configuration, open multiple terminals on the same host.

For a 2 (dataowner) party test run the following four commands on their own terminal/device.

```
target/safrnffnet --orgid 000000000000000000000000000A11CE --port 8000 --study server/src/test/data/study2.json --lookups server/src/test/data/ --data server/src/test/data/alice2.csv --query server/src/test/data/regression_no_intercept.json
```

```
target/safrnffnet --orgid 00000000000000000000000000000B0B --port 8001 --study server/src/test/data/study2.json --lookups server/src/test/data/ --data server/src/test/data/bob2.csv --query server/src/test/data/regression_no_intercept.json
```

```
target/safrnffnet --orgid 00000000000000000000000000FFFFFF --port 8003 --study server/src/test/data/study2.json --query server/src/test/data/regression_no_intercept.json
```

```
time target/safrnffnet --orgid 0000000000000000000000000000DEA1 --port 8002 --study server/src/test/data/study2.json --query server/src/test/data/regression_no_intercept.json
```

## Using SAFRN

After SAFRN is [built](#building), it is controlled by its study configuration, peer configuration, and query.

 - [**Study Configuration**](../doc/wiki/json-schemas/study-config.md) Defines the dataset of a study, and the participants. Upcoming, it will defined limitations on queries asked of a study.
 - [**Peer Configuration**](../doc/wiki/json-schemas/peer-config.md) describes connection info for each participant of a study.
 - [**Query**](../doc/wiki/json-schemas/query.md) Asks a study to perform a certain analysis.

 ```
 USAGE:
  safrnffnet --orgid {ORG ID} --port {portnum} [ --role {ROLE} ] [ --study {study.json} ] [ --peers {peers.json} ] [ --query {query.json} ] [ -- data {data.csv} ] [ --lookups {lookupsdir/} ]

OPTIONS:
--orgid        (required) Organization ID component of identity.
          {128 bit hex-encoded number with leading zeros}
--port         (required) This participant's server port number.
          {TCP/IP Port Number}
--role         (if ambiguous) The role component of identity.
          dataowner|dealer|recipient
--study        (default "./study.json") study config path.
--peers        (default "./peers.json") peers config path.
--data         (default "./data.csv") dataowner's CSV data file.
--lookups      (default "./lookups/") dataowner's lookup table files.
--query        (default "./query.json") the query definition file.
--help         prints the help text.
```

### Sample Configurations and Data

Small samples for 2 and 4 party studies are available in ``safrn/core/server/src/main/data/``.
A template for a 2 party peer configuration is available in this directory (``safrn/core/peers.json``).

## Building
To build SAFRN Core at the moment use the ``Makefile`` and run the default target.

```sh
make
```

This will build dependencies (once), and then build the safrn binaries ~~``target/safrn-server`` and ``target/safrn-client``~~ ``target/safrnffnet``.
~~``target/safrn-server`` is the SAFRN server executable which performs MPC to answer the query.
``target/safrn-client`` is the _Analyst_ client which sends queries to the servers.~~
``target/safrnffnet`` is a version of SAFRN with a feature-reduced Network Stack, all currently available analyses are supported.

The following targets are intended for "human consumption"
 - ``make dependencies``: Builds the project dependencies in the ``lib/`` folder.
 - ``make configure``: Runs CMake to configure the build system. This will automatically invoke ``make dependencies`` if necessary. The following variables can be set
   - ``CXX=[g++, clang++, ...]``: change the compiler vendor. The given compiler must support C++11. The default compiler is ``g++``.
   - ``BUILD_TYPE=[Release,Debug,Test,Ci]``: Controls some of the compiler flag settings.
     - ``Release``: A build intended to be packaged for consumers.
     - ``Debug``: A build intended for developers using a debugger, it enables debug symbols.
     - ``Test`` (default): A build intended for developer use without debug symbols.
     - ``Ci``: A build intended to be ran during continuous integration. The primary difference between ``Test`` and ``Ci`` is the use of ``-Werror`` to fail a build when a compiler warning is encountered (these are usually noted, but do not fail a build).
 - ``make build``: compiles sources and generates executables. (automatically invokes ``configure`` when necessary).
 - ``make test`` (default): runs the unit test suite (automatically invokes ``build`` when necessary).
   - ``TEST_FILTER=<testgroup>.<testname>`` can filter which tests are run.
 - ``make clean``: deletes compiled output files, causing them to be rebuilt on the next build.
 - ``make mopclean``: deletes all build system configuration files along with compiled output files.
 - ``make mrclean``: deletes all SAFRN build files (compiled output, and configuration) as well as all dependencies.
 - ``make format``: runs ``clang-format`` to format the source code.
 - ``make debug-server-test`` or ``make debug-shared-test``: This is a convenience to run the test suite under the debugger (specifically ``gdb``). This is interactive, through ``gdb``. It works best when ``BUILD_TYPE=Debug``. It can use the following variable:
   - ``TEST_FILTER=<testgroup>.<testname>`` can filter which tests being run under the debugger.

## Dependencies

### Stealth Dependencies
SAFRN depends on the following Stealth internal projects.

 - [Fortissimo](https://github.com/stealthsoftwareinc/fortissimo) a scheduler/framework for "fronctocols" (function-protocols).
 - [SST](https://github.com/stealthsoftwareinc/sst) A library of Stealth's commonly repeated code.

For internal builds, these are downloaded automatically.
For external release zips, they are packaged into the zip, no action is required from users.

### Embedded Dependencies
SAFRN depends on the following external projects, which are not expected to be provided by the system.

 - [Google Test](https://github.com/google/googletest) A unit test framework.

For internal builds, these are downloaded automatically.
For external release zips, they are packaged into the zip, no action is required from users.

### System Dependencies
SAFRN depends on the following packages, expected to be provided by the system.
The user must install these, and appropriate header packages (``-devel`` or ``-dev``).


| Dependency         | Min Tested  | Reason                     | Headers  |
|--------------------|-------------|----------------------------|----------|
| nlohmann JSON      | 2.1.1, 3.0+ | SAFRN Config/Query files   |   yes    |
| openssl            | 1.1.1       | ``openssl/bn.h``; TLS      |   yes    |
| libevent           | 2.1.8       | SAFRN Networking           |   yes    |
| nettle             | 3.4         | test data generation       |   yes    |
| GNU Scientific Lib | 2.4         | test data generation       |   yes    |
| C, C++ compiler    | C, C++ 2011 |                            | libc/c++ |
|  - gcc, g++        | 7.5.0       |                            | libc/c++ |
|  - clang, clang++  | 6.0.0       | Tested by non-apple clang  | libc/c++ |
| make               | GNU 4.1     | task orchestration         |    no    |
| cmake              | 3.10.2      | project wide code building |    no    |
| wget               |             | project wide dependencies  |    no    |
| m4                 | 1.4.18      | SST Build                  |    no    |
| libtool            | 2.4.6       | SST Build                  |    no    |
| automake           | 1.15.1      | SST Build                  |    no    |
| autoconf           | 2.69        | SST Build                  |    no    |
| jq                 | 1.5         | SST Build                  |    no    |
| texinfo            | 6.5         | SST Build                  |    no    |

Quick-install dependencies for CentOS 8:

```
yum -y install epel-release yum-utils
yum config-manager --set-enabled PowerTools
yum -y install diffutils which make cmake jq autoconf openssl openssl-devel m4 libtool automake gcc gcc-c++ json-devel libevent libevent-devel nettle nettle-devel gsl gsl-devel wget texinfo
```

Quick-install dependencies for Ubuntu LTS 18.04:

```
apt update
apt install autoconf automake build-essential gcc g++ jq m4 cmake openssl libssl-dev libtool nlohmann-json-dev libevent-* nettle-dev nettle-bin libgsl23 libgsl-dev wget texinfo
```
