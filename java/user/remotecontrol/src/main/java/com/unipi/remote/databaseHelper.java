import java.sql.*;

import org.eclipse.californium.core.CoapClient;

import com.mysql.cj.result.Row;

package com.unipi.remote;

public class databaseHelper {
    static private final String url = "jdbc:mysql://localhost:3306/iotdatabase";
    static private final String user = "root";
    static private final String password = "PASSWORD";

    public static Connection getConnection() {
        Connection connection = null;
        try {
            // Create a connection to the database
            connection = DriverManager.getConnection(url, user, password);
        } catch (SQLException e) {
            e.printStackTrace();
        }
        return connection;
    }
            
    int getDangerRows() {
        int rowCount = 0;
        try {
            // Create a connection to the database
            Connection connection = DriverManager.getConnection(url, user, password);

            // Create a statement
            Statement statement = connection.createStatement();

            // Execute the query
            String query = "SELECT COUNT(*) FROM actuator WHERE level_danger = '2'";
            ResultSet resultSet = statement.executeQuery(query);
            
            if (resultSet.next()) {
                rowCount = resultSet.getInt(1);
                System.out.println("Number of rows with level_danger = 2: " + rowCount);
            }

            resultSet.close();
            statement.close();
            connection.close();
        } catch (SQLException e) {
            e.printStackTrace();
        }
        return rowCount;
    }

    String setActuatorTemperatureThreshold(String actuator, int threshold) {
        try {
            Connection connection = DriverManager.getConnection(url, user, password);
            Statement statement = connection.createStatement();
            String query = "UPDATE actuator SET temperature_threshold = " + threshold + " WHERE name = '" + actuator + "'";
            statement.executeUpdate(query);
            statement.close();
            connection.close();

            //after updating the in db, propagate the value to the real actuator
            //get the ipv6

            String dbquery = "SELECT ipv6 FROM actuator WHERE name = '" + actuator + "' AND TYPE = 'actuator'";
            Connection dbconnection = DriverManager.getConnection(url, user, password);
            Statement dbstatement = dbconnection.createStatement();
            ResultSet dbresultSet = dbstatement.executeQuery(dbquery);
            if (dbresultSet.next()) {
                String ipv6 = dbresultSet.getString(1);
                return ipv6;
                //CoapClient client = new CoapClient("coap://[" + ipv6 + "]/" + actuator + "/temperature_threshold");

            }
        } catch (SQLException e) {
            e.printStackTrace();
        }
    }
}
