#ifndef TEMPERATURE_PREDICTION_H
#define TEMPERATURE_PREDICTION_H

float denormalized_prediction(float prediction);
float normalize(float value, float min, float max);
void update_sensor_values(float new_temperature, float new_humidity);
float predict_next_temperature();
float update_sensors_and_predict(void);
#endif // TEMPERATURE_PREDICTION_H
