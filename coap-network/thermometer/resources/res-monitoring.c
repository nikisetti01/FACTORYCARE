#include "contiki.h"
#include "contiki-net.h"
#include "coap-engine.h"
#include "machine_learning/temperature_prediction.h"
<<<<<<< HEAD
=======
#include "global_variables.h"
>>>>>>> lpgSensor
#include "../cJSON-master/cJSON.h"
#include <stdio.h>
#include <stdlib.h> // Aggiunto per utilizzare random_rand()
#include <string.h>
#include "../global_variable/global_variables.h"
static int count = 0;
static float new_temperature = 0.0;
static float new_humidity = 0.0;
extern Sample samples[10]; // Array to store the samples

<<<<<<< HEAD
=======

static int counter=0;
>>>>>>> lpgSensor
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
    
<<<<<<< HEAD
    // Usa le variabili globali per ottenere temperatura ed umidità
    printf("count %d\n", count);
    printf("new_temperature %f\n", new_temperature);
    printf("new_humidity %f\n", new_humidity);
    // leggi la temperatura e l'umidità dalla variabile globale sensors e mettile in new_temperature e new_humidity con count corrispettivo
    new_temperature = samples[count].temperature;
    new_humidity = samples[count].humidity;
=======
     // leggi da sample.txt temperatura ed umidità dove count== il primo valore di ogni riga poi incrementa (max 10) il contatore
    FILE *file = fopen("sample.txt", "r");
    if (file == NULL) {
        printf("Failed to open file\n");
        return;
    }
    int count;
    float new_temperature;
    float new_humidity;
    while (fscanf(file, "%d %f %f", &count, &new_temperature, &new_humidity) != EOF) {
        printf("count %d\n", count);
        printf("new_temperature %f\n", new_temperature);
        printf("new_humidity %f\n", new_humidity);
        if(counter==count)
        break;
            
        
    }
    fclose(file);
>>>>>>> lpgSensor

    // Crea un json poi da mandare come richiesta coap
    cJSON *root = cJSON_CreateObject();
<<<<<<< HEAD
    cJSON_AddNumberToObject(root, "timeid", count);
    cJSON_AddNumberToObject(root, "temperature", new_temperature);
    cJSON_AddNumberToObject(root, "humidity", new_humidity);
=======
    cJSON_AddNumberToObject(root, "id", count);
    cJSON_AddNumberToObject(root, "t", new_temperature);
    cJSON_AddNumberToObject(root, "h", new_humidity);
>>>>>>> lpgSensor
    char *json = cJSON_Print(root);
    cJSON_Delete(root);
    printf("json %s\n", json);
    
    // Trasformalo in stringa da mandare
    int length = snprintf((char *)buffer, preferred_size, "%s", json);
    coap_set_header_etag(response, (uint8_t *)&length, 1);
    coap_set_payload(response, (uint8_t *)buffer, length);
    
    count++;
}

<<<<<<< HEAD
static void res_event_handler(void)  {
=======
static void res_event_handler(void) {
>>>>>>> lpgSensor
    coap_notify_observers(&res_monitoring_temp);
}

