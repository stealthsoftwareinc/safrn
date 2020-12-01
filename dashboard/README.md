# Working with Dashboard
Following instructions assume JDK 1.8 or above. Baseline for project structure (including maven wrapper) was generated from https://start.spring.io/

1.  Get the source

2. cd to dashboard (working code is in branch _dash_ if you got the code prior to merging with master)
```
cd safrn
git checkout dash
cd src/dashboard
```
Also, update the SMTP and Postgres credentials(following) in file _src/main/resources/application.properties_
```
spring.mail.username= 
spring.mail.password= 
spring.datasource.username=stealth
spring.datasource.password=password
```
3. Start spring boot
```
./run.sh
```
4. Goto any browser and run following commands
a) Admin/Stealth  
```
http://localhost:8090/admin/adminlogin
```
Use the following credentials for logging in as administrator
```
stealth
password
```
b) Organization
```
http://localhost:8090/
```
5. Back button is supported. Emailing is supported; on admin creating organization, orgnizations gets emailed with credentials 

6. You can run several actuator commands to get the default/full details about the endpoints  
I) Actuator:
a) Following for viewing several paths for actuator
```
http://localhost:8090/actuator
```
b) Following to see if spring container is up and running
```
http://localhost:8090/actuator/health
```
c) Following to see all the beans that were create during runtime
```
http://localhost:8090/actuator/beans
```

b) Swagger:
```
http://localhost:8090/swagger-ui.html
http://localhost:8090/swagger-resources
http://localhost:8090/v2/api-docs

```
7. shutdown the server (of step 4) by ctrl-c, when done with testing

REST end points are in the folder _src/dashboard/src/main/java/com/stealthsoftwareinc/commercial/safrn/dashboard/controller/_

 
