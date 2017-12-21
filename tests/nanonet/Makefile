include ../Makefile.tests_common

USEMODULE += nanonet
USEMODULE += core_thread_flags
USEMODULE += nanocoap

USEMODULE += netdev_default
#USEMODULE += ethos
#CFLAGS += '-DETHOS_UART=UART_DEV(0)' -DETHOS_BAUDRATE=115200 -DUSE_ETHOS_FOR_STDIO

CFLAGS += -DNANOCOAP_DEBUG -g
DISABLE_MODULE += core_msg

include $(RIOTBASE)/Makefile.include
