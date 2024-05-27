package com.unipi.dii.iot;

import java.net.InetAddress;

import org.eclipse.californium.core.CoapResource;
import org.eclipse.californium.core.coap.CoAP;
import org.eclipse.californium.core.coap.Response;
import org.eclipse.californium.core.server.resources.CoapExchange;
import org.json.simple.JSONObject;
import org.json.simple.parser.JSONParser;
import org.json.simple.parser.ParseException;

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
        System.out.println("POST request received");
    
        String payloadString = exchange.getRequestText();
        String ipAddress = exchange.getSourceAddress().getHostAddress();
        System.out.println("Payload received: " + payloadString + " length: " + payloadString.length());
    
        Response response;
    
        String actuator =payloadString;
    
            if (actuator != null && ipAddress != null) {
                InetAddress addr = exchange.getSourceAddress();
                System.out.println("Source address: " + addr);
    
                // Insert the sensor IP in the database
                try {
                    // Assuming we need to store the details in the database as well
                    // db.insertIPv6Address(addr.getHostAddress(), actuator);
                    // db.insertSensorDetails(actuator, ipAddress, sensingType.toString(), timeSample);
    
                    // Create response JSON object
                    JSONObject responseJson = new JSONObject();
                    // Assuming some SQL query results for ipv6 addresses
                    responseJson.put("t", "[fd00::202:2:2:2]");
                    responseJson.put("l", "[fd00::204:4:4:4]");
                    System.out.println(responseJson);
    
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
    
