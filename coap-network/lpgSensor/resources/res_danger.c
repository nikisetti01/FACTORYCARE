<<<<<<< HEAD
/*
 * Copyright (c) 2013, Institute for Pervasive Computing, ETH Zurich
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
 */

/**
 * \file
 *      Example resource
 * \author
 *      Matthias Kovatsch <kovatsch@inf.ethz.ch>
 */

#include <stdlib.h>
#include <string.h>
#include "coap-engine.h"

#include "model/functionsML.h"

#define MAX_LINE_LENGTH 100


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
int get_danger();

/*
 * A handler function named [resource name]_handler must be implemented for each RESOURCE.
 * A buffer for the response payload is provided through the buffer pointer. Simple resources can ignore
 * preferred_size and offset, but must respect the REST_MAX_CHUNK_SIZE limit for the buffer.
 * If a smaller block size is requested for CoAP, the REST framework automatically splits the data.
 */
RESOURCE(res_danger,
         "title=\"airquality: ?len=0..\";rt=\"Text\"",
         res_get_handler,
         NULL,
         NULL,
         NULL);

static void
res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{

  const char *len = NULL;
  /* Some data that has the length up to REST_MAX_CHUNK_SIZE. For more, see the chunk resource. */
  int val = get_danger();

  printf("val %i\n", val);

  char message[20];
  sprintf(message, "%i", val);
  int length = 20; /*           |<-------->| */
 
  /* The query string can be retrieved by rest_get_query() or parsed for its key-value pairs. */
  if(coap_get_query_variable(request, "len", &len)) {
    printf("request:%s\n", request->payload);
    length = atoi(len);
    if(length < 0) {
      length = 0;
    }
    if(length > REST_MAX_CHUNK_SIZE) {
      length = REST_MAX_CHUNK_SIZE;
    }
    memcpy(buffer, message, length);
  } else {
    memcpy(buffer, message, length);
  }

  printf("buffer %s\n message %s\n", buffer, message);

  coap_set_header_content_format(response, TEXT_PLAIN); /* text/plain is the default, hence this option could be omitted. */
  coap_set_header_etag(response, (uint8_t *)&length, 1);
  coap_set_payload(response, buffer, length);
}
static int prova=0;
int get_danger() {
  printf("get_danger: predizione in corso\n");
  //float features[] = { 0, 0, 0, 0, 0, 0, 0 };

  /*
  FILE* file = fopen("model/modelData.csv", "r");
  if (file == NULL) {
    perror("Error opening file");
    printf("Error opening file\n");
    //features[0] = 0.1;
    //features[1] = 0.1;
    //features[2] = 0.1;
    //features[3] = 0.1;
    //features[4] = 0.1;
    //features[5] = 0.1;
    //features[6] = 0.1;

    return -1;
  }

  char line[MAX_LINE_LENGTH];
  if (fgets(line, sizeof(line), file) == NULL) {
    printf("Error reading file\n");
    fclose(file);
    return -1;
  }

  int line_count = 0;
  line_count = rand() % 301;

  while (fgets(line, sizeof(line), file)) {
    if (line_count == 0) {
      line_count++;
      continue;
    }

    char *token = strtok(line, ",");
    int token_count = 0;
    while (token != NULL && token_count < 7) {
      features[token_count] = atof(token);
      token = strtok(NULL, ",");
      token_count++;
    }

    if (token_count < 7) {
      printf("Error: insufficient tokens in line\n");
      fclose(file);
      return -1;
    }

    break;
  }

  fclose(file);

  printf("features: %f, %f, %f, %f, %f, %f, %f\n", features[0], features[1], features[2], features[3], features[4], features[5], features[6]);
  */

  int result = prova;//rand() % 3; // predict_class(features, 7);
  prova++;
  prova>2?prova=0:prova;
  printf("result: %d\n", result);

  if (result == -1) {
    printf("Prediction error\n");
  }

  return result;
=======
#include <stdlib.h>
#include <string.h>
#include "coap-engine.h"
#include <stdio.h>

#define MAX_LINE_LENGTH 100

static int prova = 0;
static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
int get_danger();
static void res_event_handler(void);

EVENT_RESOURCE(res_danger,
         "title=\"airquality\"; rt=\"Text\"",
         res_get_handler,
         NULL,
         NULL,
         NULL,
         res_event_handler);

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) {
    printf("siamo in res_get_handler\n");
    int val = get_danger();

    printf("val %i\n", val);
    
    // Formatta il valore come stringa
    int length = snprintf((char *)buffer, preferred_size, "%d", val);

    if (length < 0) {
        coap_set_status_code(response, INTERNAL_SERVER_ERROR_5_00);
        return;
    }

    // Verifica che il buffer abbia abbastanza spazio
    if (length >= preferred_size) {
        coap_set_status_code(response, BAD_OPTION_4_02);
        return;
    }

    coap_set_header_content_format(response, TEXT_PLAIN);
    coap_set_header_etag(response, (uint8_t *)&length, 1);
    coap_set_payload(response, buffer, length);
}

static void res_event_handler(void) {
    printf("mando notifica lpgSensor\n");
    coap_notify_observers(&res_danger);
}

int get_danger() {
    printf("get_danger: predizione in corso\n");

    int result = prova;
    prova++;
    if (prova > 2) prova = 0;
    printf("result: %d\n", result);

    if (result == -1) {
        printf("Prediction error\n");
    }

    return result;
>>>>>>> termobranch
}
