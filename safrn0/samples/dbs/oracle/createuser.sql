-- running this alter command drops the requirement that the user have
-- an underscore in their name.
ALTER SESSION SET "_ORACLE_SCRIPT"=true;

CREATE TABLESPACE
    users DATAFILE '/home/oracle/tbs_users.ora'
    SIZE 10M REUSE AUTOEXTEND ON NEXT 10M MAXSIZE 200M;
CREATE TEMPORARY TABLESPACE temp_01
    TEMPFILE '/home/oracle/tbs_temp_01.ora'
    SIZE 5M
    AUTOEXTEND ON;
CREATE USER root IDENTIFIED BY root
    DEFAULT TABLESPACE users
    TEMPORARY TABLESPACE temp_01
    QUOTA 20M ON users;
GRANT CONNECT TO root;
GRANT CREATE SESSION TO root;
GRANT resource TO root;

EXIT;
