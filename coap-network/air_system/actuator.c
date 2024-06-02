#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "contiki-net.h"
#include "coap-engine.h"
#include "coap-blocking-api.h"
#include "sys/etimer.h"
#include "../cJSON-master/cJSON.h"
#include "leds.h"
#if PLATFORM_SUPPORTS_BUTTON_HAL
#include "dev/button-hal.h"
#else
#include "dev/button-sensor.h"
#endif

/* Log configuration */
#include "coap-log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL  LOG_LEVEL_DBG

#define SERVER_EP_TEMP "coap://[fd00::202:2:2:2]:5683"
#define SERVER_EP_LPG  "coap://[fd00::1]:5683"
#define SERVER_EP_JAVA "coap://[fd00::1]:5683"

#define CRITICAL_TEMP_VALUE 20

#define REGISTRATION_ATTEMPTS 5
#define REGISTRATION_DELAY 10 // in seconds

static char* service_url_temp = "/predict-temp";
static char* service_url_lpg = "/res-danger";
static char* service_url_reg = "/registrationActuator";
extern coap_resource_t res_tresh;
extern coap_resource_t res_shutdown;
static char ipv6temp[50];

static char ipv6lpg[50];
static int registration_attempts = 0;
static int registered = 0;
float temp_tresh=25;
int nRisktemp=0;
int nRisklpg=0;
PROCESS(coap_client_process, "CoAP Client Process");
AUTOSTART_PROCESSES(&coap_client_process);
static coap_observee_t *obs_temp = NULL;
static coap_observee_t *obs_lpg = NULL;
static int lpgValue = 0;
static int tempValue = 0;
static int shutdown=0;

void response_handler_LPG(coap_message_t *response) {
    if(response==NULL) {
        printf("No response received.\n");
        return;
    }

    const uint8_t *chunk;
    int len = coap_get_payload(response, &chunk);
    printf("|%.*s\n", len, (char *)chunk);
   

    int value = atoi((char *)chunk);
    lpgValue = value;
    printf("lpg valore: %d \n",value);
    if(lpgValue == 1) {
        printf("lpg is normal\n");
        leds_on(LEDS_GREEN);
    } else if(lpgValue == 2) {
        printf("lpg is dangerous\n");
        leds_on(LEDS_RED);
    } else if(lpgValue == 3) {
        nRisklpg++;
        printf("lpg is critical\n");
        leds_on(LEDS_RED);
    } else {
        printf("lpg is unknown\n");
    }
}

void response_handler_TEMP(coap_message_t *response) {
    printf("response_handler\n");
    if(response == NULL) {
        printf("No response received.\n");
        return;
    }
    const uint8_t *chunk;
    int len = coap_get_payload(response, &chunk);
    char temp_str[len + 1];
    strncpy(temp_str, (char *)chunk, len);
    temp_str[len] = '\0';
    tempValue = atof(temp_str);
   // printf("Temperature Response: |%s| (Parsed: %.2f)\n", temp_str, tempValue);
}

void handle_notification_temp(struct coap_observee_s *observee, void *notification, coap_notification_flag_t flag) {
    coap_message_t *msg = (coap_message_t *)notification;
    if (msg) {
        printf("Received temperature notification\n");
        response_handler_TEMP(msg);
        process_poll(&coap_client_process); // Poll the main process to handle the event
    } else {
        printf("No temperature notification received\n");
    }
}

void handle_notification_lpg(struct coap_observee_s *observee, void *notification, coap_notification_flag_t flag) {
    coap_message_t *msg = (coap_message_t *)notification;
    if (msg) {
        printf("Received LPG notification\n");
        response_handler_LPG(msg);
        process_poll(&coap_client_process); // Poll the main process to handle the event
    } else {
        printf("No LPG notification received\n");
    }
}

void registration_handler(coap_message_t *response) {
    if (response == NULL) {
        printf("No response received.\n");
        return;
    }
    const uint8_t *chunk;
    int len = coap_get_payload(response, &chunk);
    char payload[len + 1];
    strncpy(payload, (char *)chunk, len);
    payload[len] = '\0';
    printf("Registration payload: %s\n", payload);

    // Assume payload contains IPv6 addresses in JSON format
    cJSON *json = cJSON_Parse(payload);
    if (json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            fprintf(stderr, "JSON parsing error: %s\n", error_ptr);
        }
        return;
    }

    cJSON *ipv6temp_item = cJSON_GetObjectItemCaseSensitive(json, "t");
    cJSON *ipv6lpg_item = cJSON_GetObjectItemCaseSensitive(json, "l");



    if (cJSON_IsString(ipv6temp_item) && cJSON_IsString(ipv6lpg_item)) {
        char full_ipv6temp[50];
        char full_ipv6lpg[50];
        snprintf(full_ipv6temp, sizeof(full_ipv6temp), "fd00:0:0:0:%s", ipv6temp_item->valuestring);
        snprintf(full_ipv6lpg, sizeof(full_ipv6lpg), "fd00:0:0:0:%s", ipv6lpg_item->valuestring);

        strncpy(ipv6temp, full_ipv6temp, sizeof(ipv6temp) - 1);
        ipv6temp[sizeof(ipv6temp) - 1] = '\0';

        strncpy(ipv6lpg, full_ipv6lpg, sizeof(ipv6lpg) - 1);
        ipv6lpg[sizeof(ipv6lpg) - 1] = '\0';

        printf("Registered IPv6temp: %s\n", ipv6temp);
        printf("Registered IPv6lpg: %s\n", ipv6lpg);

        registered = 1;
    } else {
        printf("Invalid JSON format or missing keys\n");
    }
 
}

PROCESS_THREAD(coap_client_process, ev, data) {
    PROCESS_BEGIN();
    static coap_endpoint_t server_ep_java;
    static coap_message_t request[1];
    static struct etimer registration_timer;
    static struct etimer main_timer;
    static struct etimer red_timer;

    coap_endpoint_parse(SERVER_EP_JAVA, strlen(SERVER_EP_JAVA), &server_ep_java);

    while (registration_attempts < REGISTRATION_ATTEMPTS && !registered) {
        coap_init_message(request, COAP_TYPE_CON, COAP_POST, 0);
        coap_set_header_uri_path(request, service_url_reg);
        coap_set_payload(request, (uint8_t *)"actuator", strlen("actuator"));

        printf("Sending registration request...\n");
        COAP_BLOCKING_REQUEST(&server_ep_java, request, registration_handler);

        if (!registered) {
            registration_attempts++;
            etimer_set(&registration_timer, CLOCK_SECOND * REGISTRATION_DELAY);
            PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&registration_timer));
        }
    }

    if (registered) {

        if(lpgValue==3)
        {
            leds_toggle(LEDS_RED);
        }
        if(etimer_expired(&red_timer))
        {
            leds_toggle(LEDS_RED);
            etimer_reset(&red_timer);
        }


        static coap_endpoint_t server_ep_temp;
        static coap_endpoint_t server_ep_lpg;

        char addr_temp[100] = "coap://[";
        char addr_lpg[100] = "coap://[";
        strcat(addr_temp, ipv6temp);
        strcat(addr_temp, "]:5683");
        strcat(addr_lpg, ipv6lpg);
        strcat(addr_lpg, "]:5683");

        coap_endpoint_parse(addr_temp, strlen(addr_temp), &server_ep_temp);
        coap_endpoint_parse(addr_lpg, strlen(addr_lpg), &server_ep_lpg);

        printf("Sending observation request to %s\n", addr_temp);
        obs_temp=coap_obs_request_registration(&server_ep_temp, service_url_temp, handle_notification_temp, NULL);

        printf("Sending observation request to %s\n", addr_lpg);
        obs_lpg=coap_obs_request_registration(&server_ep_lpg, service_url_lpg, handle_notification_lpg, NULL);
        coap_activate_resource(&res_tresh, "threshold");
        coap_activate_resource(&res_shutdown, "shutdown");

        etimer_set(&main_timer, CLOCK_SECOND * 2);
        //shutdown=0;
        while (1) {
            PROCESS_WAIT_EVENT();
            if(temp_tresh==-1 || shutdown==1){
                shutdown=1;
                printf("Shutdown incremented\n");
                temp_tresh=25;
                 if (obs_temp != NULL) {
                   coap_obs_remove_observee(obs_temp);
                }
                if (obs_lpg != NULL) {
                    coap_obs_remove_observee(obs_lpg);
                }
                process_exit(&coap_client_process);
                PROCESS_EXIT();

            }
            if (ev == PROCESS_EVENT_TIMER && etimer_expired(&main_timer)) {
                process_poll(&coap_client_process);
                etimer_reset(&main_timer);

            }
        }
       
    } else {
        printf("Failed to register after %d attempts\n", REGISTRATION_ATTEMPTS);
    }

    PROCESS_END();
}
