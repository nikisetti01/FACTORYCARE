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
#define LOG_LEVEL  LOG_LEVEL_APP

#define SERVER_EP_TEMP "coap://[fd00::202:2:2:2]:5683"
#define SERVER_EP_LPG   "coap://[fd00::203:3:3:3]:5683"

#define CRITICAL_TEMP_VALUE 20

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

      // Check if the first word is "RES_DANGER" or "RES_TEMP"
      char firstWord[20];
      sscanf((char *)chunk, "%s", firstWord);

      if (strcmp(firstWord, "RES_DANGER:") == 0) {
        // Handle RES_DANGER response
        lpgValue = atoi((char *)(chunk + strlen(firstWord) + 1));
        printf("Converted value: %d\n", lpgValue);
        if(lpgValue == 0) {
          printf("LPG is normal\n");
        } else if(lpgValue == 1) {
          printf("LPG is high\n");
        } else if (lpgValue == 2){
          printf("LPG is critical\n");
        }else{
          printf("Unknown LPG value\n");
        }


      } else if (strcmp(firstWord, "RES_TEMP:") == 0) {
        // Handle RES_TEMP response
        tempValue = atoi((char *)(chunk + strlen(firstWord) + 1));
        printf("Converted value: %d\n", tempValue);
        if (tempValue > CRITICAL_TEMP_VALUE) {
          printf("Temperature is critical\n");
        } else {
          printf("Temperature is normal\n");
        }
      } else {
        printf("Unknown response\n");
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

      if(lpgValue == 0) {
        leds_on(LEDS_NUM_TO_MASK(LEDS_GREEN));
      }
      
      if(lpgValue == 1) {
        leds_on(LEDS_NUM_TO_MASK(LEDS_RED));
        leds_toggle(LEDS_NUM_TO_MASK(LEDS_GREEN));
      }

      if(lpgValue == 2) {
        etimer_set(&lpg_timer, CLOCK_SECOND*2);
        leds_on(LEDS_NUM_TO_MASK(LEDS_RED));
      }

      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&lpg_timer));

      // Toggle the LED
      if(lpgValue == 2) {
        leds_toggle(LEDS_NUM_TO_MASK(LEDS_RED));
      }

      if(tempValue>CRITICAL_TEMP_VALUE){
        leds_on(LEDS_NUM_TO_MASK(LEDS_YELLOW));
      }else
      {
        leds_off(LEDS_NUM_TO_MASK(LEDS_YELLOW));
      }

      etimer_reset(&lpg_timer);
    }

    PROCESS_END();
}