#include "contiki.h"
#include "contiki-net.h"
#include "coap-engine.h"
#include "machine_learning/temperature_prediction.h"
#include <stdio.h>
static float current_prediction=0;
static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_event_handler(void);
void update_sensors_and_predict();
// Dichiarazione della risorsa CoAP
EVENT_RESOURCE(res_predict_temp,
         "title=\"Prediction\";rt=\"Text\"",
         res_get_handler,
         NULL,
         NULL,
         NULL,
         res_event_handler);
// Funzione handler per il CoAP endpoint
static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer,
                          uint16_t preferred_size, int32_t *offset) {
    // Simula la lettura di nuovi valori di temperatura e umidità
  /*  float new_temperature = 15.0 + (random_rand() % 150) / 10.0;
    float new_humidity = 30.0 + (random_rand() % 700) / 10.0;
    
    // Aggiorna i valori dei sensori
    update(new_temperature, new_humidity);
    
    // Predice la prossima temperatura
    float next_temperature = predict(); */
    
    // Costruisce il payload della risposta
    int length = snprintf((char *)buffer, preferred_size, "%.2f", current_prediction);

    // Imposta i campi della risposta CoAP
    coap_set_header_content_format(response, TEXT_PLAIN);
    coap_set_header_etag(response, (uint8_t *)&length, 1);
    coap_set_payload(response, (uint8_t *)buffer, length);
}
static void res_event_handler(void){
    
    printf("mando notifica \n");
    coap_notify_observers(&res_predict_temp);
}
void update_sensors_and_predict() {
    // Simula la lettura di nuovi valori di temperatura e umidità
    float new_temperature = 15.0 + (random_rand() % 150) / 10.0;
    float new_humidity = 30.0 + (random_rand() % 700) / 10.0;

    // Aggiorna i valori dei sensori
    update_sensor_values(new_temperature, new_humidity);

    // Predice la prossima temperatura
    current_prediction = predict_next_temperature();
   // printf("the current_prediction now, %f", current_prediction);

    // Attiva l'evento per notificare gli osservatori
    
}