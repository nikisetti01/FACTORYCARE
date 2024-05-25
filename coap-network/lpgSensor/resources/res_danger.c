<<<<<<< HEAD

=======
<<<<<<< HEAD
=======

>>>>>>> d6acee9 (added all the modifies for server and java app with leds for registration)
>>>>>>> main
#include <stdlib.h>
#include <string.h>
#include "coap-engine.h"
#include <stdio.h>

#define MAX_LINE_LENGTH 100

static int prova = 0;
static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
int get_danger();
static void res_event_handler(void);

EVENT_RESOURCE(res_danger,
         "title=\"airquality\"; rt=\"Text\"",
         res_get_handler,
         NULL,
         NULL,
         NULL,
         res_event_handler);

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) {
    printf("siamo in res_get_handler\n");
    int val = get_danger();

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

int get_danger() {
    printf("get_danger: predizione in corso\n");

    int result = prova;
    prova++;
    if (prova > 2) prova = 0;
    printf("result: %d\n", result);

    if (result == -1) {
        printf("Prediction error\n");
    }

    return result;
}
