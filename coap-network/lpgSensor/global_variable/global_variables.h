#ifndef GLOBAL_VARIABLES_H
#define GLOBAL_VARIABLES_H
    //modifica samples con un array di 10 elementi struttura {timeid, temperature, humidity}
typedef struct {

    float co;
    float smoke;
    bool light;
    float humidity;
} Sample;

extern int shutdown; // Variable to shutdown the device
 extern Sample sample;  

#endif /* GLOBAL_VARIABLES_H */
