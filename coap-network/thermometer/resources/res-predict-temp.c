#include "contiki.h"
#include "contiki-net.h"
#include "coap-engine.h"
#include "machine_learning/temperature_prediction.h"
#include "global_variables.h"
#include <stdio.h>
#include <stdlib.h> // Aggiunto per utilizzare random_rand()

// Dichiarazione del prototipo della funzione
float update_sensors_and_predict(void);

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer,
                            uint16_t preferred_size, int32_t *offset);

static void res_event_handler(void);

EVENT_RESOURCE(res_predict_temp,
         "title=\"Prediction\";rt=\"Text\"",
         res_get_handler,
         NULL,
         NULL,
         NULL,
         res_event_handler);

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer,
                             uint16_t preferred_size, int32_t *offset) {
    
    // Predice la prossima temperatura
    float next_temperature = update_sensors_and_predict(); 
    
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
    coap_notify_observers(&res_predict_temp);
}
