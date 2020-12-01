# Annoation based approach to DB Schema design 
Almost all the mobile apps and online applications interact with the databases. Building the first version of such applications requires nailing down the schema of the database and subsequently iterating this schema as we understand more about the use case of the application. But modifying schema also leads to modifying the source code surrounding the DB queries; hence this can get messy. Various technologies came up over the period to address such scenarios so that database design shall not come in the way of application design:-

- JDBC(1st gen): It abstracts the vendor specific idiosyncrasies and provides a uniform API. But SQL specific logic/queries will be spread all over the source code

- ORM(2nd gen): It moves SQL queries from source code to configuration files. But user has to still deal with designing the schema

- Spring JPA(3rd gen): Takes out SQL queries altogether from the source code, hence source code preserves OO look and feel. POJOs are mapped to database tables, by wiring POJOs with ‘annotations’ (which will be converted to SQL queries at execution time). JPA also provides basic libraries for creating/retrieving/updating/deleting (CRUD) the rows in each table.

SAFRN Dashboard uses the Spring JPA model. JSON payload from UI is converted into POJO (that also has annotations for wiring it to the database). Hence, the database schema of Dashboard evolves in parallel with the POJOs (and does not require a priori schema design). Following examples show the simplicity with this approach.

- INHERITANCE: Almost all the tables need to have auditing information (createDateTime and lastUpdateTime) in their schema, to keep track of the changes that were made to a specific row. In traditional schema design, 2 new columns will be added for each table. But in Dashboard, we have a separate class  “AbstractDBAudit” that holds these 2 fields and every POJO is inherited from this class (hence all tables in DB  have auditing information inherited in OO style) 
- ORM: Object Relational Mapping makes schema design less relevant. For example, an Organization in Dashboard is represented by POJO class "Organization".   Schema for a POJO is created based on how it is annotated. 
  - @Entity: Class shall be mapped to a table in DB
  - @Table: Name of the table in DB
  - @Id: Primary field in the table
  - @GeneratedValue: Monotonically increasing value is assigned to the field at the time of insertion
  - @Lob: Dashboard uses it to store user profile image as ‘Binary Large Object’
  - @JoinTable: Links 2 tables by creating an intermediate table with a foriegn key
- LINKING TABLES: Representing Session as a table is interesting because Session holds multiple lists of variable length (Verticals, Sessions, Columns).
  - @Embeddable: Peers is a smaller size list, hence it can be part of Session table. Because Peers is variable size list, we create a separate table which holds individual elements of Peers list and cross references Session with sessionId
  - @OneToMany: Verticals is also a small array, but it has to be represented as set (and not list) because a POJO can only have 1 list (by the limitation of Spring). Further, Vertical can not be @Embeddable because it holds Column POJO (again by the limitation of Spring). Hence Vertical is modelled as a separate class, with an intermediate table that holds the relationship between Session and Vertical
- Conclusions/Remarks
  - ORM provides several parameters for each annotation (ex: name of intermediate tables, size of char array fields). Dashboard uses default naming and ignores these parameters 
  - ORM also provides helper classes that allow one to operate on tables. Dashboard extensively uses them (and overrides some of the default methods)
  - Traditional ORM packages like Hibernate and MyBatis are popular. Spring JPA is a layer above Hibernate. On the upside this completely takes away schema design from bootstrapping an application; on the downside it sometimes gets hard to trace the source of the problem because annotation+ORM+reflection makes it hard to trace/debug until mastery is .
 