package com.stealthsoftwareinc.commercial.safrn;

import java.sql.Connection;
import java.sql.DatabaseMetaData;
import java.sql.DriverManager;
import java.sql.SQLException;
import java.sql.Statement;
import java.sql.ResultSet;
import java.sql.ResultSetMetaData;

public class JDBCOracleConnector {

  static String url = "jdbc:oracle:thin:" +
      "root/root" +
      "@" + "//localhost:1521" +
      "/" + "ORCLCDB.localdomain";
  static String createTable = "CREATE TABLE aliens( " +
      " alienid NUMBER(9) NOT NULL PRIMARY KEY, " +
      " LastName VARCHAR2(255), " +
      " FirstName VARCHAR2(255), " +
      " HomePlanet VARCHAR2(255))";
  static String InsertFirst = "INSERT INTO aliens (alienid, lastname, firstname, homeplanet)" +
      "VALUES (23, 'Protoss', 'Tassadar', 'Aiur')";
  static String InsertSecond = "INSERT INTO aliens (alienid, lastname, firstname, homeplanet)" +
      "VALUES (120, 'Zerg', 'Zasz', 'Char')";

  public static void main(String[] args) {

    try (Connection conn = DriverManager.getConnection(url);
         Statement stmt = conn.createStatement();) {

      stmt.executeUpdate(createTable);
      stmt.executeUpdate(InsertFirst);
      stmt.executeUpdate(InsertSecond);

      ResultSet rs = stmt.executeQuery("SELECT * FROM aliens");
      printRS(rs);
      stmt.executeUpdate("DROP TABLE aliens");
    }
    catch (SQLException ex) {
      // handle any errors
      System.out.println("SQLException: " + ex.getMessage());
      System.out.println("SQLState: " + ex.getSQLState());
      System.out.println("VendorError: " + ex.getErrorCode());
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
