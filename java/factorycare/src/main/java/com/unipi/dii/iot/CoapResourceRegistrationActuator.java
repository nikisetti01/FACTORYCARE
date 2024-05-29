package com.unipi.dii.iot;

import java.net.InetAddress;
import java.util.List;

import org.eclipse.californium.core.CoapResource;
import org.eclipse.californium.core.coap.CoAP;
import org.eclipse.californium.core.coap.Response;
import org.eclipse.californium.core.server.resources.CoapExchange;
import org.json.simple.JSONObject;

class CoapResourceRegistrationActuator extends CoapResource {

    IPv6DatabaseManager db = new IPv6DatabaseManager();

    public CoapResourceRegistrationActuator(String name) {
        super(name);
        // setObservable(true);
    }

    @Override
    public void handleGET(CoapExchange exchange) {
        exchange.respond("hello world");
    }
    
    @Override
    public void handlePOST(CoapExchange exchange) {
        //db setup
        IPv6DatabaseManager.createTableActuator();
        
        System.out.println("POST actuator received");
    
        String payloadString = exchange.getRequestText();
        String ipAddress = exchange.getSourceAddress().getHostAddress();
        System.out.println("Payload received: " + payloadString + " \nlength: " + payloadString.length());
        System.out.println("IP address: " + ipAddress + "\n");
    
        Response response;
    
        String actuator =payloadString;
    
            if (actuator != null && ipAddress != null) {
                InetAddress addr = exchange.getSourceAddress();
                System.out.println("Source address actautor: " + addr);
                System.out.println("actuator:" + actuator);
    
                // Insert the sensor IP in the database
                try {
                    // Checking for the ip of actuator
                    List<String> actuatorIPs = db.getIPs("actuator");
                    if (actuatorIPs.contains(addr)) {
                        System.out.println("actuator IP already registered");
                        response = new Response(CoAP.ResponseCode.BAD_REQUEST);
                        exchange.respond(response);
                        return;
                    }
                    System.out.println("Inserting ACTUATOR IP in db "+ addr);
                    db.insertIPv6Address(addr.getHostAddress(), "actuator" ,actuator);
                    
                    //after parsing the payload we have to create the table for this actuator
                    IPv6DatabaseManager.createTableActuator();
    
                    // Create response JSON object
                    JSONObject responseJson = new JSONObject();
                    // taking ips from database
                    List<String> sensorIPs = db.getIPs("sensor");
                    for (String ip : sensorIPs) {
                        responseJson.put("s", ip);
                    }
                    System.out.println("indirizzi ip sensori per l'attuatore " + responseJson.toString());
                    //responseJson.put("t", "fd00::202:2:2:2");
                    //responseJson.put("l", "[fd00::204:4:4:4]");
    
                    response = new Response(CoAP.ResponseCode.CREATED);
                    response.setPayload(responseJson.toJSONString());
                    System.out.print(CoAP.ResponseCode.CREATED);
                    exchange.respond(response);
                } catch (Exception e) {
                    System.err.println("Error inserting sensor IP in the database: " + e.getMessage());
                    response = new Response(CoAP.ResponseCode.INTERNAL_SERVER_ERROR);
                    exchange.respond(response);
                }
            } else {
                System.err.println("Missing required key 'a' or IP address");
                response = new Response(CoAP.ResponseCode.BAD_REQUEST);
                exchange.respond(response);
            }
        } 
    }
    
