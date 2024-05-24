package com.unipi.dii.iot;

import java.net.InetAddress;

import org.eclipse.californium.core.CoapResource;
import org.eclipse.californium.core.coap.CoAP;
import org.eclipse.californium.core.coap.Response;
import org.eclipse.californium.core.server.resources.CoapExchange;
import org.json.simple.JSONObject;
import org.json.simple.parser.JSONParser;

class CoapResourceRegistrationSensor extends CoapResource {

    IPv6DatabaseManager db = new IPv6DatabaseManager();

    public CoapResourceRegistrationSensor(String name) {
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
        String s = new String(request);
        JSONObject json = null;
        try {
            JSONParser parser = new JSONParser();
            json = (JSONObject) parser.parse(s);
        } catch (Exception err) {
            System.err.println("Json format not valid!");
        }

        Response response;
        if (json != null && json.containsKey("sensor")) {
            InetAddress addr = exchange.getSourceAddress();
            System.out.println(addr);

            // Insert the sensor IP in the database

            try {
                db.insertIPv6Address(addr.getHostAddress(), "sensor");
                response = new Response(CoAP.ResponseCode.CREATED);
            } catch (Exception e) {
                response = new Response(CoAP.ResponseCode.INTERNAL_SERVER_ERROR);
                System.err.println("error inserting sensor IP in the database" + e.getMessage());
            }

        } else {
            response = new Response(CoAP.ResponseCode.BAD_REQUEST);
        }
        exchange.respond(response);
    }
}
