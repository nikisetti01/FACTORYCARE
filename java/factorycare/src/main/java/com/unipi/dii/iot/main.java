package com.unipi.dii.iot;

public class main {

    static IPv6DatabaseManager db = new IPv6DatabaseManager();
    public static void main(String args[]) {
        MyServer server = new MyServer();
        //server.add(new CoAPResourceExample("hello"));
        server.add(new CoapResourceRegistrationSensor("registrationSensor"));
        server.add(new CoapResourceRegistrationActuator("registrationActuator"));
        server.start();
        System.out.print("The server is Running!");
    }
}
