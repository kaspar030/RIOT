APPLICATION = nanonet
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_spi periph_gpio

USEMODULE += dev_eth_autoinit
USEMODULE += ng_nativenet
USEMODULE += vtimer
USEMODULE += nanonet
USEPKG += microcoap

CFLAGS += -DMICROCOAP_DEBUG

include $(RIOTBASE)/Makefile.include
