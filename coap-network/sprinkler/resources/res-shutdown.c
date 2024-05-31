
#include "contiki.h"
#include "contiki-net.h"
#include "coap-engine.h"


#include <stdio.h>
#include <string.h>
#include "../global_variable/global_variables.h"

extern float temp_tresh;
 void res_shutdown_get_handler(coap_message_t *request, coap_message_t *response,
                                     uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
/* Resource definition */
RESOURCE(res_shutdown,
         "title=\"Shutdown Resource\";rt=\"shutdown\"",
         res_shutdown_get_handler,
         NULL,
         NULL,
         NULL);
 void res_shutdown_get_handler(coap_message_t *request, coap_message_t *response,
                                     uint8_t *buffer, uint16_t preferred_size, int32_t *offset)

{ 
    printf("Shutdown incremented\n");
    temp_tresh=-1;
   



}

