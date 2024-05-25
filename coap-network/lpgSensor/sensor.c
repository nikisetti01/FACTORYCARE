#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "coap-engine.h"
#include "resources/res_danger.c"
//for ipv6
#include "net/ipv6/uip-ds6.h"
#include "net/ipv6/uip-ds6-route.h"
#include "net/ipv6/uip-ds6-nbr.h"
#include "net/ipv6/uip-nd6.h"
#include "net/ipv6/uip-icmp6.h"
#include "leds.h"
#include "coap-blocking-api.h"

#define SERVER_EP_JAVA "coap://[fd00::1]:5683"

#if PLATFORM_SUPPORTS_BUTTON_HAL
#include "dev/button-hal.h"
#else
#include "dev/button-sensor.h"
#endif

/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_RPL
#define MAX_REGISTRATION_RETRY 3
static int max_registration_retry = MAX_REGISTRATION_RETRY;

//static char* service_url_reg = "/registrationSensor";

extern coap_resource_t
  res_danger;

coap_message_t request[1];  
static struct etimer sleep_timer;


void response_handler_registration(coap_message_t*response){
  
  leds_off(LEDS_RED);
  leds_single_off(LEDS_YELLOW);

  if(response==NULL){
    printf("No response received. %d\n",max_registration_retry);
    max_registration_retry--;
	  if(max_registration_retry==0)
		  max_registration_retry=-1;
    return;
  }
  //registration worked, so we can stop the retry
  const uint8_t *chunk;
  int len=coap_get_payload(response, &chunk);
  printf("|: %.*s\n", len, (char *)chunk);
  max_registration_retry = 0;
  printf("Registration completed\n");

}
PROCESS(lpgSensorServer, "lpgSensor Server");
AUTOSTART_PROCESSES(&lpgSensorServer);

PROCESS_THREAD(lpgSensorServer, ev, data)
{
  static struct etimer timer;
  static coap_endpoint_t server_ep_java;

  PROCESS_BEGIN();

  while(max_registration_retry!=0){
    leds_on(LEDS_RED);
    leds_single_on(LEDS_YELLOW);

		/* -------------- REGISTRATION --------------*/
		// Populate the coap_endpoint_t data structure
		coap_endpoint_parse(SERVER_EP_JAVA, strlen(SERVER_EP_JAVA), &server_ep_java);
		// Prepare the message
		coap_init_message(request, COAP_TYPE_CON,COAP_POST, 0);
		coap_set_header_uri_path(request, "/hello");//service_url_reg);
		//Set payload
		//coap_set_payload(request, (uint8_t *)"sensor", sizeof("sensor") - 1);
	
    printf("Tentativo di registrazione a %s\n",SERVER_EP_JAVA);

		COAP_BLOCKING_REQUEST(&server_ep_java, request, response_handler_registration);
    
		/* -------------- END REGISTRATION --------------*/
		if(max_registration_retry == -1){		// something goes wrong more MAX_REGISTRATION_RETRY times, node goes to sleep then try again
			etimer_set(&sleep_timer, 30*CLOCK_SECOND);
			PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&sleep_timer));
			max_registration_retry = MAX_REGISTRATION_RETRY;
		}
	}

  printf("lpgSensorServer\n");
  coap_activate_resource(&res_danger, "res_danger");
  //LOG_INFO("Risorsa avviata\n");
  printf("Risorsa avviata\n");

    etimer_set(&timer, CLOCK_SECOND * 10);

    while(1) {

      PROCESS_WAIT_EVENT();
      if(etimer_expired(&timer)){
        
      res_danger.trigger();
      printf("notifico il danger");
      etimer_reset(&timer);
      }
      
    }

    PROCESS_END();
}