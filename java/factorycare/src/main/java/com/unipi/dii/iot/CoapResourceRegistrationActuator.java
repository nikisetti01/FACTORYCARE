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

class CoapResourceRegistrationActuator extends CoapResource {

    IPv6DatabaseManager db = new IPv6DatabaseManager();

    public CoapResourceRegistrationActuator(String name) {
        super(name);
        setObservable(true);
    }

    @Override
    public void handleGET(CoapExchange exchange) {
        exchange.respond("hello world");
    }

    
    @Override
    public void handlePOST(CoapExchange exchange) {
        byte[] request = exchange.getRequestPayload();
        String s;
        s = new String(request);
        JSONObject json = null;
        try {
            JSONParser parser = new JSONParser();
            json = (JSONObject) parser.parse(s);
        }catch (Exception err){
            System.err.println("Json format not valid!");
        }

        Response response;
        if (json.containsKey("sensors")){
            InetAddress addr = exchange.getSourceAddress();
		    System.out.println(addr);
            try {
                //inserting the sensor ip in the database
                db.insertIPv6Address(String.valueOf(addr).substring(1), "actuator");
                db.insertActuator();

                // Retrieve the list of sensor IPs
                List<String> sensorIPs = db.getSensorIPs();
                JSONArray sensorArray = new JSONArray();
                sensorArray.addAll(sensorIPs);
                
                // Create the response JSON
                JSONObject jsonResponse = new JSONObject();
                //jsonResponse.put("sensorIPs", sensorArray);
                
                response = new Response(CoAP.ResponseCode.CREATED);
                response.setPayload(jsonResponse.toJSONString());
            } catch (Exception e) {

                response = new Response(CoAP.ResponseCode.INTERNAL_SERVER_ERROR);
                System.err.println("error sending ipv6s");
            }

        }else{
            response = new Response(CoAP.ResponseCode.BAD_REQUEST);
        }
        exchange.respond(response);
    }
}
