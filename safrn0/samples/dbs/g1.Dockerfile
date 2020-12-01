FROM mysql
RUN printf '[mysqld]\nsecure_file_priv=\n' >>/etc/mysql/my.cnf
COPY MPC_Test5_1K_School1.csv /var/lib/mysql-files/
COPY safrngroup1_1K.sql /docker-entrypoint-initdb.d/
ENV MYSQL_ROOT_PASSWORD=root
