#include "contiki.h"
#include "contiki-net.h"
#include "coap-engine.h"
#include "machine_learning/temperature_prediction.h"
#include "../cJSON-master/cJSON.h"
#include <stdio.h>
#include <stdlib.h> // Aggiunto per utilizzare random_rand()
#include <string.h>
#include "../global_variable/global_variables.h"

#define SAMPLE_COUNT 10 // Definisci il numero massimo di campioni

static int count = 0;
static float new_temperature = 0.0;
static float new_humidity = 0.0;
extern Sample samples[SAMPLE_COUNT]; // Array to store the samples

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer,
                            uint16_t preferred_size, int32_t *offset);

static void res_event_handler(void);

EVENT_RESOURCE(res_monitoring_temp,
         "title=\"monitoring_temp\";rt=\"Text\"",
         res_get_handler,
         NULL,
         NULL,
         NULL,
         res_event_handler);

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer,
                             uint16_t preferred_size, int32_t *offset) {

    // Verifica che count non superi SAMPLE_COUNT
    if (count >= SAMPLE_COUNT) {
        count = 0; // Reset count if it exceeds the array bounds
    }

    // Usa le variabili globali per ottenere temperatura ed umidità
    printf("count %d\n", count);
    printf("new_temperature %f\n", new_temperature);
    printf("new_humidity %f\n", new_humidity);

    // leggi la temperatura e l'umidità dalla variabile globale sensors e mettile in new_temperature e new_humidity con count corrispettivo
    new_temperature = samples[count].temperature;
    new_humidity = samples[count].humidity;

    // Crea un json poi da mandare come richiesta coap
    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "id", count);
    cJSON_AddNumberToObject(root, "temperature", new_temperature);
    cJSON_AddNumberToObject(root, "humidity", new_humidity);
    char *json = cJSON_Print(root);
    printf("length %lo\n", strlen(json));

    if (json == NULL) {
        coap_set_status_code(response, BAD_REQUEST_4_00);
        return;
    }

    printf("json %s\n", json);
    
    // Trasformalo in stringa da mandare
    int length = snprintf((char *)buffer, preferred_size, "%s", json);


    if (length < 0 || length >= preferred_size) {
        coap_set_status_code(response, BAD_REQUEST_4_00);
        return;
    }

    coap_set_header_etag(response, (uint8_t *)&length, 1);
    coap_set_payload(response, (uint8_t *)buffer, length);
    
    count++;
}

static void res_event_handler(void)  {
    coap_notify_observers(&res_monitoring_temp);
}
