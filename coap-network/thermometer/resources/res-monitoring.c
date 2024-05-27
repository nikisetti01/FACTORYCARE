#include "contiki.h"
#include "contiki-net.h"
#include "coap-engine.h"
#include "machine_learning/temperature_prediction.h"
#include "global_variables.h"
#include <stdio.h>
#include <stdlib.h> // Aggiunto per utilizzare random_rand()



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
    }
    fclose(file);

    
    //crea un json poi da mandare come richiesta coap
    cJSON *root = cJSON_CreateObject();
    cjson_add_number_to_object(root, "timeid", count);
    cJSON_AddNumberToObject(root, "temperature", new_temperature);
    cJSON_AddNumberToObject(root, "humidity", new_humidity);
    char *json = cJSON_Print(root);
    cJSON_Delete(root);
    printf("json %s\n", json);
    // trasformalo in stringa da mandare
    int length = snprintf((char *)buffer, preferred_size, "%s", json);
    coap_set  _header_etag(response, (uint8_t *)&length, 1);
    coap_set_payload(response, (uint8_t *)buffer, length);
    count++;
}

static void res_event_handler(void) {
    coap_notify_observers(&res_monitor_temp);
}

