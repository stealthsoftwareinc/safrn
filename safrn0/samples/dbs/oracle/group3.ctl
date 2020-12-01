OPTIONS (SKIP=1)
LOAD DATA
INFILE '/var/lib/oracle-files/MPC_Test5_1K_School3.csv'
BADFILE '/home/oracle/groupTbl3.bad'
DISCARDFILE '/home/oracle/groupTbl3.dsc'
INSERT INTO TABLE groupTbl3
REPLACE FIELDS TERMINATED BY ',' OPTIONALLY ENCLOSED BY '"'
TRAILING NULLCOLS
(degree,school,sex,age,caseID)
