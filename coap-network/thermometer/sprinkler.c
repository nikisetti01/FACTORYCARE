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
//static float k_lpg=0.2;
static float water=0;
PROCESS(coap_client_process, "CoAP Client Process");
AUTOSTART_PROCESSES(&coap_client_process);
float update_water_production_temperature(float water, float last_temperature, float next_temperature) {
    float diff;
    if (last_temperature < 20)
        water = STARTING_WATER;
    else
        diff = next_temperature - last_temperature;
    water += diff * k_temp;
    return water;
}

float update_water_production_lpg() {
    water = STARTING_WATER + water * water * k_lpg;
    return water;
}


void response_handler_temp(coap_message_t *response )
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
    last_temperature = next_temperature;
    next_temperature = strtof(temp_str, NULL);
    printf("Temperature Response: |%.*s| (Parsed: %.2f)\n", len, (char *)chunk, next_temperature);
}

void handle_notification_temp(struct coap_observee_s *observee, void *notification, coap_notification_flag_t flag) {
    coap_message_t *msg = (coap_message_t *)notification;
    if (msg) {
        printf("Received temperature notification\n");
        response_handler_temp(msg);
        process_poll(&coap_client_process); // Poll the main process to handle the event
    } else {
        printf("No temperature notification received\n");
    }
}

void response_handler_lpg(coap_message_t *response) {
    if (response == NULL) {
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

void handle_notification_lpg(struct coap_observee_s *observee, void *notification, coap_notification_flag_t flag) {
    coap_message_t *msg = (coap_message_t *)notification;
    if (msg) {
        printf("Received LPG notification\n");
        response_handler_lpg(msg);
        process_poll(&coap_client_process); // Poll the main process to handle the event
    } else {
        printf("No LPG notification received\n");
    }
}

PROCESS_THREAD(coap_client_process, ev, data)
{
    PROCESS_BEGIN();

    static coap_endpoint_t server_ep_temp;
    static coap_endpoint_t server_ep_lpg;
    static struct etimer ledtimer;

    coap_endpoint_parse(SERVER_EP_TEMP, strlen(SERVER_EP_TEMP), &server_ep_temp);
    coap_endpoint_parse(SERVER_EP_LPG, strlen(SERVER_EP_LPG), &server_ep_lpg);

    printf("Sending observation request to %s\n", SERVER_EP_TEMP);
    coap_obs_request_registration(&server_ep_temp, service_url_temp, handle_notification_temp, NULL);

    printf("Sending observation request to %s\n", SERVER_EP_LPG);
    coap_obs_request_registration(&server_ep_lpg, service_url_lpg, handle_notification_lpg, NULL);

    etimer_set(&ledtimer, 2 * CLOCK_SECOND); // Imposta il timer del LED a 2 secondi per iniziare

    while (1)
    {
        PROCESS_WAIT_EVENT(); // Attendiamo un evento

        // Gestione del timer del LED
        if (ev == PROCESS_EVENT_TIMER && etimer_expired(&ledtimer))
        {
            if (lpg_level == 2) {
                water=update_water_production_lpg();
                printf("on for lpg\n");
                printf("Rilascio %f di acqua\n", water);
                // Lampeggio del LED rosso se il livello di LPG è alto
                if (led_now == LEDS_RED) {
                    leds_single_off(LEDS_RED);
                } else {
                    leds_single_on(LEDS_RED);
                    led_now = LEDS_RED;
                }
                etimer_reset(&ledtimer); // Reset del timer del LED
            }
        }

        // Gestione delle notifiche
        if (ev == PROCESS_EVENT_POLL) {
            // Logica basata sulla temperatura e sul livello LPG
            if (next_temperature < 20 && lpg_level != 2) {
                leds_single_off(led_now);
                led_now = LEDS_GREEN;
                leds_single_on(led_now);
                printf("Sprinkler off\n");
            } else if (next_temperature >= 20 && lpg_level != 2) {
                leds_single_off(led_now);
                led_now = LEDS_BLUE;
                leds_single_on(led_now);
                printf("on for temperature\n");
                water = update_water_production_temperature(water, last_temperature, next_temperature);
                printf("Rilascio %f di acqua\n", water);
            }
        }
    }

    PROCESS_END();
}
