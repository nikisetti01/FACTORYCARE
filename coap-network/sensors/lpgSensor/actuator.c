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
#define SERVER_EP_LPG  "coap://[fe80::f6ce:36f4:9606:f869]:5683" //TRY WITH DONGLE
//#define SERVER_EP_LPG   "coap://[fd00::203:3:3:3]:5683"

#define CRITICAL_TEMP_VALUE 20

//static char* service_url = "predict-temp";

PROCESS(coap_client_process, "CoAP Client Process");
AUTOSTART_PROCESSES(&coap_client_process);

static int lpgValue = 0;
static int tempValue = 0;

void response_handler_LPG(coap_message_t *response) {
    if(response==NULL)
    {
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
  if(lpgValue == 0)
  {
    printf("lpg is normal\n");
    leds_single_on(LEDS_GREEN);
    return;
  }else if(lpgValue == 1)
  {
    printf("lpg is dangerous\n");
    leds_single_on(LEDS_RED);
    return;
  }else if(lpgValue == 2)
  {
    printf("lpg is critical\n");  

  }else
  {
    printf("lpg is unknown\n");
  }
}

void response_handler_TEMP(coap_message_t *response) {
    printf("response_handler\n");
    if(response==NULL)
    {
        printf("No response received.\n");
        return;
    }
  const uint8_t *chunk;

  int len = coap_get_payload(response, &chunk);
  printf("|%.*s\n", len, (char *)chunk);

  float value = atof((char *)chunk);
  tempValue = value;
  if(tempValue > CRITICAL_TEMP_VALUE)
  {
    printf("Temperature is critical");
  }else
  {
    printf("Temperature is normal");
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

    etimer_set(&timer, CLOCK_SECOND); // * 5 è assai lento
    static struct etimer lpg_timer;

    while(1) {
      printf("Starting the client process\n");
      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));

      //first request to the temperature sensor
      /*coap_init_message(request, COAP_TYPE_CON, COAP_GET, 0);
      coap_set_header_uri_path(request, service_url);
      printf("Sending request to %s\n", SERVER_EP_TEMP);
      COAP_BLOCKING_REQUEST(&server_ep_temp, request, response_handler_TEMP); //modifica creando risposta per temp e lpg in modo distinto
      */

      //second request to the lpg sensor
      coap_init_message(request, COAP_TYPE_CON, COAP_GET, 0);
      coap_set_header_uri_path(request, "/res_danger");
      printf("Sending request to %s\n", SERVER_EP_LPG);
      COAP_BLOCKING_REQUEST(&server_ep_lpg, request, response_handler_LPG);

      // Reset the timer for the next cycle
      etimer_reset(&timer);

      printf("lpgValue: %d\n", lpgValue);
      if(lpgValue == 0) {//PROVO A INVERTIRE, CORRETTAMENTE NON VA
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