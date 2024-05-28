#include "temperature_prediction.h"
#include "contiki.h"
#include "random.h"
#include "temperature_prediction_model.h"  // emlearn generated model
#include "eml_trees.h" // Correct header for tree-based models
#include <stdio.h>

// Definisci il range di normalizzazione per temperatura e umidità
#define TEMP_MIN 15.0
#define TEMP_MAX 30.0
#define HUMIDITY_MIN 30.0
#define HUMIDITY_MAX 100.0

#define MAXDATASET 30
#define MINDATASET 0

// Array per memorizzare i valori di temperatura e umidità normalizzati
static float temperatures[10];
static float humidities[10];
static int index = 0;

// Funzione per denormalizzare il valore predetto
float denormalized_prediction(float prediction) {
    return prediction * (MAXDATASET - MINDATASET) + MINDATASET;
}

// Funzione per normalizzare i valori di temperatura e umidità
float normalize(float value, float min, float max) {
    return (value - min) / (max - min);
}

// Funzione per aggiornare i valori di temperatura e umidità
void update_sensor_values(float new_temperature, float new_humidity) {
    // Normalizza i valori di temperatura e umidità
    float normalized_temperature = normalize(new_temperature, TEMP_MIN, TEMP_MAX);
    float normalized_humidity = normalize(new_humidity, HUMIDITY_MIN, HUMIDITY_MAX);

    // Aggiorna gli array con i nuovi valori normalizzati
    temperatures[index] = normalized_temperature;
    humidities[index] = normalized_humidity;

    // Aggiorna l'indice per il buffer circolare
    index = (index + 1) % 10;
}

// Funzione per predire la prossima temperatura
float predict_next_temperature() {
    // Prepara l'input per il modello di predizione
    float input[20];
    for(int i = 0; i < 10; i++) {
        input[i] = temperatures[i];
        input[10 + i] = humidities[i];
    }
    
    // Predici la prossima temperatura
    float predicted_temperature = eml_trees_regress1(&temperature_prediction_model, input, 20);
    return denormalized_prediction(predicted_temperature);
}
// funzione che prende un array di 10 elementi temp e 10 elementi umidità e predice la temperatura successiva
float predict_next_temperature_from_values(float temp[], float hum[]) {
    // Prepara l'input per il modello di predizione
    float input[20];
    for(int i = 0; i < 10; i++) {
        input[i] = temp[i];
        input[10 + i] = hum[i];
    }
    
    // Predici la prossima temperatura
    float predicted_temperature = eml_trees_regress1(&temperature_prediction_model, input, 20);
    return denormalized_prediction(predicted_temperature);
}
