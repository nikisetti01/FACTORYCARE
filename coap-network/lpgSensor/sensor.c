#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "coap-engine.h"
#include "random.h"
//for ipv6
#include "net/ipv6/uip-ds6.h"
#include "net/ipv6/uip-ds6-route.h"
#include "net/ipv6/uip-ds6-nbr.h"
#include "net/ipv6/uip-nd6.h"
#include "net/ipv6/uip-icmp6.h"
#include "leds.h"
#include "coap-blocking-api.h"
#include "global_variable/global_variables.h"
#include "../cJSON-master/cJSON.h"
#include "os/dev/button-hal.h"
#define TIME_SAMPLE 5
#define SERVER_EP_JAVA "coap://[fd00::1]:5683"
#define GOOD_ACK 65

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

static char* service_url_reg = "/registrationSensor";
static int registered = 0;
extern coap_resource_t res_danger;
extern coap_resource_t res_monitoring_lpg;
extern coap_resource_t res_shutdown;
 Sample sample;

static int shutdown=0;

// scrittura del singolo sample con timeid co,smoke, light,humidity casuale ma sensato
void write_sample() {
    printf("WRITE SAMPLE\n");
    sample.co = random_rand() % 100;
    sample.smoke = random_rand() % 100;
    sample.light = random_rand() % 2;
    sample.humidity = random_rand() % 100;
    
    cJSON *root = cJSON_CreateObject();
    cJSON*ss=cJSON_CreateArray();
    // cJSON_AddItemToObject(root,"id",cJSON_CreateNumber(count));
    cJSON_AddItemToArray(ss,cJSON_CreateNumber(sample.co));
    cJSON_AddItemToArray(ss,cJSON_CreateNumber(sample.smoke));
    cJSON_AddItemToArray(ss,cJSON_CreateBool(sample.light));
    cJSON_AddItemToArray(ss,cJSON_CreateNumber(3)); //prediction
    cJSON_AddItemToObject(root,"ss",ss);

}
coap_message_t request[1];  
static struct etimer sleep_timer;


void client_chunk_handler(coap_message_t *response) {
    printf("client_chunk_handler\n");
    const uint8_t *chunk;

    if (response == NULL) {
        LOG_ERR("Request timed out\n");
        printf("Request timed out\n");
        return;
    }
    int len = coap_get_payload(response, &chunk);
    char payload[len + 1];
    memcpy(payload, chunk, len);
    payload[len] = '\0';  // Ensure null-terminated string
    printf("Response: %s\n", payload);
    if (response->code == GOOD_ACK) {
        printf("Registration successful\n");
        registered = 1;
    } else {
        printf("Registration failed\n");
    }
}

PROCESS(lpgSensorServer, "lpgSensor Server");
AUTOSTART_PROCESSES(&lpgSensorServer);

PROCESS_THREAD(lpgSensorServer, ev, data)
{
  static struct etimer timer;
  static coap_endpoint_t server_ep_java;
   int pressed=0;

  PROCESS_BEGIN();
while(ev != button_hal_press_event || pressed==0) {
        pressed=1;
        PROCESS_YIELD();

  while(max_registration_retry!=0 && registered==0){
    leds_on(LEDS_RED);
    leds_single_on(LEDS_YELLOW);

		/* -------------- REGISTRATION --------------*/
		// Populate the coap_endpoint_t data structure
    coap_endpoint_parse(SERVER_EP_JAVA, strlen(SERVER_EP_JAVA), &server_ep_java);
    printf("Endpoint parsed: %s\n", SERVER_EP_JAVA);

    coap_init_message(request, COAP_TYPE_CON, COAP_POST, 0 );
    printf("Message initialized\n");

    coap_set_header_uri_path(request, service_url_reg);
    printf("Header URI path set to: %s\n", service_url_reg);

    // Create JSON payload
    cJSON *root = cJSON_CreateObject();
    if (root == NULL) {
      LOG_ERR("Failed to create JSON object\n");
      PROCESS_EXIT();
    }
    cJSON_AddStringToObject(root, "s", "lpgSensor");
    printf("JSON object created\n");
    //cJSON_AddStringToObject(root, "ipv6", "[fd00::202:2:2:2]");
    
     cJSON *string_array = cJSON_CreateArray();
    if (string_array == NULL) {
        LOG_ERR("Failed to create JSON array\n");
      
        PROCESS_EXIT();
    }
    cJSON_AddItemToArray(string_array, cJSON_CreateString("ts"));
    cJSON_AddItemToArray(string_array, cJSON_CreateString("co"));
    cJSON_AddItemToArray(string_array, cJSON_CreateString("smoke"));
    cJSON_AddItemToArray(string_array, cJSON_CreateString("light"));
    cJSON_AddItemToArray(string_array, cJSON_CreateString("pr")); //prediction
    cJSON_AddItemToObject(root, "ss", string_array);
    cJSON_AddNumberToObject(root, "t", TIME_SAMPLE);

    char *payload = cJSON_PrintUnformatted(root);
    if (payload == NULL) {
      LOG_ERR("Failed to print JSON object\n");
      PROCESS_EXIT();
    }
    coap_set_payload(request, (uint8_t *)payload, strlen(payload));
    //printf("il payload %s  lenght  %ld \n",payload, strlen(payload));


		//Set payload
		//coap_set_payload(request, (uint8_t *)"sensor", sizeof("sensor") - 1);
	
    printf("Tentativo di registrazione a %s\n",SERVER_EP_JAVA);
    coap_set_payload(request, (uint8_t *)payload, strlen(payload));
    printf("Sending the registration request\n");
		COAP_BLOCKING_REQUEST(&server_ep_java, request, client_chunk_handler);
    
		/* -------------- END REGISTRATION --------------*/
		if(max_registration_retry == -1){		// something goes wrong more MAX_REGISTRATION_RETRY times, node goes to sleep then try again
			etimer_set(&sleep_timer, 30*CLOCK_SECOND);
			PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&sleep_timer));
			max_registration_retry = MAX_REGISTRATION_RETRY;
		}
	}
  if(registered==1){
    printf("Registration successful\n");
    leds_off(LEDS_RED);
    leds_single_off(LEDS_YELLOW);
    
            //shutdown=0;
  printf("lpgSensorServer\n");
  write_sample();
  coap_activate_resource(&res_danger, "res-danger");
  coap_activate_resource(&res_monitoring_lpg, "monitoring");
  coap_activate_resource(&res_shutdown, "shutdown");


    etimer_set(&timer, CLOCK_SECOND * 10);
    while(1) {


      PROCESS_WAIT_EVENT();
       if(sample.co>-1 || shutdown==1){
                shutdown=1;
                printf("Shutdown incremented\n");
          
                process_exit(&lpgSensorServer);
                PROCESS_EXIT();

            }
      if(etimer_expired(&timer)){
      res_monitoring_lpg.trigger();
      res_danger.trigger();
      printf("notifico il danger");
      write_sample();
      etimer_reset(&timer);
      }
      printf("shutdown sensorc:%d\n",shutdown);
      
    }
    printf("shutdown:%d\n",shutdown);
  }
}
    PROCESS_END();

}