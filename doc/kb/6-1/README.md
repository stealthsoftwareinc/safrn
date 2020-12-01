# Working with Oracle Database

## How to get the Docker image

1. Create an account at [Docker Hub](https://hub.docker.com/) if you
   don't already have one.

2. Go to <https://hub.docker.com/_/oracle-database-enterprise-edition>,
   click the **Proceed to Checkout** button, fill in your contact
   information, and click **Get Content**.

3. Run `docker login` and log in with your Docker Hub credentials.

4. Run `docker pull store/oracle/database-enterprise:12.2.0.1`.

You can get back to the page that step 2 takes you to by logging in to
Docker Hub, going to **Username > My Content** in the top right menu,
and clicking the **Setup** button under
**Oracle Database Enterprise Edition**.
If you accidentally repeat step 2 multiple times, you'll get multiple
entries under **Oracle Database Enterprise Edition**, which seems to be
inconsequential.

## How to run a Docker container

Adjust and run the following command as needed:

```
(
  x=`pwd` || exit $?
  docker run -d -i -t \
    -p 1521:1521 \
    --name oracle \
    -v "$x"/data:/home/oracle/data \
    store/oracle/database-enterprise:12.2.0.1 \
  || exit $?;
)
```

Note that the `-v` option maps the `data` directory of the current
directory into the container as `/home/oracle/data`.
This is where you should put any files that you want to be able to
access from within the container.

Make sure to wait until `docker ps` says the container is `healthy`
before trying to do anything with it.
You can programmatically wait for the container to become `healthy`
using the following blurb of shell script:

```
(
  while :; do
    x=`docker ps -f name=oracle --format "{{.Status}}"` || exit $?
    case $x in *healthy*) break ;; esac
    sleep 1 || exit $?
  done
)
```

## How to open a shell in the Docker container

To open a regular container shell, run `docker exec -i -t oracle bash`.
This is the kind of shell you should use for general operations.

To open a root container shell, run
`docker exec -i -t -u root oracle bash`.
You'll need this kind of shell for special operations, like copying
files into a mounted directory or installing system packages with `yum`.

## How to export the schema of a database dump

Assuming you have `foo.dmp` in your `~/data` directory, run the
following commands in a regular container shell:

```
cd ~
mkdir foo
sqlplus 'sys/Oradoc_db1 as sysdba' <<<"create directory foo as '/home/oracle/foo';"
cp data/foo.dmp foo
impdp userid='"sys/Oradoc_db1 as sysdba"' directory=foo dumpfile=foo.dmp sqlfile=foo.sql
```

The last line of output should say something like
`Job <name> successfully completed at <date>`.
You should now have `foo.sql` in your `~/data` directory.

## How to import a database dump

First, follow the instructions in the previous section to export the
schema of your database dump.

Next, look through the schema for any `CREATE USER` statements and copy
these statements to another file, say `setup.sql`.
We'll use this file to create the necessary users ahead of time, as the
upcoming import command will be unable to create users whose names don't
begin with `COMMON_USER_PREFIX`, which defaults to `C##` in a container
database (CDB).

Next, put the following statement at the very top of `setup.sql`:

```
ALTER SESSION SET "_ORACLE_SCRIPT"=true;
```

This is an undocumented setting that temporarily disables the
`COMMON_USER_PREFIX` requirement.

Next, look through the `CREATE USER` statements, and for each one that
mentions a `DEFAULT TABLESPACE "bar"`, add the following statement just
above the `CREATE USER` statement:

```
CREATE TABLESPACE "bar"
  DATAFILE '/home/oracle/bar.ora'
  SIZE 10M AUTOEXTEND ON
;
```

Next, run the following command to execute `setup.sql`:

```
sqlplus 'sys/Oradoc_db1 as sysdba' <setup.sql
```

Finally, run the following command to import the dump:

```
impdp \
  USERID='"sys/Oradoc_db1 as sysdba"' \
  DIRECTORY=foo \
  DUMPFILE=foo.dmp \
  FULL=Y \
  >stdout 2>stderr \
;
```

If you inspect the `stdout` file afterward, you'll probably find that
it's empty, as `impdp` generally seems to not print anything to standard
output.
If you inspect the `stderr` file, you should see a summary of what was
imported, and you should only see errors for the `CREATE USER`
statements.
These statements are expected to fail because they might violate the
`COMMON_USER_PREFIX` requirement and because we already created the
users ahead of time.
No other statements should fail.
