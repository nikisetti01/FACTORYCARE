#ifndef GLOBAL_VARIABLES_H
#define GLOBAL_VARIABLES_H
    //modifica samples con un array di 10 elementi struttura {timeid, temperature, humidity}
typedef struct {
    int timeid;
    float temperature;
    float humidity;
} Sample;

extern int shutdown; // Variable to shutdown the device
 extern Sample samples[10]; // Array to store the samples    

#endif /* GLOBAL_VARIABLES_H */
