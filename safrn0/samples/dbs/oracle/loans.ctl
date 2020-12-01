OPTIONS (SKIP=1)
LOAD DATA
INFILE '/var/lib/oracle-files/MPC_Test5_1K_Loans.csv'
BADFILE '/home/oracle/loans.bad'
DISCARDFILE '/home/oracle/loans.dsc'
INSERT INTO TABLE loansTbl
REPLACE FIELDS TERMINATED BY ',' OPTIONALLY ENCLOSED BY '"'
TRAILING NULLCOLS
(school,caseID,max_loan)
