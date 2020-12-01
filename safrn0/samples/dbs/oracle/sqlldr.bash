#!/bin/bash
. /home/oracle/.bashrc
sqlldr userid=root/root control=/var/lib/oracle-files/oracle/group1.ctl log=/home/oracle/groupTbl1.log
sqlldr userid=root/root control=/var/lib/oracle-files/oracle/group2.ctl log=/home/oracle/groupTbl2.log
sqlldr userid=root/root control=/var/lib/oracle-files/oracle/group3.ctl log=/home/oracle/groupTbl3.log
sqlldr userid=root/root control=/var/lib/oracle-files/oracle/group4.ctl log=/home/oracle/groupTbl4.log
sqlldr userid=root/root control=/var/lib/oracle-files/oracle/income.ctl log=/home/oracle/income.log
sqlldr userid=root/root control=/var/lib/oracle-files/oracle/loans.ctl log=/home/oracle/loans.log

exit 0
