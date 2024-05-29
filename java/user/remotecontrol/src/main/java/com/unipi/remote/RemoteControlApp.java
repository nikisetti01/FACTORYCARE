package com.unipi.remote;

import java.io.IOException;
import java.util.Scanner;
import com.unipi.remote.databaseHelper;
import org.eclipse.californium.core.CoapClient;
import org.eclipse.californium.core.CoapResponse;
import org.eclipse.californium.elements.exception.ConnectorException;
import org.json.simple.JSONObject;

public class RemoteControlApp {

    static databaseHelper db = new databaseHelper();

    private static String COAP_ACTUATORS_URI = null;
    private static String COAP_SENSORS_URI = null;

    private static final String SOIL_RESOURCE_URI = "coap://[fd00::203:3:3:3]/soil";

    private static final String RESOURCE_LPGLEVEL_ = "/res_danger";
    private static final String RESROUCE_TEMP = "/res_temp";    
    private static final String RESOURCE = "/phase";


    public static void main(String[] args) throws ConnectorException, IOException {
        Scanner scanner = new Scanner(System.in);

        CoapClient sensors = new CoapClient(COAP_SENSORS_URI);
        CoapClient actuators = new CoapClient(COAP_ACTUATORS_URI);

        while (true) {
            System.out.println("Remote Control Application");
            System.out.println("1. Turn on a sensor");
            System.out.println("2. Turn off a sensor");
            System.out.println("3. Turn on an actuator");
            System.out.println("4. Turn off an actuator");
            System.out.println("5. set the temperature threshold for the actuators");
            System.out.println("6. Show status of actuators");
            System.out.println("7. Set new sample timing");
            System.out.println("8. Get the number of danger rows in the database");
            System.out.println("9. exit");
            System.out.print("\n chose and option: ");

            int choice = scanner.nextInt();
            scanner.nextLine();

            switch (choice) {
                case 1:
                    // Turn on sensor
                    System.out.print("Insert sensor to turn on: ");
                    String sensorOn = scanner.nextLine();
                    sendCommand(sensors, sensorOn, "on");
                    break;
                case 2:
                    // Turn off a sensor
                    System.out.print("Insert sensor to turn off: ");
                    String sensorOff = scanner.nextLine();
                    sendCommand(sensors, sensorOff, "off");
                    break;
                case 3:
                    // Turn on actuator
                    System.out.print("Insert actuator to turn on: ");
                    String actuatorOn = scanner.nextLine();
                    sendCommand(actuators, actuatorOn, "on");
                    break;
                case 4:
                    // Turn off a actuator
                    System.out.print("Insert actuator to turn off: ");
                    String actuatorOff = scanner.nextLine();
                    sendCommand(actuators, actuatorOff, "off");
                    break;
                case 5:
                    // Show status of sensors
                    System.out.println("Insert the temperature threshold for the actuators: ");
                    int tempThreshold = scanner.nextInt();
                    setTemperatureThreshold(tempThreshold);
                    break;
                case 6:
                    // Show status of actuators
                    getStatus(actuators);
                    break;
                case 7:
                    // Choose resource
                    System.out.println("Choose a resource:");
                    System.out.println("1. CO2");
                    System.out.println("2. Light");
                    System.out.println("3. Phase");
                    System.out.println("4. Moisture");
                    System.out.println("5. Temperature");

                    int resource = scanner.nextInt();
                    scanner.nextLine();

                    switch (resource) {
                        case 1:
                            System.out.print("Insert CO2 sample timing: ");
                            int co2sampling = scanner.nextInt();
                            //setCO2Sampling(co2sampling);
                            break;
                        case 2:
                            System.out.print("Insert Light sample timing: ");
                            int lightSampling = scanner.nextInt();
                            //setLightSampling(lightSampling);
                            break;
                        case 3:
                            System.out.print("Insert Farm Phase sample timing: ");
                            int phaseSampling = scanner.nextInt();
                            //setPhaseSampling(phaseSampling);
                            break;
                        case 4:
                            System.out.print("Insert Moisture sample timing: ");
                            int moistureSampling = scanner.nextInt();
                            //setMoistureSampling(moistureSampling);
                            break;
                        case 5:
                            System.out.print("Insert Temperature sample timing: ");
                            int temperatureSampling = scanner.nextInt();
                            //setTemperatureSampling(temperatureSampling);
                            break;
                        default:
                            System.out.println("Invalid resource");
                    }
                    break;

                case 8:
                    // Get the number of danger rows in the database
                    int rows = db.getDangerRows();
                    if (rows != 0) {
                        System.out.println("Number of danger rows: " + rows);
                    } else {
                        System.out.println("No response from server.");
                    }
                    break;

                case 9:
                    // Exit
                    System.out.println("Exiting...");
                    System.exit(0);
                    break;
                    
                default:
                    System.out.println("Invalid choice");
            }
        }
    }

    private static void setTemperatureThreshold(int tempThreshold) {
        // TODO Auto-generated method stub
        throw new UnsupportedOperationException("Unimplemented method 'setTemperatureThreshold'");
    }

    private static void sendCommand(CoapClient client, String device, String state) throws ConnectorException, IOException {
        JSONObject json = new JSONObject();

        if (client.getURI().equals(COAP_SENSORS_URI)) {
            json.put("sensor", device);
        } else if (client.getURI().equals(COAP_ACTUATORS_URI)) {
            json.put("actuator", device);
        }
        json.put("state", state);

        CoapResponse response = client.post(json.toString(), 0);
        if (response != null) {
            System.out.println("Response: " + response.getResponseText());
        } else {
            System.out.println("No response from server.");
        }
    }


    private static void getStatus(CoapClient client) throws ConnectorException, IOException {
        CoapResponse response = client.get();
        if (client.getURI().equals(COAP_SENSORS_URI)) {
            if (response != null) {
                System.out.println("Sensors overview" + response.getResponseText());
            } else {
                System.out.println("No response from server.");
            }            
        } else if (client.getURI().equals(COAP_ACTUATORS_URI)) {
            if (response != null) {
                System.out.println("Actuators overview: " + response.getResponseText());
            } else {
                System.out.println("No response from server.");
            }
        } else {
            System.out.println("No response from server.");
        }
    }


    /*private static void setCO2Sampling(int co2sampling) {
        CoapClient client = new CoapClient(CO2_RESOURCE_URI);        
    }

    private static void setLightSampling(int co2sampling) {
        CoapClient client = new CoapClient(LIGHT_RESOURCE_URI);        

    }

    private static void setPhaseSampling(int co2sampling) {
        CoapClient client = new CoapClient(PHASE_RESOURCE_URI);        

    }

    private static void setMoistureSampling(int co2sampling) {
        CoapClient client = new CoapClient(SOIL_RESOURCE_URI);        

    }*/

    private static void setTemperatureSampling(int temperatureSampling) {
        //get the ipv6 address of the actuator we want
        String ipv6 = db.setActuatorTemperatureThreshold("fanActuator", temperatureSampling);
       // CoapClient client = new CoapClient(SOIL_RESOURCE_URI);   
       CoapClient client = new CoapClient("coap://[" + ipv6 + "]/temperature_threshold");     
 
    }
}