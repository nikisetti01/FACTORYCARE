#include "contiki.h"
#include "coap-engine.h"
#include "random.h"
#include "contiki-net.h"
#include <stdio.h>
#include "sys/log.h"
#include "coap-blocking-api.h"
#include "os/dev/button-hal.h"
#include "leds.h"
#include "../cJSON-master/cJSON.h"
#include <stdlib.h>
#include <string.h>
#include "global_variable/global_variables.h"

#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_APP

#define SERVER_EP "coap://[fd00::1]:5683"
#define TIME_SAMPLE 5
#define MAX_REGISTRATION_RETRY 3
#define GOOD_ACK 65
#define MAX_SAMPLES 10

extern coap_resource_t res_predict_temp;
extern coap_resource_t res_monitoring_temp;
extern coap_resource_t res_shutdown;

static int registration_retry_count = 0;
static int registered = 0;
 static int shutdown=0;
// Array to store the samples
static int sample_count = 0; // Number of samples stored
Sample samples[MAX_SAMPLES];

PROCESS(thermometer_process, "Thermometer Process");
AUTOSTART_PROCESSES(&thermometer_process);

static struct etimer prediction_timer;
static struct etimer monitoring_timer;

// Function to write the samples to the array
void write_samples() {
    if (sample_count < MAX_SAMPLES) {
        samples[sample_count].timeid = sample_count;
        samples[sample_count].temperature = random_rand() % 30;
        samples[sample_count].humidity = random_rand() % 100;
        sample_count++;
    } else {
        LOG_WARN("Sample array is full\n");
    }
}

// Function to delete the oldest sample from the array
void delete_samples() {
    if (sample_count > 0) {
        // Shift the samples to the left
        for (int i = 0; i < sample_count - 1; i++) {
            samples[i] = samples[i + 1];
        }
        sample_count--;
    }
}

void client_chunk_handler(coap_message_t *response) {
    const uint8_t *chunk;
    if (response == NULL) {
        LOG_ERR("Request timed out\n");
        printf("Request timed out\n");
        return;
    }
    int len = coap_get_payload(response, &chunk);
    char payload[len + 1];
    memcpy(payload, chunk, len);
    payload[len] = '\0';  // Ensure null-terminated string
    printf("Response: %i\n", response->code);
    if (response->code == GOOD_ACK) {
        printf("Registration successful\n");
        registered = 1;
    } else {
        printf("Registration failed\n");
    }
}

PROCESS_THREAD(thermometer_process, ev, data)
{
    //button_hal_button_t *btn;
    static coap_endpoint_t server_ep;
    static coap_message_t request[1];
   // btn= button_hal_get_by_index(0);
    int pressed=0;

    PROCESS_BEGIN();

    random_init(0); // Initialize random number generator
    while(ev != button_hal_press_event || pressed==0) {
        pressed=1;
        PROCESS_YIELD();
    
    coap_endpoint_parse(SERVER_EP, strlen(SERVER_EP), &server_ep);
    while (registration_retry_count < MAX_REGISTRATION_RETRY && registered == 0) {
        // Initialize POST request
            leds_on(LEDS_RED);
        leds_single_on(LEDS_YELLOW);

        coap_init_message(request, COAP_TYPE_CON, COAP_POST, 0);
        coap_set_header_uri_path(request, "/registrationSensor");
        
        // Create JSON payload
        cJSON *root = cJSON_CreateObject();
        if (root == NULL) {
            LOG_ERR("Failed to create JSON object\n");
            PROCESS_EXIT();
        }
        cJSON_AddStringToObject(root, "s", "thermometer");

        cJSON *string_array = cJSON_CreateArray();
        if (string_array == NULL) {
            LOG_ERR("Failed to create JSON array\n");
         
            PROCESS_EXIT();
        }
        cJSON_AddItemToArray(string_array, cJSON_CreateString("temperature"));
        cJSON_AddItemToArray(string_array, cJSON_CreateString("humidity"));
        cJSON_AddItemToObject(root, "ss", string_array);
        cJSON_AddNumberToObject(root, "t", TIME_SAMPLE);
        
        char *payload = cJSON_PrintUnformatted(root);
        if (payload == NULL) {
            LOG_ERR("Failed to print JSON object\n");
            cJSON_Delete(root);
            PROCESS_EXIT();
        }
        coap_set_payload(request, (uint8_t *)payload, strlen(payload));
        

        printf("Sending the registration request\n");
        // Send the blocking request
        COAP_BLOCKING_REQUEST(&server_ep, request, client_chunk_handler);

        if (registered == 0) {
            LOG_INFO("Retry registration (%d/%d)\n", registration_retry_count, MAX_REGISTRATION_RETRY);
            registration_retry_count++;
            etimer_set(&prediction_timer, CLOCK_SECOND * 5); // Wait 5 seconds before retrying
            PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&prediction_timer));
        }
    }

    if (registered == 1) {
        //shutdown=0;
        leds_off(LEDS_RED);
        leds_single_off(LEDS_YELLOW);
        write_samples();
        printf(" a sample %f \n", samples[0].temperature);
        printf("Activate server term\n");
        //LOG_INFO("Starting Erbium Example Server\n");
        
        // Activate resources
        coap_activate_resource(&res_predict_temp, "predict-temp");
        coap_activate_resource(&res_monitoring_temp, "monitoring");
        coap_activate_resource(&res_shutdown, "shutdown");

        printf("CoAP server started\n");

        // Main loop
        etimer_set(&prediction_timer, CLOCK_SECOND * 10);
        etimer_set(&monitoring_timer, CLOCK_SECOND * 2);
   

        while (1) {

            PROCESS_YIELD();
            if(samples[0].humidity==-1 || shutdown==1){
                shutdown=1;
                printf("Shutdown incremented\n");
            
                process_exit(&thermometer_process);
                PROCESS_EXIT();

            }
            if (etimer_expired(&monitoring_timer)) {
                res_monitoring_temp.trigger();
                etimer_reset(&monitoring_timer);
            }
            if (etimer_expired(&prediction_timer)) {
                res_predict_temp.trigger();
                delete_samples();
                write_samples();
                etimer_reset(&prediction_timer);
            }
        }
        // mando notifica a tutti di spegnere
        printf("shutdown  %i\n", shutdown);

        

    } else {
        printf("Reached maximum number of registration attempts\n");
    }

} 


    PROCESS_END();
}