package com.unipi.dii.iot;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.List;

public class IPv6DatabaseManager {
    static final String JDBC_URL = "jdbc:mysql://localhost:3306/iotDatabase?useSSL=false";
    static final String JDBC_USER = "root";
    static final String JDBC_PASSWORD = "root";

    private Connection connect() throws SQLException {
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

    public void fetchIPv6Addresses() {
        String querySQL = "SELECT * FROM ipv6_addresses";

        try (Connection conn = connect();
             Statement stmt = conn.createStatement();
             ResultSet rs = stmt.executeQuery(querySQL)) {

            while (rs.next()) {
                int id = rs.getInt("id");
                String address = rs.getString("address");
                String type = rs.getString("type");
                System.out.println("ID: " + id + ", Address: " + address + ", Type: " + type);
            }
        } catch (SQLException e) {
            e.printStackTrace();
        }
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

    public void createTableActuator() {
        // TODO Auto-generated method stub
        throw new UnsupportedOperationException("Unimplemented method 'createTableActuator'");
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
