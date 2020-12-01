# Transition Document 

## 1.0 Existing SAFRN Dashboard documentation
 - [Background on DB](database.md)   
 - [Running Dashboard from the source](README.md)  
 - [HTML/UI directory structure](src/main/resources/readme.md)
 
 
## 2.0 Resources for Spring 
#### 2.1 Documentation
 - [JPA (database)](https://spring.io/projects/spring-data-jpa)   
 - [Boot (rest)](https://spring.io/projects/spring-boot)    
 - [Securirty (login)](https://spring.io/guides/gs/securing-web)    
 - [Spring (general)](https://www.baeldung.com)
 
#### 2.2 Samples (working examples on internet)
- https://github.com/eugenp
- https://github.com/spring-guides

#### 2.3 Few files/classes
- [Entry point for all rest endpoints](src/main/java/com/stealthsoftwareinc/commercial/safrn/dashboard/config/SecurityConfigurer.java)   
- [Rest endpoints (admin)](src/main/java/com/stealthsoftwareinc/commercial/safrn/dashboard/controller/AdminController.java)    
- [Rest endpoints (organization)](src/main/java/com/stealthsoftwareinc/commercial/safrn/dashboard/controller/OrganizationController.java)    
- [User DB schema representation](src/main/java/com/stealthsoftwareinc/commercial/safrn/dashboard/pojo/User.java)   
- [Exception/error handling](src/main/java/com/stealthsoftwareinc/commercial/safrn/dashboard/controller/DashboardErrorController.java)   
- [Properties text file](src/main/resources/application.properties)   
- [Properties java file](src/main/java/com/stealthsoftwareinc/commercial/safrn/dashboard/config/AppConfig.java)  




#### 3.0 Code changes
- To take out Swagger, remove/comment the file **com.stealthsoftwareinc.commercial.safrn.dashboard.config.SwaggerConfig**   and take out the compilation errors
- To start dashboard on local machine, start postgres (docker?) and make sure  schema `database` exists
- mvn compilation may throw mission interceptor jar. If so add following dependency to pom.xml
 ```
    <dependency>
        <groupId>javax.interceptor</groupId>
        <artifactId>javax.interceptor-api</artifactId>
        <version>1.2</version>
     </dependency>
 ```