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
<<<<<<< HEAD
#define LOG_LEVEL  LOG_LEVEL_DBG

#define SERVER_EP_TEMP "coap://[fd00::202:2:2:2]:5683"
#define SERVER_EP_LPG  "coap://[fd00::1]:5683"

#define CRITICAL_TEMP_VALUE 20

=======
#define LOG_LEVEL LOG_LEVEL_APP

#define SERVER_EP_TEMP "coap://[fd00::202:2:2:2]:5683"
#define SERVER_EP_LPG  "coap://[fd00::204:4:4:4]:5683" //TRY WITH DONGLE
//#define SERVER_EP_LPG   "coap://[fd00::203:3:3:3]:5683"

#define CRITICAL_TEMP_VALUE 20

static char* service_url_temp = "predict-temp";
static char* service_url_lpg = "res_danger";

>>>>>>> termobranch
PROCESS(coap_client_process, "CoAP Client Process");
AUTOSTART_PROCESSES(&coap_client_process);

static int lpgValue = 0;
<<<<<<< HEAD
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
=======
static float tempValue = 0;

void response_handler_LPG(coap_message_t *response) {
    printf("response_handler_LPG\n");
    if(response == NULL) {
        printf("No response received from LPG sensor.\n");
        return;
    }
    const uint8_t *chunk;
    int len = coap_get_payload(response, &chunk);
    char lpg_str[len + 1];
    strncpy(lpg_str, (char *)chunk, len);
    lpg_str[len] = '\0';
    lpgValue = atoi(lpg_str);
    printf("LPG Response: |%s| (Parsed: %d)\n", lpg_str, lpgValue);
}

void response_handler_TEMP(coap_message_t *response) {
    printf("response_handler_TEMP\n");
    if(response == NULL) {
        printf("No response received from temperature sensor.\n");
>>>>>>> termobranch
        return;
    }
    const uint8_t *chunk;
    int len = coap_get_payload(response, &chunk);
<<<<<<< HEAD
    printf("|%.*s\n", len, (char *)chunk);

    float value = atof((char *)chunk);
    tempValue = value;
    if(tempValue > CRITICAL_TEMP_VALUE) {
        printf("Temperature is critical\n");
    } else {
        printf("Temperature is normal\n");
=======
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
>>>>>>> termobranch
    }
}

PROCESS_THREAD(coap_client_process, ev, data)
{
    PROCESS_BEGIN();

    static coap_endpoint_t server_ep_temp;
    static coap_endpoint_t server_ep_lpg;
<<<<<<< HEAD
    static coap_message_t request[1];
    static struct etimer timer;
=======
    static struct etimer timer;
    static struct etimer lpg_timer;
>>>>>>> termobranch

    coap_endpoint_parse(SERVER_EP_TEMP, strlen(SERVER_EP_TEMP), &server_ep_temp);
    coap_endpoint_parse(SERVER_EP_LPG, strlen(SERVER_EP_LPG), &server_ep_lpg);

<<<<<<< HEAD
    etimer_set(&timer, CLOCK_SECOND);
    static struct etimer lpg_timer;

    while(1) {
        printf("Starting the client process\n");
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));

        // Request to the LPG sensor
        coap_init_message(request, COAP_TYPE_CON, COAP_GET, 0);
        coap_set_header_uri_path(request, "/hello");
        printf("Sending request to %s\n", SERVER_EP_LPG);
        COAP_BLOCKING_REQUEST(&server_ep_lpg, request, response_handler_LPG);

        // Reset the timer for the next cycle
        etimer_reset(&timer);

        printf("lpgValue: %d\n", lpgValue);
        if(lpgValue == 0) {
            printf("SETTING LED GREEN\n");
        }
        
        if(lpgValue == 1) {
            printf("SETTING LED RED\n");
        }

        if(lpgValue == 2) {
            etimer_set(&lpg_timer, CLOCK_SECOND);
            leds_single_toggle(LEDS_RED);
            printf("SETTING LED RED PULSING\n");
        }

        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&lpg_timer));

        if(tempValue > CRITICAL_TEMP_VALUE) {
            leds_on(LEDS_NUM_TO_MASK(LEDS_YELLOW));
        } else {
            leds_off(LEDS_NUM_TO_MASK(LEDS_YELLOW));
        }

        etimer_reset(&lpg_timer);
=======
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
>>>>>>> termobranch
    }

    PROCESS_END();
}
