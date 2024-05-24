package com.unipi.dii.iot;

public class main {
    public static void main(String args[]) {
        MyServer server = new MyServer();
       // server.add(new CoAPResourceExample("hello"));
       server.add(new CoapResourceRegistrationSensor("sensor_registration"));

        server.start();
        System.out.print("The server is Running!");
    }
}
