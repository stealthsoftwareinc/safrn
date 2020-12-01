FROM mysql
RUN printf '[mysqld]\nsecure_file_priv=\n' >>/etc/mysql/my.cnf
COPY MPC_Test5_1K_Income.csv /var/lib/mysql-files/
COPY safrnincome_1K.sql /docker-entrypoint-initdb.d/
ENV MYSQL_ROOT_PASSWORD=root
