#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "contiki-net.h"
#include "coap-engine.h"
#include "coap-blocking-api.h"
#include "sys/etimer.h"

#if PLATFORM_SUPPORTS_BUTTON_HAL
#include "dev/button-hal.h"
#else
#include "dev/button-sensor.h"
#endif

/* Log configuration */
#include "coap-log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL  LOG_LEVEL_APP

#define SERVER_EP_TEMP "coap://[fd00::202:2:2:2]:5683"
#define SERVER_EP_LPG   "coap://[fd00::204:4:4:4]:5683"

static char* service_url = "predict-temp";

PROCESS(coap_client_process, "CoAP Client Process");
AUTOSTART_PROCESSES(&coap_client_process);

void response_handler(coap_message_t *response) {
    if(response == NULL) {
        printf("No response received.\n");
        return;
    }
    const uint8_t *chunk;
    int len = coap_get_payload(response, &chunk);
    printf("Response: |%.*s|\n", len, (char *)chunk);
}

PROCESS_THREAD(coap_client_process, ev, data)
{
    PROCESS_BEGIN();

    static coap_endpoint_t server_ep_temp;
    static coap_endpoint_t server_ep_lpg;
    static coap_message_t request[1];
    static struct etimer timer;

    coap_endpoint_parse(SERVER_EP_TEMP, strlen(SERVER_EP_TEMP), &server_ep_temp);
    coap_endpoint_parse(SERVER_EP_LPG,strlen(SERVER_EP_LPG),&server_ep_lpg);

    etimer_set(&timer, CLOCK_SECOND * 5);

    while(1) {
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));

        coap_init_message(request, COAP_TYPE_CON, COAP_GET, 0);
        coap_set_header_uri_path(request, service_url);

        printf("Sending request to %s\n", SERVER_EP_TEMP);

        COAP_BLOCKING_REQUEST(&server_ep_temp, request, response_handler);
        coap_init_message(request, COAP_TYPE_CON, COAP_GET, 0);
        coap_set_header_uri_path(request, "/res_danger");
        printf("Sending request to %s\n", SERVER_EP_LPG);
        COAP_BLOCKING_REQUEST(&server_ep_lpg, request, response_handler);

            // Reset the timer for the next cycle
            etimer_reset(&timer);

        etimer_reset(&timer);
    }

    PROCESS_END();
}