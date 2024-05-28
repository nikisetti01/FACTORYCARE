package com.unipi.dii.iot;

import org.eclipse.californium.core.CoapClient;
import org.eclipse.californium.core.CoapHandler;
import org.eclipse.californium.core.CoapObserveRelation;
import org.eclipse.californium.core.CoapResponse;
import org.json.simple.JSONArray;
import org.json.simple.JSONObject;
import org.json.simple.parser.JSONParser;

import com.mysql.cj.xdevapi.JsonArray;
import com.unipi.dii.iot.IPv6DatabaseManager;

public class CoapObserverTemp {

    private CoapClient client;
  private  CoapObserveRelation relation;
    

    public CoapObserverTemp() {
       
        // Crea il client CoAP
        String uri="coap://[fd00::202:2:2:2]:5683/monitoring_temp";
        client = new CoapClient(uri);
    

    }

    public void startObserving() {
        // Inizia l'osservazione della risorsa
        relation = client.observe(new CoapHandler() {
            @Override
            public void onLoad(CoapResponse response) {
                IPv6DatabaseManager dbManger= new IPv6DatabaseManager();
                String content = response.getResponseText();
                System.out.println("Notification: " + content);
                JSONObject json= null;
                try{
                   JSONParser parser = new JSONParser();
                    json = (JSONObject) parser.parse(content);
                    int tiemeid=(int) json.get("id");
                    JSONArray ssArray =(JSONArray) json.get("ss");
                    dbManger.insertSensorTHERMOMETER("thermometer", "fd00:202:202",ssArray, tiemeid);

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
}
