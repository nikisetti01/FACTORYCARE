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
#define MAX_REGISTRATION_RETRY 3
#define GOOD_ACK 65
extern coap_resource_t res_predict_temp;
extern coap_resource_t res_monitoring_temp;
static int registration_retry_count = 0;
static int registered = 0;

PROCESS(thermometer_process, "Thermometer Process");
AUTOSTART_PROCESSES(&thermometer_process);
 static struct etimer prediction_timer;
 static struct etimer monitoring_timer;
 // funzione che scrive in un file di testo campioni casuali di temperatura e umidità per la previsione (10) ogni riga tipo 1 20.0 30.0
void write_samples() {
    FILE *file = fopen("samples.txt", "w");
    if (file == NULL) {
        LOG_ERR("Failed to open file\n");
        return;
    }

    for (int i = 0; i < 10; i++) {
        fprintf(file, "%d %.1f %.1f\n", i, 15.0 + (random_rand() % 150) / 10.0, 30.0 + (random_rand() % 700) / 10.0);
    }

    fclose(file);
}
// funzione che cancella i sample creati
void delete_samples() {
    if (remove("samples.txt") != 0) {
        LOG_ERR("Failed to delete file\n");
    }
}

 void client_chunk_handler(coap_message_t *response) {
    const uint8_t *chunk;

    if (response == NULL) {
        LOG_ERR("Request timed out\n");
        printf("Request timed out\n");
        return;
    }

    int len = coap_get_payload(response, &chunk);
    char payload[len + 1];
    memcpy(payload, chunk, len);
    payload[len] = '\0';  // Ensure null-terminated string
    printf("Response: %i\n", response->code);

    if (response->code == GOOD_ACK
    ) {
        printf("Registration successful\n");
        registered = 1;
    } else {
        printf("Registration failed\n");
    }
}


PROCESS_THREAD(thermometer_process, ev, data)
{ static coap_endpoint_t server_ep;
static coap_message_t request[1];
  
    PROCESS_BEGIN();

 
    coap_endpoint_parse(SERVER_EP, strlen(SERVER_EP), &server_ep);
     while (registration_retry_count < MAX_REGISTRATION_RETRY && registered==0) {
    // Initialize POST request
    coap_init_message(request, COAP_TYPE_CON, COAP_POST, 0);
    coap_set_header_uri_path(request, "/registrationSensor");

    // Create JSON payload
    cJSON *root = cJSON_CreateObject();
    if (root == NULL) {
        LOG_ERR("Failed to create JSON object\n");
        PROCESS_EXIT();
    }
    cJSON_AddStringToObject(root, "s", "thermometer");
    //cJSON_AddStringToObject(root, "ipv6", "[fd00::202:2:2:2]");
    
    cJSON *string_array = cJSON_CreateArray();
    if (string_array == NULL) {
        LOG_ERR("Failed to create JSON array\n");
        cJSON_Delete(root);
        PROCESS_EXIT();
    }
    cJSON_AddItemToArray(string_array, cJSON_CreateString("temperature"));
    cJSON_AddItemToArray(string_array, cJSON_CreateString("humidity"));
    cJSON_AddItemToObject(root, "ss", string_array);
    cJSON_AddNumberToObject(root, "t", TIME_SAMPLE);

    char *payload = cJSON_PrintUnformatted(root);
    if (payload == NULL) {
        LOG_ERR("Failed to print JSON object\n");
        cJSON_Delete(root);
        PROCESS_EXIT();
    }
    printf("il payload %s  lenght  %ld \n",payload, strlen(payload));


    coap_set_payload(request, (uint8_t *)payload, strlen(payload));
    printf("Sending the registration request\n");

    // Send the blocking request
    COAP_BLOCKING_REQUEST(&server_ep, request,client_chunk_handler);
     
      if (registered==0) {
            LOG_INFO("Retry registration (%d/%d)\n", registration_retry_count, MAX_REGISTRATION_RETRY);
            etimer_set(&prediction_timer, CLOCK_SECOND * 10); // Wait 10 seconds before retrying
            PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&prediction_timer));
        }
    }
    // Clean up JSON objects
    //cJSON_Delete(root);
    //free(payload);
if(registered==1){
    // vorrei settare un timer che ogni due secondi attiva la risorsa per il monitoraggio temp e umidità res-monitoring
    // e ogni 10 secondi attiva la risorsa per la previsione della temperatura res-predict-temp
    write_samples();
    printf("Activate server term\n");
    LOG_INFO("Starting Erbium Example Server\n");

    // Activate resource
    coap_activate_resource(&res_predict_temp, "predict-temp");
    coap_activate_resource(&res_monitoring_temp, "monitoring-temp");

    // Initialize random number generator
    random_init(0);

    printf("CoAP server started\n");

    // Main loop
     etimer_set(&prediction_timer, CLOCK_SECOND * 10);
     etimer_set(&monitoring_timer, CLOCK_SECOND * 2);


    // Processo principaale in loop
    while(1) {
        PROCESS_YIELD();
        // Gestione del timer per il monitoraggio
        if(etimer_expired(&monitoring_timer)) {
            // Aggiorna i sensori e fai la previsione             
              res_monitoring_temp.trigger();
              

            // Resetta il timer per il prossimo aggiornamento
            etimer_reset(&monitoring_timer);
        }
        if(etimer_expired(&prediction_timer)) {
            // Aggiorna i sensori e fai la previsione             
              res_predict_temp.trigger();
            
              delete_samples();
              write_samples();
            
              
              

            // Resetta il timer per il prossimo aggiornamento
            etimer_reset(&prediction_timer);
        }

        
    }
} else {
    printf("numero richieste raggiunte");
}



    PROCESS_END();
}
