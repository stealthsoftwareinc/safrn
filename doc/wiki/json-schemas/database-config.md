# Database Configuration File

## NOTE: 
This is not used/relevant for SAFRN v1.0: There are no "databases" for each data owner. Instead, we have a simplified model where each "database" is just a local .csv file (already appropriately "configured" according to schema).

## Introduction

This configuration file is for local use by a data owner's server to store connection information for its own database back-end that stores the data corresponding to one of the vertical schemas for a given Study.

## Structure

 - ``<<array<columnMap_t>>> columnMaps``
   - ``<<string>> localName``
   - ``<<SQLSMALLINT>> localSQLType``
   - ``<<integer>> verticalIndex``
     > Refers to verticalIndex from [study-config](/doc/wiki/json-schemas/study-config.md)
     > Index is redundant to the order of the items in columnMaps to improve
     > human-readability.
     > This index will be checked against the location in the array, and the configuration 
     > rejected if they do not match.

   - ``<<integer>> columnIndex``
     > Refers to columnIndex from [study-config](/doc/wiki/json-schemas/study-config.md)
     > Index is redundant to the order of the items in columnMaps to improve
     > human-readability.
     > This index will be checked against the location in the array, and the configuration
     > rejected if they do not match.
   
 - ``databaseSettings``

   - ``<<string>> driverName``
     
     > Same string as returned by odbcinst tool. <br>
> Examples: "PostgreSQL Unicode", "PostgreSQL ANSI", and "MariaDB ODBC Driver"
     
   - ``<<string>> userID``
   - ``<<string>> password``
  
   > Only allows "password" (plain-text) authentication method at this time.
     
   - ``<<array<string>>> additionalParameters``
     > Additional parameters are ODBC driver specific.
     > They generally consist of the following format for each item: $(NAME)=$(VALUE)
     > Additional parameters (such as "Trusted_Connection=Yes") are available that are not
     > shown in the below example, however they tend to be specific to the database.
   
     > Postgres Example:
     > * Server=127.0.0.1
  > * Port=5432
     > * Database=ODBCTest
   
     > Here are some references for more connections strings: <br>
     > * [SQL Server](https://www.connectionstrings.com/microsoft-sql-server-odbc-driver/)
     > * [Postgres](https://www.connectionstrings.com/postgresql-odbc-driver-psqlodbc/)
     > * [MySQL](https://www.connectionstrings.com/mysql/)
     > * [SQLite](https://www.connectionstrings.com/sqlite/)
     > * [OracleDB](https://www.connectionstrings.com/oracle/)

## Example

See ``database.json`` file included in repository.
