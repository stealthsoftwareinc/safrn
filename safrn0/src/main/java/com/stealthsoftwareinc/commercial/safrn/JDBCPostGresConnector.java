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

public class JDBCPostGresConnector {
  public static void main(String[] args) {

    Connection conn = null;
    try {

      conn = DriverManager.getConnection(
          "jdbc:postgresql://localhost:5432/?user=root&password=root&ssl=true");
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
      stmt.execute("DROP SCHEMA IF EXISTS testschema CASCADE;");
      stmt.execute("CREATE SCHEMA IF NOT EXISTS testschema;");
      stmt.execute("SET search_path TO testschema;");
      stmt.execute("CREATE TABLE IF NOT EXISTS aliens( " +
                   " alienid int PRIMARY KEY, " +
                   " LastName varchar(255), " +
                   " FirstName varchar(255), " +
                   " HomePlanet varchar(255));");
      stmt.execute("INSERT INTO aliens (alienid, lastname, firstname, homeplanet) VALUES (23, 'Protoss', 'Tassadar', 'Aiur');");
      stmt.execute("INSERT INTO aliens (alienid, lastname, firstname, homeplanet) VALUES (120, 'Zerg', 'Zasz', 'Char');");
      rs = stmt.executeQuery("SELECT * FROM aliens;");
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
