package com.unipi.dii.iot;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.List;

import org.json.simple.JSONArray;

public class IPv6DatabaseManager {
    static final String JDBC_URL = "jdbc:mysql://localhost:3306/iotdatabase";
    static final String JDBC_USER = "root";
    static final String JDBC_PASSWORD = "PASSWORD";

    public static void createDatabase() {
        String JDBC_URL1 = "jdbc:mysql://localhost:3306/";
        String JDBC_USER1 = "root";
        String JDBC_PASSWORD1 = "PASSWORD";
        String DATABASE_NAME1 = "iotdatabase";
    
        String createDatabaseSQL = "CREATE DATABASE IF NOT EXISTS " + DATABASE_NAME1;
    
        try (Connection conn = DriverManager.getConnection(JDBC_URL1, JDBC_USER1, JDBC_PASSWORD1);
             Statement stmt = conn.createStatement()) {
            stmt.execute(createDatabaseSQL);
            System.out.println("Database created successfully.");
        } catch (SQLException e) {
            System.out.println("Database not created.");
            e.printStackTrace();
        }
    }
    
    static Connection connect() throws SQLException {
        return DriverManager.getConnection(JDBC_URL, JDBC_USER, JDBC_PASSWORD);
    }

    public void createTableIPV6() {
        String createTableSQL = "CREATE TABLE IF NOT EXISTS ipv6_addresses (" +
                                "id INT AUTO_INCREMENT PRIMARY KEY, " +
                                "address VARCHAR(39) NOT NULL, " +
                                "type VARCHAR(50) NOT NULL)";

        try (Connection conn = connect();
             Statement stmt = conn.createStatement()) {
            stmt.execute(createTableSQL);
            System.out.println("Table created successfully.");
        } catch (SQLException e) {
            e.printStackTrace();
        }
    }

    public void insertIPv6Address(String address, String type) {
        createTableIPV6();
        
        String insertSQL = "INSERT INTO ipv6_addresses (address, type) VALUES (?, ?)";

        try (Connection conn = connect();
             PreparedStatement pstmt = conn.prepareStatement(insertSQL)) {
            pstmt.setString(1, address);
            pstmt.setString(2, type);
            pstmt.executeUpdate();
            System.out.println("IPv6 address inserted successfully.");
        } catch (SQLException e) {
            e.printStackTrace();
        }
    }

    public List<String> fetchIPv6Addresses() {
        List<String> ipAddresses = new ArrayList<>();
        String querySQL = "SELECT address FROM ipv6_addresses";

        try (Connection conn = connect();
             Statement stmt = conn.createStatement();
             ResultSet rs = stmt.executeQuery(querySQL)) {

            while (rs.next()) {
                String address = rs.getString("address");
                ipAddresses.add(address);
            }
        } catch (SQLException e) {
            e.printStackTrace();
        }

        return ipAddresses;
    }

    public List<String> getSensorIPs() {
        List<String> sensorIPs = new ArrayList<>();
        String querySQL = "SELECT address FROM ipv6_addresses WHERE type = 'sensor'";

        try (Connection conn = connect();
             PreparedStatement pstmt = conn.prepareStatement(querySQL);
             ResultSet rs = pstmt.executeQuery()) {
            while (rs.next()) {
                sensorIPs.add(rs.getString("address"));
            }
        } catch (SQLException e) {
            e.printStackTrace();
        }

        return sensorIPs;
    }

    public static void createTableActuator(){
        String createTableSQL = "CREATE TABLE IF NOT EXISTS actuators ("
                + "ipAddress VARCHAR(50) NOT NULL, "
                + "actuatorType VARCHAR(50) NOT NULL, "
                + "threshold FLOAT NOT NULL, "
                + "state VARCHAR(10) NOT NULL CHECK (stato IN ('active', 'off')), "
                + "PRIMARY KEY (ipAddress, actuatorType))";

        try (Connection conn = connect();
             Statement stmt = conn.createStatement()) {
            stmt.execute(createTableSQL);
            System.out.println("Table created successfully.");
        } catch (SQLException e) {
            e.printStackTrace();
        }
    }


    public static void createTableSensor(String sensorName, String ip, JSONArray ss, int value) {
        System.out.println("Creating table sensors");
        ip = ip.replace(":", "");

        String tableName = sensorName + "_" + ip;

        String createTableColumns = "";
        for (int i = 0; i < ss.size(); i++) {
            createTableColumns += ss.get(i).toString() + " FLOAT NOT NULL, ";
        }
        createTableColumns += "value INT NOT NULL";

        String createTableSQL = "CREATE TABLE IF NOT EXISTS " + tableName + " ("
                + "sensorName VARCHAR(50) NOT NULL, "
                + "ipAddress VARCHAR(50) NOT NULL, "
                + createTableColumns + ", "
                + "PRIMARY KEY (sensorName, ipAddress))";

        try (Connection conn = connect();
             Statement stmt = conn.createStatement()) {
            stmt.execute(createTableSQL);
            System.out.println("Table created successfully.");
        } catch (SQLException e) {
            e.printStackTrace();
        }
    }

    public void insertSensorTHERMOMETER(String sensorName, String ip, JSONArray ss,int value) {
        // tipo sensore | valore predetto | valore |
        ip = ip.replace(":", "");
        String tableName = sensorName + "_" + ip;
        String insertSQL = "INSERT INTO " + tableName + " (sensorName, temperature, humidity, value) VALUES (?, ?, ?, ?)";

        try (Connection conn = connect();
            PreparedStatement pstmt = conn.prepareStatement(insertSQL)) {
            pstmt.setString(1, sensorName);
            pstmt.setFloat(2, value);
            pstmt.setFloat(3, value);
            pstmt.setFloat(4, value);
            pstmt.setInt(4, value);
            pstmt.executeUpdate();
            System.out.println("Sensor inserted successfully.");
        } catch (SQLException e) {
            e.printStackTrace();
        }
    }

    public void insertSensorLPG(String sensorName, String ip, JSONArray ss,int value) {
        // tipo sensore | valore predetto | valore |
        String tableName = sensorName + "_" + ip;
        String insertSQL = "INSERT INTO " + tableName + " (sensorName, ts, co, humidity, light, motion, smoke, value) VALUES (?, ?, ?, ?, ?, ?, ?, ?)";

        try (Connection conn = connect();
            PreparedStatement pstmt = conn.prepareStatement(insertSQL)) {
            pstmt.setString(1, sensorName);
            /*pstmt.setFL(2, ss.get(0).toString());
            pstmt.setString(3, ss.get(1).toString()); DA METTERE I VALORI INTERI
            pstmt.setString(4, ss.get(2).toString());
            pstmt.setString(5, ss.get(3).toString());
            pstmt.setString(6, ss.get(4).toString());
            pstmt.setString(7, ss.get(5).toString());*/
            pstmt.setInt(8, value);
            pstmt.executeUpdate();
            System.out.println("Sensor inserted successfully.");
        } catch (SQLException e) {
            e.printStackTrace();
        }
    }

    public void insertActuator() {
        // indirizzi ip | tipo attuatore | cosa fa | threshold | in funzione/ non attiva |
        throw new UnsupportedOperationException("Unimplemented method 'insertActuator'");
    }

    /*
    public static void main(String[] args) {
        IPv6DatabaseManager dbManager = new IPv6DatabaseManager();
        dbManager.createTable();

        // Inserisci qualche esempio di indirizzo IPv6
        dbManager.insertIPv6Address("2001:0db8:85a3:0000:0000:8a2e:0370:7334", "ExampleType1");
        dbManager.insertIPv6Address("2001:0db8:85a3:0000:0000:8a2e:0370:7335", "ExampleType2");

        // Recupera e stampa gli indirizzi IPv6
        dbManager.fetchIPv6Addresses();
    }
    */
}
