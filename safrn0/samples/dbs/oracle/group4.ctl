OPTIONS (SKIP=1)
LOAD DATA
INFILE '/var/lib/oracle-files/MPC_Test5_1K_School4.csv'
BADFILE '/home/oracle/groupTbl4.bad'
DISCARDFILE '/home/oracle/groupTbl4.dsc'
INSERT INTO TABLE groupTbl4
REPLACE FIELDS TERMINATED BY ',' OPTIONALLY ENCLOSED BY '"'
TRAILING NULLCOLS
(degree,school,sex,age,caseID)
