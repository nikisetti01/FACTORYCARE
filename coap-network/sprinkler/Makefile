CONTIKI_PROJECT = sprinkler

all: $(CONTIKI_PROJECT)

MODULES_REL=./resources
MODULES_REL+=./global_variable
LDFLAGS += -lm
# Do not try to build on Sky because of code size limitation
PLATFORMS_EXCLUDE = sky z1
MODULES_REL+=../cJSON-master


CONTIKI=../../../..


# Include the CoAP implementation
include $(CONTIKI)/Makefile.dir-variables
MODULES += $(CONTIKI_NG_APP_LAYER_DIR)/coap

include $(CONTIKI)/Makefile.include
