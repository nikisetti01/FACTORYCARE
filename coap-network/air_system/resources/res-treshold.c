#include "contiki.h"
#include "contiki-net.h"
#include "coap-engine.h"
#include "../../cJSON-master/cJSON.h" 
#include "../global_variable/global_variables.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer,
                            uint16_t preferred_size, int32_t *offset);
static void res_post_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer,
                             uint16_t preferred_size, int32_t *offset);

// Declare a global variable for the temperature threshold
extern float temp_tresh;  // Default threshold value
extern int nRisktemp;
RESOURCE(res_tresh,
         "title=\"Set Temperature Threshold\";rt=\"Text\"",
         res_get_handler,
         res_post_handler,
         NULL,
         NULL);
static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer,
                            uint16_t preferred_size, int32_t *offset) {
    printf("GET ricevuta\n");
    // Create a JSON object
    cJSON *json=cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "rt", nRisktemp);
    cJSON_AddNumberToObject(json, "rl", nRisklpg);

    // Convert JSON object to string
    char *json_str = cJSON_Print(json);

    // Check if the JSON string fits in the buffer
    int length = strlen(json_str);
    if (length > preferred_size) {
        length = preferred_size;
    }

    // Copy JSON string to the buffer
    memcpy(buffer, json_str, length);

   
    // Set response fields
    coap_set_header_content_format(response, APPLICATION_JSON);
    coap_set_header_etag(response, (uint8_t *)&length, 1);
    coap_set_payload(response, buffer, length);

}

static void res_post_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer,
                             uint16_t preferred_size, int32_t *offset) {
    // Declare a temporary variable to store the new threshold
    printf("POST ricevuta\n");
    nRisklpg=0;
    nRisktemp=0;
    const uint8_t *payload = NULL;
    int payload_len = coap_get_payload(request, &payload);

    // Check if payload is present and is valid
    if (payload_len) {
        char temp_str[16];
        if (payload_len >= sizeof(temp_str)) {
            coap_set_status_code(response, BAD_REQUEST_4_00);
            return;
        }

        // Copy the payload into a temporary buffer
        memcpy(temp_str, payload, payload_len);
        temp_str[payload_len] = '\0';

        // Convert the payload to float
        temp_tresh = atof(temp_str);

        // Log the new threshold
        printf("New temperature threshold set: %.2f\n", temp_tresh);

        // Construct the response payload
        int length = snprintf((char *)buffer, preferred_size, "Threshold set to: %.2f", temp_tresh);

        // Set response fields
        coap_set_header_content_format(response, TEXT_PLAIN);
        coap_set_header_etag(response, (uint8_t *)&length, 1);

        // Set response payload
        coap_set_payload(response, (uint8_t *)buffer, length);
    } else {
        // No payload, set response code to BAD_REQUEST
        coap_set_status_code(response, BAD_REQUEST_4_00);
    }
}