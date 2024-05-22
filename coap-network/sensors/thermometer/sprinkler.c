#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "contiki-net.h"
#include "coap-engine.h"
#include "coap-blocking-api.h"
#include "sys/etimer.h"
#include "os/dev/leds.h"

#if PLATFORM_SUPPORTS_BUTTON_HAL
#include "dev/button-hal.h"
#else
#include "dev/button-sensor.h"
#endif

/* Log configuration */
#include "coap-log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_APP
#define STARTING_WATER 0.2
#define SERVER_EP_TEMP "coap://[fd00::202:2:2:2]:5683"
#define SERVER_EP_LPG "coap://[fd00::204:4:4:4]:5683"

static char *service_url = "predict-temp";
static float next_temperature=0;
static int lpg_level;
static int led_now = -1;
static float last_temperature;
static float k_temp=0.1;
static float k_temp=0.2;
static float water=0;
PROCESS(coap_client_process, "CoAP Client Process");
AUTOSTART_PROCESSES(&coap_client_process);
void update_water_production_temperature(float &water, float last_temperature, float next_temperature){
    if(last_temperature<20)
    water=STARTING_WATER;
    else
    diff=new_temperaure- last_temperature;
    water+=diff*k_temp;



}
void update_water_production_lpg(float &water, float last_temperature, float next_temperature){
   water=STARTING_WATER+water*water*k_lpg;

}
void response_handler_temp(coap_message_t *response)
{
    if (response == NULL)
    {
        printf("No response received from temperature sensor.\n");
        return;
    }
    const uint8_t *chunk;
    int len = coap_get_payload(response, &chunk);
    char temp_str[len + 1];
    strncpy(temp_str, (char *)chunk, len);
    temp_str[len] = '\0';
    last_temperature=next_temperature;
    next_temperature = strtof(temp_str, NULL);
    printf("Temperature Response: |%.*s| (Parsed: %.2f)\n", len, (char *)chunk, next_temperature);
}

void response_handler_lpg(coap_message_t *response)
{
    if (response == NULL)
    {
        printf("No response received from LPG sensor.\n");
        return;
    }
    const uint8_t *chunk;
    int len = coap_get_payload(response, &chunk);
    char lpg_str[len + 1];
    strncpy(lpg_str, (char *)chunk, len);
    lpg_str[len] = '\0';
    lpg_level = atoi(lpg_str); // Assumendo che il livello LPG sia un numero intero (0, 1, 2)
    printf("LPG Response: |%s| (Parsed: %d)\n", lpg_str, lpg_level);
}

PROCESS_THREAD(coap_client_process, ev, data)
{
    PROCESS_BEGIN();

    static coap_endpoint_t server_ep_temp;
    static coap_endpoint_t server_ep_lpg;
    static coap_message_t request[1];
    static struct etimer timer;
    static struct etimer ledtimer;

    coap_endpoint_parse(SERVER_EP_TEMP, strlen(SERVER_EP_TEMP), &server_ep_temp);
    coap_endpoint_parse(SERVER_EP_LPG, strlen(SERVER_EP_LPG), &server_ep_lpg);

    etimer_set(&timer, CLOCK_SECOND * 5);
    etimer_set(&ledtimer, CLOCK_SECOND); // Imposta il timer del LED a 1 secondo per iniziare

    while (1)
    {
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));

        // Request per il sensore di temperatura
        coap_init_message(request, COAP_TYPE_CON, COAP_GET, 0);
        coap_set_header_uri_path(request, service_url);
        printf("Sending request to %s\n", SERVER_EP_TEMP);
        COAP_BLOCKING_REQUEST(&server_ep_temp, request, response_handler_temp);

        // Request per il sensore LPG
        coap_init_message(request, COAP_TYPE_CON, COAP_GET, 0);
        coap_set_header_uri_path(request, "/res_danger");
        printf("Sending request to %s\n", SERVER_EP_LPG);
        COAP_BLOCKING_REQUEST(&server_ep_lpg, request, response_handler_lpg);

        // Logica basata sulla temperatura e sul livello LPG
        if (next_temperature < 20 && lpg_level != 2)
        {
            leds_single_off(led_now);
            led_now = LEDS_GREEN;
            leds_single_on(led_now);
            printf("Sprinkler off\n");
        }
        else if (next_temperature >= 20 && lpg_level != 2)
        {
            leds_single_off(led_now);
            led_now = LEDS_BLUE;
            leds_single_on(led_now);
            printf("on for temperature");
            update_water_production_temperature(&water,last_temperature,next_temperature);
            printf("Rilascio %f di acqua", water);
        }
        else if (lpg_level == 2)
        {   printf("on for lpg");
            update_water_production_lpg(&water);
            printf("Rilascio %f di acqua", water);
            // Lampeggia il LED rosso quando il livello LPG è 2
            if (etimer_expired(&ledtimer))
            {
                // Inverti lo stato del LED rosso
                if (led_now == LEDS_RED)
                {
                    leds_single_off(LEDS_RED);
                   // led_now = LEDS_NONE; Imposta il LED a NONE per evitare che sia acceso senza controllo
                }
                else
                {
                    leds_single_on(LEDS_RED);
                    led_now = LEDS_RED;
                }
                etimer_reset(&ledtimer); // Resetta il timer del LED per farlo lampeggiare ogni secondo
            }
        }

        // Resetta il timer per il prossimo ciclo
        etimer_reset(&timer);
    }

    PROCESS_END();
}
