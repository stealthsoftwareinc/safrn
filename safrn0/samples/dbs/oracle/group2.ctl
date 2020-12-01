OPTIONS (SKIP=1)
LOAD DATA
INFILE '/var/lib/oracle-files/MPC_Test5_1K_School2.csv'
BADFILE '/home/oracle/groupTbl2.bad'
DISCARDFILE '/home/oracle/groupTbl2.dsc'
INSERT INTO TABLE groupTbl2
REPLACE FIELDS TERMINATED BY ',' OPTIONALLY ENCLOSED BY '"'
TRAILING NULLCOLS
(degree,school,sex,age,caseID)
