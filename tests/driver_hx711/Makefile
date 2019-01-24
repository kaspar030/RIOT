include ../Makefile.tests_common

FEATURES_REQUIRED += periph_gpio

USEMODULE += hx711

# export parameters
CFLAGS += '-DHX711_PARAM_DOUT=ARDUINO_PIN_4'
CFLAGS += '-DHX711_PARAM_PD_SCK=ARDUINO_PIN_5'

include $(RIOTBASE)/Makefile.include
