package com.unipi.remote;

import java.sql.*;
import java.util.ArrayList;
import java.util.List;

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

    String setActuatorTemperatureThreshold(String actuator) {
        String ipv6 = null;
        try {
            String dbquery = "SELECT address FROM ipv6_addresses WHERE name = '" + actuator + "' AND TYPE = 'actuator'";
            Connection dbconnection = DriverManager.getConnection(url, user, password);
            Statement dbstatement = dbconnection.createStatement();
            ResultSet dbresultSet = dbstatement.executeQuery(dbquery);
            if (dbresultSet.next()) {
                ipv6 = dbresultSet.getString(1);
                return ipv6;


            }
        } catch (SQLException e) {
            e.printStackTrace();
        }
        return ipv6;
    }
    
public class PairNameIp {
    String ip;
    String name;

    public PairNameIp() {
        this.ip = null;
        this.name = null;
    }

    public PairNameIp(String ip, String name) {
        this.ip = ip;
        this.name = name;
    }
}

    public List<PairNameIp> getIPs(String type) {
        List<PairNameIp> sensorIPs = new ArrayList<>();
        String querySQL = "SELECT address, name FROM ipv6_addresses WHERE type = '" + type + "'";

        try (Connection connection = DriverManager.getConnection(url, user, password);
             PreparedStatement pstmt = connection.prepareStatement(querySQL);
             ResultSet rs = pstmt.executeQuery()) {
            while (rs.next()) {
                sensorIPs.add(new PairNameIp(rs.getString("address"),rs.getString("name")));
            }
        } catch (SQLException e) {
            e.printStackTrace();
        }

        return sensorIPs;
    }
}
