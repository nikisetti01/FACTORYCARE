package com.unipi.remote;

import java.io.IOException;
import java.io.StringReader;
import java.util.List;
import java.util.Scanner;

import javax.json.Json;
import javax.json.JsonObject;
import javax.json.JsonReader;

import org.eclipse.californium.core.CoapClient;
import org.eclipse.californium.core.CoapResponse;
import org.eclipse.californium.core.coap.MediaTypeRegistry;
import org.eclipse.californium.elements.exception.ConnectorException;

import com.unipi.remote.databaseHelper.PairNameIp;



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
        try{
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
                    Boolean checkDevice0 = false;
                    String nodeName0 = null;
                    String filter = null;

                    while(!checkDevice0){
                        System.out.print("insert the node to check: ");
                        nodeName0 = scanner.nextLine();
                        //checking of the device is a valid one
                        if (nodeName0.equals("actuator") || nodeName0.equals("sprinkler")) {
                            checkDevice0 = true;
                            filter="actuator";

                        }else if(nodeName0.equals("lpgsensor") || nodeName0.equals("thermometer"))
                        {
                            checkDevice0 = true;
                            filter="sensor";
                        }
                        else{
                            System.out.println("Invalid device name");
                        }
                        
                        if(db.getIPs(filter)==null)
                        {
                            System.out.println(filter + "might be not working or inactive\n");
                        }else
                        {
                            System.out.println(filter + " active and working properly\n");
                        }
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
                    String jsonText = responseDanger.getResponseText();
                    JsonReader jsonReader = Json.createReader(new StringReader(jsonText));
                    JsonObject jsonObject = jsonReader.readObject();
                    
                    int rt = jsonObject.getInt("rt");
                    int rl = jsonObject.getInt("rl");
                    
                    System.out.println("rt: " + rt);
                    System.out.println("rl: " + rl);
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
                    String filterDb = "sensor";
                    if(nodeName.equals("actuator") || nodeName.equals("sprinkler"))
                    {
                        filterDb = "actuator";
                        List<PairNameIp> ipsToContact2 = db.getIPs(filterDb);

                    String ipcont2 = null;

                    for (PairNameIp pair : ipsToContact2) {
                        System.out.println("name e ip: " + pair.name + " " + pair.ip);
                        if(pair.name.equals(nodeName))
                        {
                            ipcont2 = pair.ip;
                        }
                    }
                    CoapClient client4 = new CoapClient("coap://[" + 'f' + ipcont2 + "]/shutdown");
                    System.out.println("RICHIEDO SHUTDOWN A coap://[" +'f' +ipcont2 + "]/shutdown");
                    CoapResponse response4 = client4.get();

                    if (response4 != null) {
                        System.out.println(nodeName + " is shutted\n");
                        //remove from ipv6_addresses the device ip
                        db.removeIp(ipcont2);
                    } else {
                        System.out.println("Server is down or not responding");
                    }
                    }else if(nodeName.equals("lpgsensor") || nodeName.equals("thermometer"))
                    {
                        List<PairNameIp> ipsToContact = db.getIPs(filterDb);
                        String ipcont = null;

                        for (PairNameIp pair : ipsToContact) {
                            System.out.println("name e ip: " + pair.name + " " + pair.ip);
                            if(pair.name.equals(nodeName))
                            {
                                ipcont = pair.ip;
                            }
                        }
                        CoapClient client3 = new CoapClient("coap://["+ ipcont + "]/shutdown");
                        System.out.println("RICHIEDO SHUTDOWN A coap://["+ipcont + "]/shutdown");
                        CoapResponse response3 = client3.get();

                        if (response3 != null) {
                            System.out.println(nodeName + " is shutted\n");
                            //remove from ipv6_addresses the device ip
                            db.removeIp(ipcont);
                        } else {
                            System.out.println("Server is down or not responding");
                        }
                    }
                    //taking the ip of the node
                    
                    break;

                case 5:
                    // Exit
                    System.out.println("Exiting...");
                    System.exit(0);
                    break;
                    
            default:
                System.out.println("Invalid choice");
            }
        }catch(Exception e)
        {
            System.out.println("Invalid input");
            scanner.nextLine();
        }
        }
    }
}