#include "contiki.h"

#include "coap-engine.h"
#include "random.h"
#include "contiki-net.h"
#include <stdio.h>
#include "sys/log.h"
#include "coap-blocking-api.h"
#include "../cJSON-master/cJSON.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define LOG_MODULE "App"
#define SERVER_EP "coap://[fd00::1]"
#define LOG_LEVEL LOG_LEVEL_APP
#define TIME_SAMPLE 5


extern coap_resource_t res_predict_temp;
coap_message_t request[1]; 

PROCESS(thermometer_process, "Thermometer Process");
AUTOSTART_PROCESSES(&thermometer_process);
// function for ask registration
static void send_reg_request() {
    static coap_endpoint_t server_ep;
    static coap_message_t request[1];
    //ipv6 java server
    // init post request
    coap_endpoint_parse(SERVER_EP, strlen(SERVER_EP), &server_ep);
    coap_init_message(request, COAP_TYPE_CON, COAP_POST, 0);
    coap_set_header_uri_path(request, "/sensor_registration");
    // creation with cJSON post request
    cJSON *root = cJSON_CreateObject();
    if (root == NULL) {
        LOG_ERR("Failed to create JSON object\n");
        return;
    }
    // adding the name of the sensor
    cJSON_AddStringToObject(root, "sensor", "thermometer");
    // adding the IPV6
    cJSON_AddStringToObject(root, "ipv6", "[fd00::202:2:2:2]");
    cJSON *string_array = cJSON_CreateArray();
    if (string_array == NULL) {
        // Gestione dell'errore se la creazione dell'array fallisce
        cJSON_Delete(root);
        printf("problemi json");
        return;
    }
    // adding an array of the sensor inside thermometer-> for the column in monitoring table
    cJSON_AddItemToArray(string_array, cJSON_CreateString("temperature"));
    cJSON_AddItemToArray(string_array, cJSON_CreateString("humidity"));
    cJSON_AddItemToObject(root, "sensing_type", string_array);
    // also add I think the number for sampling
    cJSON_AddNumberToObject(root, "time_sample", TIME_SAMPLE);
    char *payload = cJSON_PrintUnformatted(root);
    if (payload == NULL) {
        LOG_ERR("Failed to print JSON object\n");
        cJSON_Delete(root);
        return;
    }

    coap_set_payload(request, (uint8_t *)payload, strlen(payload));

    printf("sending the registration request");
    COAP_BLOCKING_REQUEST(&server_ep, request, NULL);
    cJSON_Delete(root);
    free(payload);
}


PROCESS_THREAD(thermometer_process, ev, data)
{
    PROCESS_BEGIN();
    PROCESS_PAUSE();
    send_reg_request();
    printf("Activate server term \n");
     LOG_INFO("Starting Erbium Example Server\n");

    // Aggiungi la risorsa
    coap_activate_resource(&res_predict_temp, "predict-temp");

    // Inizializza il generatore di numeri casuali
    random_init(0);
    

    printf("CoAP server started\n");

    // Processo principaale in loop
    while(1) {
        PROCESS_WAIT_EVENT();
        printf("è successo qualcosa");
    }

    PROCESS_END();
}
