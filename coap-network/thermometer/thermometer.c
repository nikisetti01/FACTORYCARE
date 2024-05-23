#include "contiki.h"
#include "contiki-net.h"
#include "coap-engine.h"
#include "random.h"
#include "global_variables.h"
#include <stdio.h>
#include "sys/log.h"
#include "resources/machine_learning/temperature_prediction.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_APP
 

extern coap_resource_t res_predict_temp;
coap_message_t request[1]; 

PROCESS(thermometer_process, "Thermometer Process");
AUTOSTART_PROCESSES(&thermometer_process);
 static struct etimer prediction_timer;
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
    etimer_set(&prediction_timer, CLOCK_SECOND * 3);
    printf("CoAP server started\n");

    // Processo principaale in loop
    while(1) {
        PROCESS_WAIT_EVENT();
        if(etimer_expired(&prediction_timer)) {
            // Aggiorna i sensori e fai la previsione             
              res_predict_temp.trigger();

            // Resetta il timer per il prossimo aggiornamento
            etimer_reset(&prediction_timer);
        }
       
    }

    PROCESS_END();
}
