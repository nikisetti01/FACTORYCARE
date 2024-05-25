#include "contiki.h"
#include "coap-engine.h"
#include "random.h"
#include "contiki-net.h"
#include <stdio.h>
#include "sys/log.h"
#include "coap-blocking-api.h"
#include "../cJSON-master/cJSON.h"
#include <stdlib.h>
#include <string.h>

#define LOG_MODULE "App"
#define SERVER_EP "coap://[fd00::1]:5683"
#define LOG_LEVEL LOG_LEVEL_APP
#define TIME_SAMPLE 5

extern coap_resource_t res_predict_temp;


PROCESS(thermometer_process, "Thermometer Process");
AUTOSTART_PROCESSES(&thermometer_process);
 static struct etimer prediction_timer;
   void client_chunk_handler(coap_message_t *response) {
    const uint8_t *chunk;


    if (response == NULL) {
        LOG_ERR("Request timed out\n");
        return;
    }

    int len = coap_get_payload(response, &chunk);
    char payload[len + 1];
    memcpy(payload, chunk, len);
    payload[len] = '\0';  // Ensure null-terminated string
    printf("Response: %s\n", payload);
}


PROCESS_THREAD(thermometer_process, ev, data)
{ static coap_endpoint_t server_ep;
static coap_message_t request[1];
  
    PROCESS_BEGIN();

 
    coap_endpoint_parse(SERVER_EP, strlen(SERVER_EP), &server_ep);
    // Initialize POST request
    coap_init_message(request, COAP_TYPE_CON, COAP_POST, 0);
    coap_set_header_uri_path(request, "/registrationSensor");

    // Create JSON payload
    cJSON *root = cJSON_CreateObject();
    if (root == NULL) {
        LOG_ERR("Failed to create JSON object\n");
        PROCESS_EXIT();
    }
    cJSON_AddStringToObject(root, "sensor", "thermometer");
    cJSON_AddStringToObject(root, "ipv6", "[fd00::202:2:2:2]");
    
    cJSON *string_array = cJSON_CreateArray();
    if (string_array == NULL) {
        LOG_ERR("Failed to create JSON array\n");
        cJSON_Delete(root);
        PROCESS_EXIT();
    }
    cJSON_AddItemToArray(string_array, cJSON_CreateString("temperature"));
    cJSON_AddItemToArray(string_array, cJSON_CreateString("humidity"));
    cJSON_AddItemToObject(root, "sensing_type", string_array);
    cJSON_AddNumberToObject(root, "time_sample", TIME_SAMPLE);

    char *payload = cJSON_PrintUnformatted(root);
    if (payload == NULL) {
        LOG_ERR("Failed to print JSON object\n");
        cJSON_Delete(root);
        PROCESS_EXIT();
    }
    printf("il payload %s \n",payload);


    coap_set_payload(request, (uint16_t *)payload, strlen(payload)+30);
    printf("Sending the registration request\n");

    // Send the blocking request
    COAP_BLOCKING_REQUEST(&server_ep, request,client_chunk_handler);

    // Clean up JSON objects
    cJSON_Delete(root);
    free(payload);

    printf("Activate server term\n");
    LOG_INFO("Starting Erbium Example Server\n");

    // Activate resource
    coap_activate_resource(&res_predict_temp, "predict-temp");

    // Initialize random number generator
    random_init(0);

    printf("CoAP server started\n");

    // Main loop
     etimer_set(&prediction_timer, CLOCK_SECOND * 3);


    // Processo principaale in loop
    while(1) {
        PROCESS_YIELD();
        if(etimer_expired(&prediction_timer)) {
            // Aggiorna i sensori e fai la previsione             
              res_predict_temp.trigger();

            // Resetta il timer per il prossimo aggiornamento
            etimer_reset(&prediction_timer);
        }

        printf("è successo qualcosa");
    }



    PROCESS_END();
}
