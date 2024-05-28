#include "contiki.h"
#include "contiki-net.h"
#include "coap-engine.h"
#include "machine_learning/temperature_prediction.h"
#include "global_variables.h"
#include <stdio.h>
#include <string.h>
#include "global_variables.h"

#include <stdlib.h> // Aggiunto per utilizzare random_rand()


static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer,
                            uint16_t preferred_size, int32_t *offset);

static void res_event_handler(void);
static float temperatures[10]; 
static float humidities[10];
EVENT_RESOURCE(res_predict_temp,
         "title=\"Prediction\";rt=\"Text\"",
         res_get_handler,
         NULL,
         NULL,
         NULL,
         res_event_handler);

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer,
                             uint16_t preferred_size, int32_t *offset) {
    
   // usa la funzione float predict_next_temperature_from_values(float temp[], float hum[]) per predire la temperatura con input 10 elemnti temp e 10 elementi hum della globale samples
   for(int i=0; i<10; i++){
       temperatures[i] = samples[i].temperature;
       humidities[i] = samples[i].humidity;
   }
    float next_temperature = predict_next_temperature_from_values(temperatures, humidities);
    printf("Next temperature: %.2f\n", next_temperature);


    
    // Controlla se il buffer ha abbastanza spazio
    if (preferred_size < COAP_MAX_CHUNK_SIZE) {
        coap_set_status_code(response, BAD_OPTION_4_02);
        return;
    }

    // Costruisce il payload della risposta
    int length = snprintf((char *)buffer, preferred_size, "%.2f", next_temperature);

    // Imposta i campi della risposta CoAP
    coap_set_header_content_format(response, TEXT_PLAIN);
    coap_set_header_etag(response, (uint8_t *)&length, 1);
    coap_set_payload(response, (uint8_t *)buffer, length);
}

static void res_event_handler(void) {
    printf("mando notifica \n");
    coap_notify_observers(&res_predict_temp);
}
