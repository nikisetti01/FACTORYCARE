package com.unipi.dii.iot;

import java.net.InetAddress;

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
        System.out.println("POST request received");
        
        byte[] request = exchange.getRequestPayload();
        String payloadString = new String(request);
        System.out.println("Payload received: " + payloadString);
        
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
            String sensor = (String) json.get("sensor");
            String ipv6 = (String) json.get("ipv6");
            JSONArray sensingType = (JSONArray) json.get("sensing_type");
            Long timeSample = (Long) json.get("time_sample");
           
            if (sensor != null && ipv6 != null && sensingType != null && timeSample != null) {
                InetAddress addr = exchange.getSourceAddress();
                System.out.println("Source address: " + addr);

                // Insert the sensor IP in the database
               
                try {
                   
                    db.insertIPv6Address(addr.getHostAddress(), sensor);
                    System.out.println("Sensor: " + sensor);
                    System.out.println("IPv6: " + ipv6);
                    System.out.println("Sensing Types: " + sensingType);
                    System.out.println("Time Sample: " + timeSample);

                    // Assuming we need to store the details in the database as well
                   // db.insertSensorDetails(sensor, ipv6, sensingType.toString(), timeSample);

                    response = new Response(CoAP.ResponseCode.CREATED);
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
        exchange.respond(response);
    }
}
