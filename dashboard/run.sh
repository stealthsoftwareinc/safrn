#TODO: USE DEV/STAGING/PROD PROFILES


./mvnw  clean install -DskipTests
java -jar target/safrn-dashboard.jar --debug 

#alternatively below command
#./mvnw spring-boot:run

