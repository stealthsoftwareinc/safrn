-- Create the six tables in OracleDB

CREATE TABLE GroupTbl1 (
  CaseID NUMBER(9) NOT NULL PRIMARY KEY,
  Degree VARCHAR2(12),
  School VARCHAR2(12),
     Sex VARCHAR2(12),
     Age NUMBER(3));

CREATE TABLE GroupTbl2 (
  CaseID NUMBER(9) NOT NULL PRIMARY KEY,
  Degree VARCHAR2(12),
  School VARCHAR2(12),
     Sex VARCHAR2(12),
     Age NUMBER(3));

CREATE TABLE GroupTbl3 (
  CaseID NUMBER(9) NOT NULL PRIMARY KEY,
  Degree VARCHAR2(12),
  School VARCHAR2(12),
     Sex VARCHAR2(12),
     Age NUMBER(3));

CREATE TABLE GroupTbl4 (
  CaseID NUMBER(9) NOT NULL PRIMARY KEY,
  Degree VARCHAR2(12),
  School VARCHAR2(12),
     Sex VARCHAR2(12),
     Age NUMBER(3));

CREATE TABLE IncomeTbl (
  CaseID NUMBER(9) NOT NULL PRIMARY KEY,
  Income2 NUMBER(9),
  Income3 NUMBER(9),
  Income10 NUMBER(9));

CREATE TABLE LoansTbl (
  CaseID NUMBER(9) NOT NULL,
  Max_loan NUMBER(9),
  School VARCHAR(12));

CREATE TABLE Degree_domain (
  id Number(2),
  Degree VARCHAR2(12)
);

INSERT INTO Degree_domain (id, Degree)
            WITH degrees AS (
              SELECT 1, 'None' FROM dual UNION ALL
              SELECT 2, 'Associate' FROM dual UNION ALL
              SELECT 3, 'Bachelor' FROM dual UNION ALL
              SELECT 4, 'Graduate' FROM dual UNION ALL
              SELECT 5, 'Other' FROM dual UNION ALL
              SELECT 6, '"*"' FROM dual)
SELECT * from degrees;

CREATE TABLE School_domain (
  id Number(2),
  School VARCHAR(12)
);

INSERT INTO School_domain (id, School)
            WITH schools AS (
              SELECT 1, 'Gryffindor' FROM dual UNION ALL
              SELECT 2, 'Ravenclaw' FROM dual UNION ALL
              SELECT 3, 'Hufflepuff' FROM dual UNION ALL
              SELECT 4, 'Slytherin' FROM dual UNION ALL
              SELECT 5, '"*"' FROM dual)
SELECT * from schools;

CREATE TABLE Sex_domain (
  Id Number(2),
  Sex VARCHAR2(12)
);

INSERT INTO Sex_domain (id, Sex)
            WITH sexes AS (
              SELECT 1, 'Male' FROM dual UNION ALL
              SELECT 2, 'Female' FROM dual UNION ALL
              SELECT 3, '"*"' FROM dual)
SELECT * from sexes;

CREATE TABLE Age_domain (
  Id Number(2),
  Age VARCHAR(12)
);

INSERT INTO Age_domain (id, Age)
            WITH ages AS (
              SELECT 1, '18' FROM dual UNION ALL
              SELECT 2, '19' FROM dual UNION ALL
              SELECT 3, '20' FROM dual UNION ALL
              SELECT 4, '21' FROM dual UNION ALL
              SELECT 5, '22' FROM dual UNION ALL
              SELECT 6, '23' FROM dual UNION ALL
              SELECT 7, '24' FROM dual UNION ALL
              SELECT 8, '25' FROM dual UNION ALL
              SELECT 9, '26' FROM dual UNION ALL
              SELECT 10, '27' FROM dual UNION ALL
              SELECT 11, '28' FROM dual UNION ALL
              SELECT 12, '29' FROM dual UNION ALL
              SELECT 13, '"*"' FROM dual)
SELECT * from ages;

EXIT;
