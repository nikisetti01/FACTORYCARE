package com.unipi.dii.iot;

public class main {
    public static void main(String args[]) {

        // Create the database table
        IPv6DatabaseManager dbManager = new IPv6DatabaseManager();
        dbManager.createTableIPV6();
        dbManager.createTableActuator();


        MyServer server = new MyServer();
        server.add(new CoAPResourceExample("hello"));
        server.add(new CoapResourceRegistrationActuator("registerActuator"));
        server.add(new CoapResourceRegistrationSensor("registerSensor"));
        server.start();
        System.out.print("The server is Running!");
    }
}
