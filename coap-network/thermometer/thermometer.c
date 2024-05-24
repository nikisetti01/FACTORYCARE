#include "contiki.h"
#include "contiki-net.h"
#include "coap-engine.h"
#include "random.h"
#include "resources/res-predict-temp.c"
#include <stdio.h>
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_APP

extern coap_resource_t res_predict_temp;
coap_message_t request[1]; 

PROCESS(thermometer_process, "Thermometer Process");
AUTOSTART_PROCESSES(&thermometer_process);

PROCESS_THREAD(thermometer_process, ev, data)
{
    PROCESS_BEGIN();
    PROCESS_PAUSE();
    printf("Activate server term \n");
     LOG_INFO("Starting Erbium Example Server\n");

    // Aggiungi la risorsa
    coap_activate_resource(&res_predict_temp, "predict-temp");

    // Inizializza il generatore di numeri casuali
    random_init(0);

    printf("CoAP server started\n");

    // Processo principaale in loop
    while(1) {
        PROCESS_WAIT_EVENT();
        printf("è successo qualcosa");
    }

    PROCESS_END();
}
