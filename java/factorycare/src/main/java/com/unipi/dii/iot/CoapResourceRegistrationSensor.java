package com.unipi.dii.iot;

import java.net.InetAddress;
import java.sql.SQLException;

import org.eclipse.californium.core.CoapResource;
import org.eclipse.californium.core.coap.CoAP;
import org.eclipse.californium.core.coap.Response;
import org.eclipse.californium.core.server.resources.CoapExchange;
import org.json.simple.JSONArray;
import org.json.simple.JSONObject;
import org.json.simple.parser.JSONParser;
import org.json.simple.parser.ParseException;

class CoapResourceRegistrationSensor extends CoapResource {

    IPv6DatabaseManager db = new IPv6DatabaseManager();

    public CoapResourceRegistrationSensor(String name) {
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
        IPv6DatabaseManager.createTableSensor();

        System.out.println("POST request received");
        
        String payloadString = exchange.getRequestText();
        String ipAddress=exchange.getSourceAddress().getHostAddress();
        System.out.println("Payload received: " + payloadString+ "lunghezza"+ payloadString.length());
        
        JSONObject json = null;
        try {
            JSONParser parser = new JSONParser();
            json = (JSONObject) parser.parse(payloadString);
            System.out.println("Parsed JSON: " + json);
            } catch (ParseException err) {
                System.err.println("JSON parsing error: " + err.getMessage());
                exchange.respond(CoAP.ResponseCode.BAD_REQUEST, "Invalid JSON format");
                return;
            }
            Response response;
            // Extracting and handling each element in the JSON payload
            if (json != null) {
                String sensor = (String) json.get("s");
                String ipv6 = ipAddress;
                JSONArray sensingType = (JSONArray) json.get("ss");
                int timeSample = (int) json.get("t");

                if (sensor != null && ipv6 != null && sensingType != null ) {
                    InetAddress addr = exchange.getSourceAddress();
                    System.out.println("Source address: " + addr);

                    // Insert the sensor IP in the database

                    try {

                        db.insertIPv6Address(addr.getHostAddress(), sensor);
                        System.out.println("Sensor: " + sensor);
                        System.out.println("IPv6: " + ipv6);
                        System.out.println("Sensing Types: " + sensingType);
                        System.out.println("Time Sample: " + timeSample);

                        db.insertSensor(sensor, ipv6, sensingType.toString(), timeSample);

                        // Assuming we need to store the details in the database as well
                        // db.insertSensorDetails(sensor, ipv6, sensingType.toString(), timeSample);

                        response = new Response(CoAP.ResponseCode.CREATED);
                        System.out.print(CoAP.ResponseCode.CREATED);
                    } catch (Exception e) {
                        System.err.println("Error inserting sensor IP in the database: " + e.getMessage());
                        response = new Response(CoAP.ResponseCode.INTERNAL_SERVER_ERROR);
                    }
                } else {
                    System.err.println("Missing required JSON keys");
                    response = new Response(CoAP.ResponseCode.BAD_REQUEST);
                }
            } else {
                response = new Response(CoAP.ResponseCode.BAD_REQUEST);
            }

            // Assigning values from JSON payload to variables
            String sensor = (String) json.get("s");
            String[] sensingTypes = ((JSONArray) json.get("ss")).toArray(new String[0]);
            long timeSample = (Long) json.get("t");

            exchange.respond(response);
            }
            }
