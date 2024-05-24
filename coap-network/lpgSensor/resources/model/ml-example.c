/*
 * Copyright (c) 2006, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */

/**
 * \file
 *         A very simple Contiki application showing how Contiki programs look
 * \author
 *         Adam Dunkels <adam@sics.se>
 */

#include "contiki.h"

#include "random.h"

//#include "airDetetionModel.h" // emlearn generated model

#include "air_model.h"

#include "eml_net.h"


#include <stdio.h> /* For printf() */
/*---------------------------------------------------------------------------*/
PROCESS(hello_world_process, "Hello world process");
AUTOSTART_PROCESSES(&hello_world_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(hello_world_process, ev, data)
{
  static struct etimer timer;

  PROCESS_BEGIN();

  float features[] = { 30, 90, 1060, 0, 0, 0, 0 };

  printf("%p\n", eml_net_activation_function_strs); // This is needed to avoid compiler error (warnings == errors)

  /* Setup a periodic timer that expires after 10 seconds. */
  //etimer_set(&timer, CLOCK_SECOND * 10);

  random_init(0);

  while(1) {
    etimer_set(&timer, CLOCK_SECOND * 10);
     PROCESS_WAIT_EVENT();
       //printf("received line: %s\n", (char *)data);
       printf("evento registrato\n");

       float ts=0, co=10, hum=100, light=2, mot=0, smoke=0, tmp=0;

      ts = 1594512094.3859746;//random_rand() % 1000;
      co = 0.0049559386483912;//random_rand() % 1000;
      hum = 51; // random_rand() % 1000;
      light = 0;//random_rand() % 1000;
      mot = 0;//random_rand() % 1000;
      smoke = 0.204112701;//random_rand() % 1000;
      tmp = 22.7;//random_rand() % 1000;
       
       //unnecessary for our project to parse value, we collect and predict in the same dongle
       //sprintf(data, "%u %u %u", tmp, hum, pre);

      features[0]= (float) ts;
      features[1] = (float) co;
      features[2] = (float) hum;
      features[3] = (float) light;
      features[4] = (float) mot;
      features[5] = (float) smoke;
      features[6] = (float) tmp;

      int lenght = 7;

      printf("features %f, %f, %f, %f, %f, %f, %f\n", features[0], features[1], features[2], features[3], features[4], features[5], features[6]);

      //float outputs[3] = {0, 0, 0};
      
      printf("%p\n",  eml_net_activation_function_strs);

      //eml_net_predict_proba(&air_model, features, 7, outputs, 3);

      // using generated "inline" code for the decision forest
      //const int32_t predicted_class = air_model_predict(features, lenght);
      //printf("prediction %li\n", predicted_class);

      // ALT: using the generated decision forest datastructure
      // eml_trees_predict(const EmlTrees *forest, const float *features, int8_t features_length)
      const int32_t predicted_class2 = eml_trees_predict(&air_model,features, lenght);
      int uscita = (int)predicted_class2;
      printf("prediction2 %i\n", uscita);

      //printf("probabilities %i, %i, %i\n",  (int) ((float) outputs[0]*100), (int) ((float) outputs[1]*100), (int) ((float)outputs[2]*100));

    /* Wait for the periodic timer to expire and then restart the timer. */
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
    etimer_reset(&timer);
	  
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/