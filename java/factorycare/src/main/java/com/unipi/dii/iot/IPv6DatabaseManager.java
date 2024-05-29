package com.unipi.dii.iot;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.sql.Timestamp;
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

    public static void createTableIPV6() {
        String createTableSQL = "CREATE TABLE IF NOT EXISTS ipv6_addresses (" +
                                "id INT AUTO_INCREMENT PRIMARY KEY, " +
                                "address VARCHAR(89) NOT NULL, " +
                                "type VARCHAR(80) NOT NULL," + 
                                "name VARCHAR(80) NOT NULL)";

        try (Connection conn = connect();
             Statement stmt = conn.createStatement()) {
            stmt.execute(createTableSQL);
            System.out.println("Table created successfully.");
        } catch (SQLException e) {
            e.printStackTrace();
        }
    }

    public void insertIPv6Address(String address, String type, String name) {
        createTableIPV6();
        
        String insertSQL = "INSERT INTO ipv6_addresses (address, type, name) VALUES (?, ?, ?)";

        try (Connection conn = connect();
             PreparedStatement pstmt = conn.prepareStatement(insertSQL)) {
            pstmt.setString(1, address);
            pstmt.setString(2, type);
            pstmt.setString(3, name);
            pstmt.executeUpdate();
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
//utility class to store the name and ip of the sensor
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

        try (Connection conn = connect();
             PreparedStatement pstmt = conn.prepareStatement(querySQL);
             ResultSet rs = pstmt.executeQuery()) {
            while (rs.next()) {
                sensorIPs.add(new PairNameIp(rs.getString("address"),rs.getString("name")));
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
                + "state VARCHAR(10) NOT NULL, "
                + "PRIMARY KEY (ipAddress, actuatorType))";

        try (Connection conn = connect();
             Statement stmt = conn.createStatement()) {
            stmt.execute(createTableSQL);
        } catch (SQLException e) {
            e.printStackTrace();
        }
    }

    public static void deleteDB() {
        String deleteDBSQL = "DROP DATABASE IF EXISTS iotdatabase";

        try (Connection conn = DriverManager.getConnection(JDBC_URL, JDBC_USER, JDBC_PASSWORD);
             Statement stmt = conn.createStatement()) {
            stmt.execute(deleteDBSQL);
            System.out.println("Database deleted successfully.");
        } catch (SQLException e) {
            e.printStackTrace();
        }
    }
    public static void createTableSensor(String sensorName, String ip, JSONArray ss) {
        ip = ip.replace(":", "");

        String tableName = sensorName.toLowerCase() + "_" + ip;
        System.out.println("tableName: " + tableName);   
        String createTableColumns = "";
        for (int i = 0; i < ss.size(); i++) {
            System.out.println("VALORI COLONNA: " + ss.get(i).toString());
            if(!ss.get(i).toString().equals("value") && !ss.get(i).toString().equals("ts")){
                System.out.println("VALORI COLONNA CREATI: " + ss.get(i).toString());
                createTableColumns += ss.get(i).toString() + " LONG NOT NULL, ";
            }
        }
        String createTableSQL = "CREATE TABLE IF NOT EXISTS " + tableName + " ("
            + "id INT AUTO_INCREMENT, "
            + "sensorName VARCHAR(50) NOT NULL, "
            + createTableColumns 
            + "PRIMARY KEY (id)) ";

        try (Connection conn = connect();
             Statement stmt = conn.createStatement()) {
            stmt.execute(createTableSQL);
            System.out.println("Table created successfully.");
        } catch (SQLException e) {
            e.printStackTrace();
        }
    }

    public void insertSensorTHERMOMETER(String sensorName, String ip, JSONArray ss) {
        //checking if table exists, if not we create it
        JSONArray valuesArray = new JSONArray();
        valuesArray.add("temperature");
        valuesArray.add("humidity");
        sensorName = sensorName.toLowerCase();
        createTableSensor(sensorName, ip, valuesArray);

        ip = ip.replace(":", "");
        String tableName = sensorName + "_" + ip;
        String insertSQL = "INSERT INTO " + tableName + " (sensorName, temperature, humidity) VALUES (?, ?, ?)";

        try (Connection conn = connect();
            PreparedStatement pstmt = conn.prepareStatement(insertSQL)) {
            pstmt.setString(1, sensorName);
            pstmt.setLong(2, (Long)ss.get(0));
            pstmt.setLong(3, (Long)ss.get(1));       
            pstmt.executeUpdate();
            System.out.println("Sensor inserted successfully.");
        } catch (SQLException e) {
            e.printStackTrace();
        }
    }

    public void insertSensorLPG(String sensorName, String ip, JSONArray ss,Long value) {
        //checking if table exists, if not we create it
        JSONArray valuesArray = new JSONArray();
        valuesArray.add("co");
        valuesArray.add("light");
        valuesArray.add("smoke");
        String sensorNameTable = sensorName.toLowerCase();
        createTableSensor(sensorNameTable, ip, valuesArray);

        ip = ip.replace(":", "");
        String tableName = sensorNameTable + "_" + ip;
        String insertSQL = "INSERT INTO " + tableName + " (sensorName, co, light, smoke) VALUES (?, ?, ?, ?)";

        //converting the boolean value to Long
        Long light = 0L;
        if((Boolean)ss.get(2)){
            light=1L;
        }

        try (Connection conn = connect();
            PreparedStatement pstmt = conn.prepareStatement(insertSQL)) {
            pstmt.setString(1, sensorNameTable);
            pstmt.setLong(2, (Long)ss.get(0));
            pstmt.setLong(3, light);
            pstmt.setLong(4, (Long)ss.get(1));       
            pstmt.executeUpdate();
            System.out.println("Sensor inserted successfully.");
        } catch (SQLException e) {
            e.printStackTrace();
        }
    }

    public void insertActuatorSPRINKLER(String name, String ip, float threshold, String state) {

        String tableName = name + "_" + ip.replace(":", "");
        String insertSQL = "INSERT INTO " + tableName + " (sensorName, ipAddress, threshold, state, timestamp) VALUES (?, ?, ?, ?, ?)";

        try (Connection conn = connect();
            PreparedStatement pstmt = conn.prepareStatement(insertSQL)) {
            pstmt.setString(1, name);
            pstmt.setString(2, ip);
            pstmt.setFloat(3, threshold);
            pstmt.setString(4, state);
            pstmt.setTimestamp(5, new Timestamp(System.currentTimeMillis()));
            pstmt.executeUpdate();
            System.out.println("Actuator inserted successfully.");
        } catch (SQLException e) {
            e.printStackTrace();
        }
    }


    public void insertActuatorFAN(String name, String ip, float threshold, String state) {

        String tableName = name + "_" + ip.replace(":", "");
        String insertSQL = "INSERT INTO " + tableName + " (sensorName, ipAddress, threshold, state, timestamp) VALUES (?, ?, ?, ?, ?)";

        try (Connection conn = connect();
            PreparedStatement pstmt = conn.prepareStatement(insertSQL)) {
            pstmt.setString(1, name);
            pstmt.setString(2, ip);
            pstmt.setFloat(3, threshold);
            pstmt.setString(4, state);
            pstmt.setTimestamp(5, new Timestamp(System.currentTimeMillis()));
            pstmt.executeUpdate();
            System.out.println("Actuator inserted successfully.");
        } catch (SQLException e) {
            e.printStackTrace();
        }
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
