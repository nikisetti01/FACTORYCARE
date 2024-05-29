package com.unipi.dii.iot;

import org.eclipse.californium.core.CoapClient;
import org.eclipse.californium.core.CoapHandler;
import org.eclipse.californium.core.CoapObserveRelation;
import org.eclipse.californium.core.CoapResponse;
import org.json.simple.JSONArray;
import org.json.simple.JSONObject;
import org.json.simple.parser.JSONParser;


public class CoapObserverTemp implements Runnable {

    private CoapClient client;
  private  CoapObserveRelation relation;
  private String name;
  private String ipv6;
    

    public CoapObserverTemp(String ipv6,String name) {
       
        // Crea il client CoAP
        String uri = "coap://[" + ipv6 + "]:5683/monitoring"; // Adjust the URI as needed
        client = new CoapClient(uri);
        this.ipv6=ipv6;
        this.name=name;
    

    }

    public void startObserving() {
        // Inizia l'osservazione della risorsa
        relation = client.observe(new CoapHandler() {
            @Override
            public void onLoad(CoapResponse response) 
            {
                System.out.println("Received notification");
                IPv6DatabaseManager dbManger= new IPv6DatabaseManager();
                String content = response.getResponseText();
                System.out.println("Notification: " + content);
                JSONObject json= null;
                try{
                   JSONParser parser = new JSONParser();
                   if(name.equals("thermometer")){
                    json = (JSONObject) parser.parse(content);
                    long tiemeid=(long) json.get("id");
                    JSONArray ssArray =(JSONArray) json.get("ss");
                    System.out.println("ARRAY VALORI THERMOMETER:" + ssArray.toString());
                    dbManger.insertSensorTHERMOMETER("thermometer", ipv6,ssArray, tiemeid);
                   }
                   else if(name.equals("lpgSensor")) {
                    json = (JSONObject) parser.parse(content);
                    if (json.containsKey("id") && json.containsKey("ss")) {
                        long timeid = (long) json.get("id");
                        JSONArray ssArray = (JSONArray) json.get("ss");
                        System.out.println("ARRAY VALORI LPG:" + ssArray.toString());
                        dbManger.insertSensorLPG("lpgSensor", ipv6,ssArray, timeid);
                    } else {
                        System.out.println("Il JSON non contiene i campi 'id' e/o 'ss'");
                    }
                }

                } catch (Exception e) {
                    e.printStackTrace();
                }
              
            }

            @Override
            public void onError() {
                System.err.println("Failed to receive notification");
            }
        });
    }

    public void stopObserving() {
        if (relation != null) {
            relation.proactiveCancel();
        }
        if (client != null) {
            client.shutdown();
        }
    }
    @Override
    public void run() {
        startObserving();
    }
}
