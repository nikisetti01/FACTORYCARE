package com.unipi.dii.iot;

import org.eclipse.californium.core.CoapResource;
import org.eclipse.californium.core.server.resources.CoapExchange;

class CoAPResourceExample extends CoapResource {
    public CoAPResourceExample(String name) {
    super(name);
    setObservable(true);
    }
    @Override
    public void handleGET(CoapExchange exchange) {
        System.out.println("\nGET request received");
    exchange.respond("hello world\n");
    }
    @Override
    public void handlePOST(CoapExchange exchange) {
    /* your stuff */
    exchange.respond("post hello world!\n");
    }
}
   