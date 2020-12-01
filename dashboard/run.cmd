REM TODO: USE DEV/STAGING/PROD PROFILES
call mvnw  clean install -DskipTests
call java -jar target\safrn-dashboard.jar --debug 

REM alternatively below command
REM mvnw spring-boot:run
