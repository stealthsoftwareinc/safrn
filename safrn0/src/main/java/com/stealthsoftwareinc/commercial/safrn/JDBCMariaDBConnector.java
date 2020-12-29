/*
 * This file is from the SAFRN package.
 *
 * The following copyright notice is generally applicable:
 *
 *      Copyright (C)
 *         Stealth Software Technologies Commercial, Inc.
 *
 * The full copyright information depends on the distribution
 * of the package. For more information, see the COPYING file.
 * However, depending on the context in which you are viewing
 * this file, the COPYING file may not be available.
 */
package com.stealthsoftwareinc.commercial.safrn;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;
import java.sql.Statement;
import java.sql.ResultSet;
import java.sql.ResultSetMetaData;
//import java.com.mysql.jdbc.Driver;

// Notice, do not import com.mysql.jdbc.*
// or you will have problems!

public class JDBCMariaDBConnector {
  public static void main(String[] args) {
  	
  	/*
    try {
      // The newInstance() call is a work around for some
      // broken Java implementations

      Class.forName("com.mysql.jdbc.Driver").newInstance();
    } catch (Exception ex) {  	
      System.out.println("Exception:" + ex);
      // handle the error
    }
    */

    Connection conn = null;
    try {
      
      conn =
       DriverManager.getConnection("jdbc:mariadb://127.0.0.1:55556/?" +
                                   "user=root&password=my-secret-pw");

    // Do something with the Connection

   
    } catch (SQLException ex) {
      // handle any errors
      System.out.println("SQLException: " + ex.getMessage());
      System.out.println("SQLState: " + ex.getSQLState());
      System.out.println("VendorError: " + ex.getErrorCode());
    }
  

    // assume that conn is an already created JDBC connection (see previous examples)

    Statement stmt = null;
    ResultSet rs = null;

    try {
      stmt = conn.createStatement();
      stmt.execute("CREATE DATABASE IF NOT EXISTS MtestDB;");
      stmt.execute("USE MtestDB;");
      stmt.execute("CREATE TABLE IF NOT EXISTS Aliens(AlienID int, " + 
      	            "LastName varchar(255), " + 
      	            "FirstName varchar(255), " + 
      	            "HomePlanet varchar(255), " +
      	            "PRIMARY KEY (AlienID));");
      stmt.execute("INSERT INTO Aliens VALUES (23, 'Protoss', 'Tassadar', 'Aiur')" + 
      	"ON DUPLICATE KEY UPDATE AlienID = AlienID;");
      stmt.execute("INSERT INTO Aliens VALUES (120, 'Zerg', 'Zasz', 'Char')" + 
      	"ON DUPLICATE KEY UPDATE AlienID = AlienID;");
      rs = stmt.executeQuery("SELECT * FROM Aliens;");
      printRS(rs);

      //stmt.execute("CREATE TABLE ")
      
      // or alternatively, if you don't know ahead of time that
      // the query will be a SELECT...

      //if (stmt.execute("SELECT foo FROM bar")) {
      //  rs = stmt.getResultSet();
      //}

      
    }
    catch (SQLException ex){
      // handle any errors
      System.out.println("SQLException: " + ex.getMessage());
      System.out.println("SQLState: " + ex.getSQLState());
      System.out.println("VendorError: " + ex.getErrorCode());
    }
    finally {
      // it is a good idea to release
      // resources in a finally{} block
      // in reverse-order of their creation
      // if they are no-longer needed

      if (rs != null) {
        try {
          rs.close();
        } catch (SQLException sqlEx) { } // ignore
        rs = null;
      }

      if (stmt != null) {
        try {
          stmt.close();
        } catch (SQLException sqlEx) { } // ignore

      stmt = null;
      }
    }
  }

  public static void printRS(ResultSet rs) throws SQLException
  {
    ResultSetMetaData rsmd = rs.getMetaData();
    int numColumns = rsmd.getColumnCount();

    for(int i = 1; i <= numColumns;i++)
    {
      System.out.printf("%-20s",rsmd.getColumnLabel(i));
    }
    System.out.print("\n");

    while(rs.next())
    {
      for(int i = 1; i <= numColumns;i++)
      {
    	System.out.printf("%-20s",rs.getString(i));
      }
      System.out.print("\n");
    }
  }
}
