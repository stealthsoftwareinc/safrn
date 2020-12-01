DROP DATABASE IF EXISTS SAFRN2Loans;

CREATE DATABASE SAFRN2Loans;

CREATE TABLE SAFRN2Loans.LoansTbl (
  CaseID int NOT NULL,
  Max_Loan int,
  School CHAR(12)
);

CREATE TABLE SAFRN2Loans.Degree_domain (id int, Degree VARCHAR(20));
CREATE TABLE SAFRN2Loans.School_domain (id int, School VARCHAR(20));
CREATE TABLE SAFRN2Loans.Sex_domain (id int, Sex VARCHAR(20));
CREATE TABLE SAFRN2Loans.Age_domain (id int, Age VARCHAR(20));
INSERT INTO SAFRN2Loans.Degree_domain VALUES (1,'None'),(2,'Associate'),(3,'Bachelor'),(4,'Graduate'),(5,'Other'),(6,'"*"');
INSERT INTO SAFRN2Loans.School_domain VALUES (1,'Gryffindor'),(2,'Ravenclaw'),(3,'Hufflepuff'),(4,'Slytherin'),(5,'"*"');
INSERT INTO SAFRN2Loans.Sex_domain VALUES (1,'Male'),(2,'Female'),(3,'"*"');
INSERT INTO SAFRN2Loans.Age_domain VALUES (1,'18'),(2,'19'),(3,'20'),(4,'21'),(5,'22'),(6,'23'),(7,'24'),(8,'25'),(9,'26'),(10,'27'),(11,'28'),(12,'29'),(13,'"*"');


LOAD DATA INFILE
  '/var/lib/mysql-files/MPC_Test5_1K_Loans.csv'
  INTO TABLE SAFRN2Loans.LoansTbl
  FIELDS TERMINATED BY ','
  OPTIONALLY ENCLOSED BY '"'
  LINES TERMINATED BY '\n'
  IGNORE 1 LINES
  (School, CaseID, Max_Loan)
;
