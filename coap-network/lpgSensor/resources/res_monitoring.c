#include "contiki.h"
#include "contiki-net.h"
#include "coap-engine.h"
#include "../cJSON-master/cJSON.h"
#include <stdio.h>
#include <stdlib.h> // Aggiunto per utilizzare random_rand()
#include <string.h>
#include "../global_variable/global_variables.h"
#include "model/functionsML.h"


static float new_co = 0.0;
static float new_smoke = 0.0;
static bool new_light = 0;
static int new_pred = 0;
extern Sample sample;// Array to store the samples
int count=0;
static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer,
                            uint16_t preferred_size, int32_t *offset);

static void res_event_handler(void);

EVENT_RESOURCE(res_monitoring_lpg,
         "title=\"monitoring_lpg\";rt=\"Text\"",
         res_get_handler,
         NULL,
         NULL,
         NULL,
         res_event_handler);

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer,
                             uint16_t preferred_size, int32_t *offset) {


    count++;

 // new variable prendano elementi della struct sample
    new_co = sample.co;
    new_smoke = sample.smoke;
    new_light = sample.light;

    float features[4] = {sample.co, sample.smoke, sample.light, sample.humidity};
    new_pred = predict_class(features,4);

    printf("CO: %f\n", new_co);
    printf("Smoke: %f\n", new_smoke);
    printf("Light: %d\n", new_light);
    printf("Prediction: %d\n", new_pred);

    // crea il json
    cJSON *root = cJSON_CreateObject();
    cJSON*ss=cJSON_CreateArray();
    // cJSON_AddItemToObject(root,"id",cJSON_CreateNumber(count));
    cJSON_AddItemToArray(ss,cJSON_CreateNumber(new_co));
    cJSON_AddItemToArray(ss,cJSON_CreateNumber(new_smoke));
    cJSON_AddItemToArray(ss,cJSON_CreateBool(new_light));
    cJSON_AddItemToArray(ss,cJSON_CreateNumber(new_pred)); //prediction
    cJSON_AddItemToObject(root,"ss",ss);
    char *json = cJSON_Print(root);
    printf("length %lo\n", strlen(json));

    int length = snprintf((char *)buffer, preferred_size, "%s", json);


    if (length < 0 || length >= preferred_size) {
        coap_set_status_code(response, BAD_REQUEST_4_00);
        return;
    }

    coap_set_header_etag(response, (uint8_t *)&length, 1);
    coap_set_payload(response, (uint8_t *)buffer, length);
    
    count++;
}

static void res_event_handler(void)  {
    coap_notify_observers(&res_monitoring_lpg);
}
