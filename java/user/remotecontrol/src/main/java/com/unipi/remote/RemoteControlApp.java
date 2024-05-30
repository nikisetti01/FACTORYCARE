package com.unipi.remote;

import java.io.IOException;
import java.util.List;
import java.util.Scanner;
import com.unipi.remote.databaseHelper;
import com.unipi.remote.databaseHelper.PairNameIp;

import org.eclipse.californium.core.CoapClient;
import org.eclipse.californium.core.CoapResponse;
import org.eclipse.californium.core.coap.MediaTypeRegistry;
import org.eclipse.californium.elements.exception.ConnectorException;
import org.json.simple.JSONObject;



public class RemoteControlApp {

    static databaseHelper db = new databaseHelper();

    public static void main(String[] args) throws ConnectorException, IOException {
        Scanner scanner = new Scanner(System.in);

        while (true) {
            System.out.println("Remote Control Application");
            System.out.println("1. set the temperature threshold for the actuators"); //DONE
            System.out.println("2. Show status of actuators"); //DONE
            System.out.println("3. Get the number of danger events"); //DONE
            System.out.println("4. Turn off a node"); //DONE 
            System.out.println("5. exit"); //DONE
            System.out.print("\n chose and option: ");

            int choice = scanner.nextInt();
            scanner.nextLine();

            switch (choice) {
                case 1:
                    // set the temperature threshold for the actuators
                    System.out.print("Set the threshold for the sprinkler: ");
                    int Sprinklerthr = scanner.nextInt();

                    System.out.print("Set the threshold for the air system: ");
                    int AirSistemthr = scanner.nextInt();


                    String ipv6 =db.setActuatorTemperatureThreshold("sprinkler");
                    String ipv6AirSystem =db.setActuatorTemperatureThreshold("actuator");

                    if(ipv6 == null || ipv6AirSystem == null)
                    {
                        System.out.println("No actuator found: " + ipv6 + " " + ipv6AirSystem);
                        break;
                    }

                    String uri = "coap://[" +'f'+ ipv6 + "]:5683/threshold";
                    CoapClient client = new CoapClient(uri);
                    String payload = Integer.toString(Sprinklerthr);
                    CoapResponse response = client.post(payload, MediaTypeRegistry.TEXT_PLAIN);

                    String uriAir = "coap://[" + 'f' + ipv6AirSystem + "]:5683/threshold";
                    CoapClient client2 = new CoapClient(uriAir);
                    String payload2 = Integer.toString(AirSistemthr);
                    CoapResponse response2 = client2.post(payload2, MediaTypeRegistry.TEXT_PLAIN);

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
                    System.out.print("Enter the name of the actuator to check: ");
                    String actuatorName = scanner.nextLine();
                    System.out.println("Actuator: " + actuatorName);
                    List<PairNameIp> ipsToContact = db.getIPs(actuatorName.toString());

                    String ipcont = null;

                    for (PairNameIp pair : ipsToContact) {
                        System.out.println(pair.name + " " + pair.ip);
                        if(pair.name.equals("actuator"))
                        {
                            ipcont = pair.ip;
                        }
                    }
                    if(ipcont == null)
                    {
                        System.out.println("No actuator found");
                        break;
                    }
                    CoapClient client3 = new CoapClient("coap://[" + ipcont + "]");
                    boolean response3 = client3.ping();

                    if (response3) {
                        System.out.println("Server is up");
                    } else {
                        System.out.println("Server is down or not responding");
                    }
                    break;


                case 3:
                    // get the number of danger events
                    List<PairNameIp> ips = db.getIPs("actuator");
                    String ip = null;
                    for (PairNameIp pair : ips) {
                        if(pair.name.equals("actuator"))
                        {
                            ip = pair.ip;
                        }
                    }
                    if(ip==null)
                    {
                        System.out.println("No actuator found\n");
                        break;
                    }
                    
                    CoapClient clientDanger = new CoapClient("coap://[" + 'f'+ ip  + "]:5683/threshold");
                    System.out.println("RICHIEDO DANGER A coap://[" + 'f'+ip  + "]:5683/threshold");
                    CoapResponse responseDanger = clientDanger.get();
                    if (responseDanger != null) {
                        System.out.println("number of danger events : " + responseDanger.getResponseText() + "\n");
                    } else {
                        System.out.println("No response from server.");
                    }
                    break;             

                case 4:
                    // Turn off a node
                    Boolean checkDevice = false;
                    String nodeName = null;

                    //asking the user for the name of the node to turn off
                    while(!checkDevice){
                        System.out.print("Enter the name of the node to turn off: ");
                        nodeName = scanner.nextLine();
                        //checking of the device is a valid one
                        if (nodeName.equals("actuator") || nodeName.equals("sprinkler") || nodeName.equals("lpgsensor") || nodeName.equals("thermometer")) {
                            checkDevice = true;
                        }
                        else{
                            System.out.println("Invalid device name");
                        }
                    }
                    //taking the ip of the actuator
                    List<PairNameIp> ipsToContact2 = db.getIPs(nodeName.toString());

                    String ipcont2 = null;

                    for (PairNameIp pair : ipsToContact2) {
                        if(pair.name.equals(nodeName))
                        {
                            ipcont2 = pair.ip;
                        }
                    }
                    CoapClient client4 = new CoapClient("coap://[" + 'f'+ipcont2 + "]/shutdown");
                    CoapResponse response4 = client4.get();

                    if (response4 != null) {
                        System.out.println(nodeName + " is shutted");
                        //remove from ipv6_addresses the device ip
                        db.removeIp(ipcont2);
                    } else {
                        System.out.println("Server is down or not responding");
                    }
                    break;

                case 5:
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

    /*private static void setTemperatureSampling(int temperatureSampling) {
        //get the ipv6 address of the actuator we want
        String ipv6 = db.setActuatorTemperatureThreshold("fanActuator", temperatureSampling);
       // CoapClient client = new CoapClient(SOIL_RESOURCE_URI);   
       CoapClient client = new CoapClient("coap://[" + ipv6 + "]/temperature_threshold");     
 
    }*/
}