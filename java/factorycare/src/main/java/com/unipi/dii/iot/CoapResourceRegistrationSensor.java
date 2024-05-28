package com.unipi.dii.iot;

import java.net.InetAddress;
import java.util.List;

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
        //db setup
        
        String payloadString = exchange.getRequestText();
        String ipAddress=exchange.getSourceAddress().getHostAddress();
        System.out.println("Payload received: " + payloadString+ " \nlunghezza: "+ payloadString.length());
        System.out.println("IP address: " + ipAddress + "\n");
        
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
                System.out.println("PROVA1\n");
                String ipv6 = ipAddress;
                System.out.println("PROVA2\n");
                JSONArray sensingType = (JSONArray) json.get("ss");
                System.out.println("PROVA3 \n");
                if (json.containsKey("t") && json.get("t") instanceof String) {
                    String temporary = (String) json.get("t");
                    // il resto del tuo codice
                } else {
                    System.out.println("Il valore di 't' non è una stringa o 't' non esiste");
                }
                if (sensor != null && ipv6 != null && sensingType != null ) {
                    System.out.println("PROVA5\n");
                    InetAddress addr = exchange.getSourceAddress();
                    System.out.println("PROVA6\n");
                    System.out.println("Source address: " + addr);
                    try {
                        // Insert the sensor IP in the database
                        //checking if ip
                        List<String> sensorIPs = db.getSensorIPs();
                        if (sensorIPs.contains(addr.getHostAddress())) {
                            System.err.println("Sensor IP already registered");
                            response = new Response(CoAP.ResponseCode.BAD_REQUEST);
                        }
                        db.insertIPv6Address(addr.getHostAddress(), sensor);
                        System.out.println("Sensor: " + sensor);
                        System.out.println("IPv6: " + ipv6);
                        System.out.println("Sensing Types: " + sensingType);
                        System.out.println("Time Sample: " + timeSample);

                        //insert sensor in the database
                        IPv6DatabaseManager.createTableSensor(sensor, ipv6, sensingType, timeSample);
                        /*if(sensor.equals("lpgsensor")) {
                            db.insertSensorLPG(sensor, ipv6, sensingType, timeSample);
                        } else if(sensor.equals("thermometer")){
                            db.insertSensorTHERMOMETER(sensor, ipv6, sensingType, timeSample);
                        }
                        else
                        {
                            System.err.println("Missing required JSON keys");
                            response = new Response(CoAP.ResponseCode.BAD_REQUEST);
                        }*/
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
            String[] sensingTypes = (String[]) ((JSONArray) json.get("ss")).toArray(new String[0]);
            long timeSample = (Long) json.get("t");

            exchange.respond(response);
            }
    }