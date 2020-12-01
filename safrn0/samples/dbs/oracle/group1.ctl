OPTIONS (SKIP=1)
LOAD DATA
INFILE '/var/lib/oracle-files/MPC_Test5_1K_School1.csv'
BADFILE '/home/oracle/groupTbl1.bad'
DISCARDFILE '/home/oracle/groupTbl1.dsc'
INSERT INTO TABLE groupTbl1
REPLACE FIELDS TERMINATED BY ',' OPTIONALLY ENCLOSED BY '"'
TRAILING NULLCOLS
(degree,school,sex,age,caseID)
