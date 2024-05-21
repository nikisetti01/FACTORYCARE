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

#define CRITICAL_TEMP_VALUE = 20; 

static char* service_url = "predict-temp";

PROCESS(coap_client_process, "CoAP Client Process");
AUTOSTART_PROCESSES(&coap_client_process);

static int lpgValue = 0;
static int tempValue = 0;

void response_handler(coap_message_t *response) {
    if(response == NULL) {
        printf("No response received.\n");
        return;
    }
    const uint8_t *chunk;
    int len = coap_get_payload(response, &chunk);
    printf("Response: |%.*s|\n", len, (char *)chunk);

    //lpg analysis
    if(strncmp((char *)chunk, "0", len) == 0) {
        printf("lpg sensor activity is normal\n");
        lpgValue = 0;
    } else if(strncmp((char *)chunk, "1", len) == 0) {
        printf("lpg sensor activity is high: DANGER\n");
        lpgValue = 1;
    } else if(strncmp((char *)chunk, "2", len) == 0) {
        printf("lpg sensor activity is extremely high: CRITIC\n");
        lpgValue = 2;
    } else {
        printf("Unknown response\n");
    }
    
    //temp analysis
    tempValue = atoi((char *)chunk);
    printf("Converted value: %d\n", tempValue);
    if(tempValue>CRITICAL_TEMP_VALUE){
        printf("Temperature is critical\n");
    }
    else{
        printf("Temperature is normal\n");
    }
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
    static struct etimer lpg_timer;

    while(1) {
      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));

      //first request to the temperature sensor
      coap_init_message(request, COAP_TYPE_CON, COAP_GET, 0);
      coap_set_header_uri_path(request, service_url);
      printf("Sending request to %s\n", SERVER_EP_TEMP);
      COAP_BLOCKING_REQUEST(&server_ep_temp, request, response_handler);

      //second request to the lpg sensor
      coap_init_message(request, COAP_TYPE_CON, COAP_GET, 0);
      coap_set_header_uri_path(request, "/res_danger");
      printf("Sending request to %s\n", SERVER_EP_LPG);
      COAP_BLOCKING_REQUEST(&server_ep_lpg, request, response_handler);

      // Reset the timer for the next cycle
      etimer_reset(&timer);

      if(lpgValue == 2) {
        // Activate the lpg timer
        etimer_set(&lpg_timer, CLOCK_SECOND);
      } else {
        // Deactivate the lpg timer
        etimer_stop(&lpg_timer);
      }

      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&lpg_timer));

      // Toggle the LED
      if(lpgValue == 2) {
        // Turn on the red LED
        printf("Turning on the red LED\n");

        

      } else {
        // Turn off the red LED
        printf("Turning off the red LED\n");
      }

      etimer_reset(&lpg_timer);
    }

    PROCESS_END();
}