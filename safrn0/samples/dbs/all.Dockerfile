FROM mysql
RUN printf '[mysqld]\nsecure_file_priv=\n' >>/etc/mysql/my.cnf
COPY MPC_Test5_1K_Income.csv /var/lib/mysql-files/
COPY MPC_Test5_1K_Loans.csv /var/lib/mysql-files/
COPY MPC_Test5_1K_School1.csv /var/lib/mysql-files/
COPY MPC_Test5_1K_School2.csv /var/lib/mysql-files/
COPY MPC_Test5_1K_School3.csv /var/lib/mysql-files/
COPY MPC_Test5_1K_School4.csv /var/lib/mysql-files/
COPY safrngroup1_1K.sql /docker-entrypoint-initdb.d/
COPY safrngroup2_1K.sql /docker-entrypoint-initdb.d/
COPY safrngroup3_1K.sql /docker-entrypoint-initdb.d/
COPY safrngroup4_1K.sql /docker-entrypoint-initdb.d/
COPY safrnincome_1K.sql /docker-entrypoint-initdb.d/
COPY safrnloans_1K.sql /docker-entrypoint-initdb.d/
ENV MYSQL_ROOT_PASSWORD=root
