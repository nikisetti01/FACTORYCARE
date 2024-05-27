#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "contiki-net.h"
#include "coap-engine.h"
#include "coap-blocking-api.h"
#include "sys/etimer.h"
#include "os/dev/leds.h"
#include "../cJSON-master/cJSON.h"
#define GOOD_ACK 65
#if PLATFORM_SUPPORTS_BUTTON_HAL
#include "dev/button-hal.h"
#else
#include "dev/button-sensor.h"
#endif

/* Log configuration */
#include "coap-log.h"

#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_APP
#define STARTING_WATER 0.2
#define SERVER_EP_APP "coap://[fd00::1]:5683"

#define MAX_REGISTRATION_ENTRY 3
static char ipv6temp[40];
static char ipv6lpg[40];
static char *service_url_temp= "predict-temp";
static char *service_url_lpg= "res-danger";
static float next_temperature=0;
static int lpg_level;
static int led_now = -1;
static float last_temperature;
static float k_temp=0.1;
static float k_lpg=0.2;
static float water=0;
static int registered=0;
static int registration_retry_count=0;
PROCESS(coap_client_process, "CoAP Client Process");
AUTOSTART_PROCESSES(&coap_client_process);
float update_water_production_temperature(float water, float last_temperature, float next_temperature) {
    float diff;
    if (last_temperature < 20)
        water = STARTING_WATER;
    else
        diff = next_temperature - last_temperature;
    water += diff * k_temp;
    return water;
}

float update_water_production_lpg() {
    water = STARTING_WATER + water * water * k_lpg;
    return water;
}
void registration_handler(coap_message_t* response){
      const uint8_t *chunk;

    if (response == NULL) {
        LOG_ERR("Request timed out\n");
        return;
    }

    int len = coap_get_payload(response, &chunk);
    char payload[len + 1];
    memcpy(payload, chunk, len);
    payload[len] = '\0';  // Ensure null-terminated string
    printf("payload : %s \n", payload);

    if (response->code == GOOD_ACK) {
        printf("Registration successful\n");
        registered = 1;
        // recieved the payload back 
       cJSON *json = cJSON_Parse(payload);
        if (json == NULL) {
            const char *error_ptr = cJSON_GetErrorPtr();
            if (error_ptr != NULL) {
                fprintf(stderr, "JSON parsing error: %s\n", error_ptr);
            }
            return;
        }
        
        // Extract the IPv6 addresses
        cJSON *ipv6temp_item = cJSON_GetObjectItemCaseSensitive(json, "t");
        cJSON *ipv6lpg_item = cJSON_GetObjectItemCaseSensitive(json, "l");
        

           
        if (cJSON_IsString(ipv6temp_item)  &&
            cJSON_IsString(ipv6lpg_item) ) {
            

            strncpy(ipv6temp, ipv6temp_item->valuestring, sizeof(ipv6temp) - 1);
            strncpy(ipv6lpg, ipv6lpg_item->valuestring, sizeof(ipv6lpg) - 1);

            // Ensure null termination
            ipv6temp[sizeof(ipv6temp) - 1] = '\0';
            ipv6lpg[sizeof(ipv6lpg) - 1] = '\0';

            printf("IPv6temp: %s\n", ipv6temp);
            printf("IPv6lpg: %s\n", ipv6lpg);
               } else {
            printf("Invalid JSON format or missing keys\n");
        }

    } else {
        printf("Registration failed\n");
    }
    


}



void response_handler_temp(coap_message_t *response )
{
    if (response == NULL)
    {
        printf("No response received from temperature sensor.\n");
        return;
    }
    const uint8_t *chunk;
    int len = coap_get_payload(response, &chunk);
    char temp_str[len + 1];
    strncpy(temp_str, (char *)chunk, len);
    temp_str[len] = '\0';
    last_temperature = next_temperature;
    next_temperature = strtof(temp_str, NULL);
    printf("Temperature Response: |%.*s| (Parsed: %.2f)\n", len, (char *)chunk, next_temperature);
}

void handle_notification_temp(struct coap_observee_s *observee, void *notification, coap_notification_flag_t flag) {
    coap_message_t *msg = (coap_message_t *)notification;
    if (msg) {
        printf("Received temperature notification\n");
        response_handler_temp(msg);
        process_poll(&coap_client_process); // Poll the main process to handle the event
    } else {
        printf("No temperature notification received\n");
    }
}

void response_handler_lpg(coap_message_t *response) {
    if (response == NULL) {
        printf("No response received from LPG sensor.\n");
        return;
    }
    const uint8_t *chunk;
    int len = coap_get_payload(response, &chunk);
    char lpg_str[len + 1];
    strncpy(lpg_str, (char *)chunk, len);
    lpg_str[len] = '\0';
    lpg_level = atoi(lpg_str); // Assumendo che il livello LPG sia un numero intero (0, 1, 2)
    printf("LPG Response: |%s| (Parsed: %d)\n", lpg_str, lpg_level);
}

void handle_notification_lpg(struct coap_observee_s *observee, void *notification, coap_notification_flag_t flag) {
    coap_message_t *msg = (coap_message_t *)notification;
    if (msg) {
        printf("Received LPG notification\n");
        response_handler_lpg(msg);
        process_poll(&coap_client_process); // Poll the main process to handle the event
    } else {
        printf("No LPG notification received\n");
    }
}

PROCESS_THREAD(coap_client_process, ev, data)
{
    PROCESS_BEGIN();
    static coap_endpoint_t server_ep_app;
    static struct etimer prediction_timer;

    static coap_endpoint_t server_ep_temp;
    static coap_endpoint_t server_ep_lpg;
    static struct etimer ledtimer;
    static coap_message_t request[1];
    coap_endpoint_parse(SERVER_EP_APP, strlen(SERVER_EP_APP),&server_ep_app);
    while (registration_retry_count<MAX_REGISTRATION_ENTRY && registered==0)
    {
        coap_init_message(request,COAP_TYPE_CON, COAP_POST, 0);
        coap_set_header_uri_path(request, "/registrationActuator");
        char*payload="sprinkler";
        coap_set_payload(request,(uint8_t*)payload, strlen(payload));
        printf("Sending the registration request \n");
        COAP_BLOCKING_REQUEST(&server_ep_app,request,registration_handler);

         if (registered==0) {
            LOG_INFO("Retry registration (%d/%d)\n", registration_retry_count, MAX_REGISTRATION_ENTRY);
            etimer_set(&prediction_timer, CLOCK_SECOND * 10); // Wait 10 seconds before retrying
            PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&prediction_timer));
        }
    }
    if(registered==1){
        char addr_temp[50] = "coap://";
        char addr_lpg[50] = "coap://";
        strcat(addr_temp,ipv6temp);
        strcat(addr_temp,":5683");
        strcat(addr_lpg,ipv6lpg);
        strcat(addr_lpg,":5683"); 
        


   // coap_endpoint_parse(addr_temp, strlen(addr_temp), &server_ep_temp);
   // coap_endpoint_parse(addr_lpg, strlen(addr_lpg), &server_ep_lpg);

    printf("Sending observation request to %s\n",addr_temp);
    coap_obs_request_registration(&server_ep_temp, service_url_temp, handle_notification_temp, NULL);

    printf("Sending observation request to %s\n", ipv6lpg);
    coap_obs_request_registration(&server_ep_lpg, service_url_lpg, handle_notification_lpg, NULL);

    etimer_set(&ledtimer, 2 * CLOCK_SECOND); // Imposta il timer del LED a 2 secondi per iniziare

    while (1)
    {
        PROCESS_WAIT_EVENT(); // Attendiamo un evento

        // Gestione del timer del LED
        if (ev == PROCESS_EVENT_TIMER && etimer_expired(&ledtimer))
        {
            if (lpg_level == 2) {
                water=update_water_production_lpg();
                printf("on for lpg\n");
                printf("Rilascio %f di acqua\n", water);
                // Lampeggio del LED rosso se il livello di LPG è alto
                if (led_now == LEDS_RED) {
                    leds_single_off(LEDS_RED);
                } else {
                    leds_single_on(LEDS_RED);
                    led_now = LEDS_RED;
                }
                etimer_reset(&ledtimer); // Reset del timer del LED
            }
        }

        // Gestione delle notifiche
        if (ev == PROCESS_EVENT_POLL) {
            // Logica basata sulla temperatura e sul livello LPG
            if (next_temperature < 20 && lpg_level != 2) {
                leds_single_off(led_now);
                led_now = LEDS_GREEN;
                leds_single_on(led_now);
                printf("Sprinkler off\n");
            } else if (next_temperature >= 20 && lpg_level != 2) {
                leds_single_off(led_now);
                led_now = LEDS_BLUE;
                leds_single_on(led_now);
                printf("on for temperature\n");
                water = update_water_production_temperature(water, last_temperature, next_temperature);
                printf("Rilascio %f di acqua\n", water);
            }
        }
    }
    } else {
        printf("Problem for registration");
    }

    PROCESS_END();
}
