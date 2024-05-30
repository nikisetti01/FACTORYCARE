package com.unipi.remote;

import java.io.IOException;
import java.util.List;
import java.util.Scanner;
import com.unipi.remote.databaseHelper;
import com.unipi.remote.databaseHelper.PairNameIp;

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
            System.out.println("1. set the temperature threshold for the actuators");
            System.out.println("2. Show status of actuators");
            System.out.println("3. Get the number of danger events");
            System.out.println("4. exit");
            System.out.print("\n chose and option: ");

            int choice = scanner.nextInt();
            scanner.nextLine();

            switch (choice) {
                case 1:
                    System.out.print("Set the threshold for the sprinkler: ");
                    int Sprinklerthr = scanner.nextInt();

                    System.out.print("Set the threshold for the air system: ");
                    int AirSistemthr = scanner.nextInt();


                    String ipv6 =db.setActuatorTemperatureThreshold("sprinkler");
                    String ipv6AirSystem =db.setActuatorTemperatureThreshold("actuator");

                    String uri = "coap://[" + ipv6 + "]:5683/threshold";
                    CoapClient client = new CoapClient(uri);
                    JSONObject json = new JSONObject();
                    json.put("t", Sprinklerthr);
                    CoapResponse response = client.post(json.toString(), 0);

                    String uriAir = "coap://[" + ipv6AirSystem + "]:5683/threshold";
                    CoapClient client2 = new CoapClient(uriAir);
                    JSONObject json2 = new JSONObject();
                    json.put("t", AirSistemthr);
                    CoapResponse response2 = client2.post(json2.toString(), 0);

                    if (response != null) {
                        System.out.println("Response sprinkler: " + response.getResponseText());
                    } else {
                        System.out.println("No response from sprinkler.");
                    }
                    if (response2 != null) {
                        System.out.println("Response air system: " + response2.getResponseText());
                    } else {
                        System.out.println("No response from air system.");
                    }
                    break;

                case 2:
                    // show status of actuators
                    System.out.print("Insert sensor to turn off: ");
                    String sensorOff = scanner.nextLine();
                    sendCommand(sensors, sensorOff, "off");
                    break;

                case 3:
                    List<PairNameIp> ips = db.getIPs("actuator");
                    String ip = null;
                    for (PairNameIp pair : ips) {
                        if(pair.name.equals("actuator"))
                        {
                            ip = pair.ip;
                        }
                    }
                    CoapClient clientDanger = new CoapClient("coap://[" + ip  + "]/threshold");
                    CoapResponse responseDanger = clientDanger.get();
                    if (responseDanger != null) {
                        System.out.println("number of danger events : " + responseDanger.getResponseText());
                    } else {
                        System.out.println("No response from server.");
                    }
                    break;             

                case 4:
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