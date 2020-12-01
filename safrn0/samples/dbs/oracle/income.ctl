OPTIONS (SKIP=1)
LOAD DATA
INFILE '/var/lib/oracle-files/MPC_Test5_1K_Income.csv'
BADFILE '/home/oracle/data/income.bad'
DISCARDFILE '/home/oracle/data/income.dsc'
INSERT INTO TABLE incomeTbl
REPLACE FIELDS TERMINATED BY ',' OPTIONALLY ENCLOSED BY '"'
TRAILING NULLCOLS
(income2,income3,income10,caseID)
