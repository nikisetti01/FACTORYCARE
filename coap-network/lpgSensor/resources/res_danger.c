
#include <stdlib.h>
#include <string.h>
#include "coap-engine.h"

#include <stdio.h>
#include "../global_variable/global_variables.h"
#include "model/functionsML.h"

#define MAX_LINE_LENGTH 100
extern Sample sample;
static int prediction = 0;
static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);

static void res_event_handler(void);

EVENT_RESOURCE(res_danger,
         "title=\"airquality\"; rt=\"Text\"",
         res_get_handler,
         NULL,
         NULL,
         NULL,
         res_event_handler);

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) {
    printf("get_danger: predizione in corso\n");

  // metti in array features gli elementi di sample
  float features[4] = {sample.co, sample.smoke, sample.light, sample.humidity};
  // usa la funzione predi
    prediction = predict_class(features,4);
  //prediction=features[0];
    
    int val = prediction;

    printf("val %i\n", val);
    
    // Formatta il valore come stringa
    int length = snprintf((char *)buffer, preferred_size, "%d", val);

    if (length < 0) {
        coap_set_status_code(response, INTERNAL_SERVER_ERROR_5_00);
        return;
    }

    // Verifica che il buffer abbia abbastanza spazio
    if (length >= preferred_size) {
        coap_set_status_code(response, BAD_OPTION_4_02);
        return;
    }

    coap_set_header_content_format(response, TEXT_PLAIN);
    coap_set_header_etag(response, (uint8_t *)&length, 1);
    coap_set_payload(response, buffer, length);
}

static void res_event_handler(void) {
    printf("mando notifica lpgSensor\n");
    coap_notify_observers(&res_danger);
}

