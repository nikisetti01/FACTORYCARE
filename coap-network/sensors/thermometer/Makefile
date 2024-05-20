CONTIKI_PROJECT = thermometer
# coap-example-observe-client is outdated but will be ported at a later point
all: $(CONTIKI_PROJECT)
MODULES_REL=./resources
MODULES_REL=./resources/machine_learning
# Do not try to build on Sky because of code size limitation
PLATFORMS_EXCLUDE = sky z1
MODULES_REL += /home/iot_ubuntu_intel/.local/lib/python3.10/site-packages/emlearn
TARGET_LIBFILES += -lm
INC += /home/iot_ubuntu_intel/.local/lib/python3.10/site-packages/emlearn
CONTIKI=../../../../..

# Include the CoAP implementation
include $(CONTIKI)/Makefile.dir-variables
MODULES += $(CONTIKI_NG_APP_LAYER_DIR)/coap

include $(CONTIKI)/Makefile.include
