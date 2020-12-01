DROP DATABASE IF EXISTS SAFRN2Group4;

CREATE DATABASE SAFRN2Group4;

CREATE TABLE SAFRN2Group4.GroupTbl (
  CaseID int NOT NULL PRIMARY KEY,
  Degree CHAR(12),
  School CHAR(12),
  Sex CHAR(12),
  Age int
);

CREATE TABLE SAFRN2Group4.Degree_domain (id int, Degree VARCHAR(20));
CREATE TABLE SAFRN2Group4.School_domain (id int, School VARCHAR(20));
CREATE TABLE SAFRN2Group4.Sex_domain (id int, Sex VARCHAR(20));
CREATE TABLE SAFRN2Group4.Age_domain (id int, Age VARCHAR(20));
INSERT INTO SAFRN2Group4.Degree_domain VALUES (1,'None'),(2,'Associate'),(3,'Bachelor'),(4,'Graduate'),(5,'Other'),(6,'"*"');
INSERT INTO SAFRN2Group4.School_domain VALUES (1,'Gryffindor'),(2,'Ravenclaw'),(3,'Hufflepuff'),(4,'Slytherin'),(5,'"*"');
INSERT INTO SAFRN2Group4.Sex_domain VALUES (1,'Male'),(2,'Female'),(3,'"*"');
INSERT INTO SAFRN2Group4.Age_domain VALUES (1,'18'),(2,'19'),(3,'20'),(4,'21'),(5,'22'),(6,'23'),(7,'24'),(8,'25'),(9,'26'),(10,'27'),(11,'28'),(12,'29'),(13,'"*"');


LOAD DATA INFILE
  '/var/lib/mysql-files/MPC_Test5_1K_School4.csv'
  INTO TABLE SAFRN2Group4.GroupTbl
  FIELDS TERMINATED BY ','
  OPTIONALLY ENCLOSED BY '"'
  LINES TERMINATED BY '\n'
  IGNORE 1 LINES
  (Degree, School, Sex, Age, CaseID, @dummy)
;

