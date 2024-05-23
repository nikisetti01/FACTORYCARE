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
#define MAX_REGISTRATION_RETRY 3

#define SERVER_EP_TEMP "coap://[fd00::202:2:2:2]:5683"
#define SERVER_EP_LPG  "coap://[fe80::f6ce:36f4:9606:f869]:5683" //TRY WITH DONGLE
#define SERVER_EP_JAVA "coap://[fd00:1]:5836"
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
    leds_on(LEDS_GREEN);
    return;
  }else if(lpgValue == 1)
  {
    printf("lpg is dangerous\n");
    leds_on(LEDS_RED);
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
    printf("response_handler temp\n");
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

void response_handler_java(coap_message_t *response)
{
  printf("response_handler_java\n");
  if(response==NULL)
    {
        printf("No response received.\n");
        return;
    }

  const uint8_t *chunk;
  int len = coap_get_payload(response, &chunk);
  printf("|lpgSensor ipv6: %.*s\n", len, (char *)chunk);
  printf("Registration completed\n");
}


PROCESS_THREAD(coap_client_process, ev, data)
{
    PROCESS_BEGIN();
    printf("fanActuator starting\n");

    static coap_endpoint_t server_ep_java; //for registration

    static coap_endpoint_t server_ep_temp; 
    static coap_endpoint_t server_ep_lpg;
    static coap_message_t request[1];
    static struct etimer timer;

    coap_endpoint_parse(SERVER_EP_JAVA,strlen(SERVER_EP_JAVA),&server_ep_java); //for registration

    coap_endpoint_parse(SERVER_EP_TEMP, strlen(SERVER_EP_TEMP), &server_ep_temp);
    coap_endpoint_parse(SERVER_EP_LPG,strlen(SERVER_EP_LPG),&server_ep_lpg);

    etimer_set(&timer, CLOCK_SECOND); // * 5 è assai lento
    static struct etimer lpg_timer;


    while(max_registration_retry!=0){
		/* -------------- REGISTRATION --------------*/
		// Populate the coap_endpoint_t data structure
		coap_endpoint_parse(SERVER_EP, strlen(SERVER_EP), &server_ep);
		// Prepare the message
		coap_init_message(request, COAP_TYPE_CON,COAP_POST, 0);
		coap_set_header_uri_path(request, "registrationActuator");
		//Set payload
		coap_set_payload(request, (uint8_t *)NODE_NAME_JSON, sizeof(NODE_NAME_JSON) - 1);
	
		COAP_BLOCKING_REQUEST(&server_ep, request, client_chunk_handler);
    
		/* -------------- END REGISTRATION --------------*/
		if(max_registration_retry == -1){		// something goes wrong more MAX_REGISTRATION_RETRY times, node goes to sleep then try again
			etimer_set(&sleep_timer, 30*CLOCK_SECOND);
			PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&sleep_timer));
			max_registration_retry = MAX_REGISTRATION_RETRY;
		}
	}


    //registration
    coap_init_message(request, COAP_TYPE_CON, COAP_POST, 0);
    coap_set_header_uri_path(request, "register");
    printf("Sending registration request to %s\n", SERVER_EP_JAVA);

    COAP_BLOCKING_REQUEST(&server_ep_temp, request, response_handler_java);


    while(1) {
      printf("Starting the client process\n");
      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));

      //first request to the temperature sensor
      /*
      coap_init_message(request, COAP_TYPE_CON, COAP_GET, 0);
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
      if(lpgValue == 0) {
         printf("SETTING LED GREEN\n");
      }
      
      if(lpgValue == 1) {
        printf("SETTING LED RED\n");
      }

      if(lpgValue == 2) {
        etimer_set(&lpg_timer, CLOCK_SECOND);
        leds_toggle(LEDS_RED);
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