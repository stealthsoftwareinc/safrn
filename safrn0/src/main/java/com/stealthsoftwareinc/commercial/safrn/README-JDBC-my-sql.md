# MySQL and JDBC

There are two choices when connecting to a mysql server via JDBC: Either use the latest version of mysql and JDBC (version `8.0.17` as of this writing), or one of the versions `5.*.*`.

The versions of mysql and JDBC should match, or there are compatibility issues with mysql's choice of authentication method.

The current version of SAFRN uses version `5.*.*`.

## Set up a mysql server with Docker

We set up a mysql server on port 55555 directly from docker to avoid the hassle of installing and setting up mysql.

To get the docker mysql image:

`docker pull mysql`

To run version `5.*.*` (version `5.7.27`, I believe):

`sudo docker run --name=mysql-test-server --publish=127.0.0.1:55555:3306/tcp -e MYSQL_ROOT_PASSWORD=my-secret-pw -d mysql:5` 

To run version `8.0.17` of mysql:

`sudo docker run --name=mysql-test-server --publish=127.0.0.1:55555:3306/tcp -e MYSQL_ROOT_PASSWORD=my-secret-pw -d mysql:latest` 

## Connecting to mysql with Java JDBC

To download the JDBC drivers for version `5.*.*`:

`sudo apt install libmysql-java` will update the drivers. Then include the address on the classpath:

`export CLASSPATH = /usr/local/share/java/*:/usr/share/java/*`

To download the JDBC drivers for version `8.*.*`:

First run `sudo apt install libmysql-java`.

Then download the current version of `mysql-connector-java-*.jar` [here](https://dev.mysql.com/downloads/connector/j/). It should autoinstall into the same directory as the other `mysql.jar` files. Update classpath:

`export CLASSPATH = /usr/local/share/java/*:/usr/share/java/*` 

After set up, you can compile and run the file `JDBCConnector.java`, which will connect to 127.0.0.1 port 55555 username=root password=my-secret-pw. Currently, `JDBCConnector.java` creates a database called `testDB`, a table called `Persons`, and adds two entries to that table.

## Connecting with mysql client

You can also use a mysql client to connect to the mysql server.

With mysql installed, run 

`mysql --host=127.0.0.1 --port=55555 -uroot -pmy-secret-pw`

To connect with docker, run:

`sudo docker run -it --rm --network=host mysql mysql --host=127.0.0.1 --port=55555 -uroot -pmy-secret-pw`

This works for version 5 and version 8.

# MariaDB and JDBC

The steps for MariaDB are similar to the steps for MySQL.

## Set up a MariaDB server with Docker:

Get the docker image:

`docker pull mariadb`

Run a mariadb server on port 55556 (or 55555 if that port is not already in use by mysql-server).

`docker run --name mariadb-test-server --publish=127.0.0.1:55556:3306/tcp -e MYSQL_ROOT_PASSWORD=my-secret-pw -d mariadb:latest`

## Connecting to MariaDB with Java JDBC

Two options: (1) Download the `.jar` file directly [here](https://mariadb.com/downloads/#connectors), save it somewhere, and add its location to your CLASSPATH.

(2) Or if you work with a package manager like Maven, follow these [instructions](https://mariadb.com/kb/en/library/installing-mariadb-connectorj/) to download and install the Connector/J for MariaDB. 

Then compile and run the file `JDBCMariaDBConnector.java`, which creates a database called `MtestDB`, a table called `Aliens`, and adds two entries to that table.

## Connecting with MariaDB client

To connect with docker, run the command:

`sudo docker run -it --rm --network=host mariadb mysql -h127.0.0.1 --port=55556 -uroot -pmy-secret-pw`

## Note: Compatibility of MariaDB and MySQL

Possible conflicts can emerge when using MariaDB and MySQL on the same machine. This is one of the reasons to dockerize the MariaDB server and client.

For example, `getConnection("jdbc:mysql:*")` can be ambiguous if both drivers' locations are on the [classpath](https://mariadb.com/kb/en/library/about-mariadb-connector-j/#having-mariadb-and-mysql-drivers-in-the-same-classpath). On my machine, `JDBCConector.java` worked correctly for me before and after adding `&disableMariaDbDriver` to `getConnection("jdbc:mysql:*")`.
