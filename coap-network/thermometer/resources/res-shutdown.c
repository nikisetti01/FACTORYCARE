
#include "contiki.h"
#include "contiki-net.h"
#include "coap-engine.h"


#include <stdio.h>
#include <string.h>
#include "global_variables.h"

/* Handler function for the GET request */
static void res_shutdown_get_handler(coap_message_t *request, coap_message_t *response,
                                     uint8_t *buffer, uint16_t preferred_size, int32_t *offset);

/* Resource definition */
RESOURCE(res_shutdown,
         "title=\"Shutdown Resource\";rt=\"shutdown\"",
         res_shutdown_get_handler,
         NULL,
         NULL,
         NULL);
static void res_shutdown_get_handler(coap_message_t *request, coap_message_t *response,
                                     uint8_t *buffer, uint16_t preferred_size, int32_t *offset)

{ 
    printf("Shutdown incremented\n");
    shutdown=1; // Increment the shutdown variable

    /* Set the response payload */
    coap_set_payload(response, "Shutdown incremented", 19);

    /* Set the response code */
    coap_set_status_code(response, CONTENT_2_05);
}
