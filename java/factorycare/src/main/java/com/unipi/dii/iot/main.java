package com.unipi.dii.iot;

public class main {
    public static void main(String args[]) {
        MyServer server = new MyServer();
        server.add(new CoAPResourceExample("hello"));
        server.start();
        System.out.print("The server is Running!");
    }
}
