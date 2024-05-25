package com.unipi.dii.iot;

public class main {
    public static void main(String args[]) {
        MyServer server = new MyServer();
<<<<<<< HEAD
       // server.add(new CoAPResourceExample("hello"));
       server.add(new CoapResourceRegistrationSensor("sensor_registration"));

=======
        server.add(new CoAPResourceExample("hello"));
        server.add(new CoapResourceRegistrationSensor("registrationSensor"));
        server.add(new CoapResourceRegistrationActuator("registrationActuator"));
<<<<<<< HEAD
=======
>>>>>>> d6acee9 (added all the modifies for server and java app with leds for registration)
>>>>>>> main
        server.start();
        System.out.print("The server is Running!");
    }
}
