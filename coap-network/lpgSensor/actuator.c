#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "contiki-net.h"
#include "coap-engine.h"
#include "coap-blocking-api.h"
#include "sys/etimer.h"
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

#define CRITICAL_TEMP_VALUE 20

#define LOG_LEVEL LOG_LEVEL_APP
//TRY WITH DONGLE
//#define SERVER_EP_LPG   "coap://[fd00::203:3:3:3]:5683"

#define CRITICAL_TEMP_VALUE 20
#define SERVER_EP_JAVA "coap://[fd00::1]:5683"

static char* service_url_temp = "predict-temp";
static char* service_url_lpg = "res_danger";

PROCESS(coap_client_process, "CoAP Client Process");
AUTOSTART_PROCESSES(&coap_client_process);


static int lpgValue = 0;
static int tempValue = 0;



void response_handler_LPG(coap_message_t *response) {
    if(response==NULL) {
        printf("No response received.\n");
        return;
    }
    leds_single_off(LEDS_GREEN);
    leds_single_off(LEDS_RED);
    leds_single_off(LEDS_YELLOW);
    leds_single_off(LEDS_BLUE);

    const uint8_t *chunk;
    int len = coap_get_payload(response, &chunk);
    printf("|%.*s\n", len, (char *)chunk);

    int value = atoi((char *)chunk);
    lpgValue = value;
    if(lpgValue == 0) {
        printf("lpg is normal\n");
        leds_single_on(LEDS_GREEN);
        return;
    } else if(lpgValue == 1) {
        printf("lpg is dangerous\n");
        leds_single_on(LEDS_RED);
        return;
    } else if(lpgValue == 2) {
        printf("lpg is critical\n");
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
    printf("Temperature Response: |%s| (Parsed: %.2f)\n", temp_str, tempValue);
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

//declaring servers ip as global for parsing in the registration handler
static coap_endpoint_t server_ep_temp;
static coap_endpoint_t server_ep_lpg;

void response_handler_registration(coap_message_t *response) {
    if(response == NULL) {
        printf("No response received.\n");
        return;
    }
    const uint8_t *chunk;
    int len = coap_get_payload(response, &chunk);
    printf("|%.*s\n", len, (char *)chunk);

    char server_ep_temp_str[50];
    char server_ep_lpg_str[50];
    strncpy(server_ep_temp_str, (char *)chunk, len);
    server_ep_temp_str[len] = '\0';
    strncpy(server_ep_lpg_str, (char *)chunk + len + 1, len);
    server_ep_lpg_str[len] = '\0';
    coap_endpoint_parse(server_ep_temp_str, strlen(server_ep_temp_str), &server_ep_temp);
    coap_endpoint_parse(server_ep_lpg_str, strlen(server_ep_lpg_str), &server_ep_lpg);

    printf("Registration completed, ipv6 of servers:\n%s\n%s\n",server_ep_temp_str,server_ep_lpg_str);
}

PROCESS_THREAD(coap_client_process, ev, data)
{
    PROCESS_BEGIN();

    //creating registration request
    static coap_endpoint_t server_ep_java;

    static struct etimer timer;
    static struct etimer lpg_timer;

    coap_endpoint_parse(SERVER_EP_JAVA, strlen(SERVER_EP_JAVA), &server_ep_java);
    coap_init_message(request, COAP_TYPE_CON, COAP_POST, 0);
    coap_set_header_uri_path(request, service_url_reg, strlen(service_url_reg));
    //coap_set_header_content_format(request, TEXT_PLAIN);
    coap_set_payload(request, (uint8_t *)"actuator", 8);
    COAP_BLOCKING_REQUEST(&server_ep_java, request, response_handler_registration);

    //registration already sets the values of ips 
    //coap_endpoint_parse(SERVER_EP_TEMP, strlen(SERVER_EP_TEMP), &server_ep_temp);
    //coap_endpoint_parse(SERVER_EP_LPG, strlen(SERVER_EP_LPG), &server_ep_lpg);

    printf("Sending observation request to %s\n", SERVER_EP_TEMP);
    coap_obs_request_registration(&server_ep_temp, service_url_temp, handle_notification_temp, NULL);

    printf("Sending observation request to %s\n", SERVER_EP_LPG);
    coap_obs_request_registration(&server_ep_lpg, service_url_lpg, handle_notification_lpg, NULL);

    etimer_set(&timer, CLOCK_SECOND / 4);

    while(1) {
        PROCESS_WAIT_EVENT();

        if (ev == PROCESS_EVENT_TIMER && etimer_expired(&timer)) {
            // Poll the main process to handle the event
            process_poll(&coap_client_process);
        }

        // Manage the LED status based on the LPG value
        if (ev == PROCESS_EVENT_POLL) {
            if (lpgValue == 0) {
                leds_single_on(LEDS_GREEN);
            } else if (lpgValue == 1) {
                leds_single_on(LEDS_RED);
                leds_single_off(LEDS_NUM_TO_MASK(LEDS_GREEN));
            } else if (lpgValue == 2) {
                etimer_set(&lpg_timer, CLOCK_SECOND * 2);
                leds_single_on(LEDS_RED);
            }

            if (tempValue > CRITICAL_TEMP_VALUE) {
                leds_on(LEDS_NUM_TO_MASK(LEDS_YELLOW));
            } else {
                leds_off(LEDS_NUM_TO_MASK(LEDS_YELLOW));
            }

            // Toggle the LED for LPG level 2
            if (ev == PROCESS_EVENT_TIMER && etimer_expired(&lpg_timer)) {
                if (lpgValue == 2) {
                    leds_single_toggle(LEDS_NUM_TO_MASK(LEDS_RED));
                }
                etimer_reset(&lpg_timer);
            }
        }

        etimer_reset(&timer);
    }

    PROCESS_END();
}
