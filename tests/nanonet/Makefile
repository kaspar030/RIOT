APPLICATION = nanonet
include ../Makefile.tests_common

USEMODULE += nanonet core_thread_flags
USEMODULE += prng_musl_lcg
USEPKG += microcoap

#USEMODULE += netdev2_tap
USEMODULE += ethos
CFLAGS += '-DETHOS_UART=UART_DEV(0)' -DETHOS_BAUDRATE=115200 -DUSE_ETHOS_FOR_STDIO

CFLAGS += -DMICROCOAP_DEBUG -g

include $(RIOTBASE)/Makefile.include
