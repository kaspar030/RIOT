DEVELHELP = 0
include ../Makefile.tests_common
DISABLE_MODULE += test_utils_interactive_sync

USEMODULE += nanocoap

#USEMODULE += netdev_default

USEMODULE += nanonet nanonet_udp nanonet_eth nanonet_ipv4 nanonet_ipv6
#USEMODULE += nanonet_tcp xtimer fmt

DISABLE_MODULE += core_msg
#
# uncomment this to use ethos
#
#USEMODULE += ethos
#CFLAGS += '-DETHOS_UART=UART_DEV(0)' -DETHOS_BAUDRATE=115200 -DUSE_ETHOS_FOR_STDIO

#
# uncomment this to use encx26j600
#
#ENC_SPI ?= SPI_DEV\(0\)
#ENC_CS  ?= GPIO_PIN\(PORT_C,10\)
#ENC_INT ?= GPIO_PIN\(PORT_D,2\)
#CFLAGS += -DENCX24J600_SPI=$(ENC_SPI)
#CFLAGS += -DENCX24J600_CS=$(ENC_CS)
#CFLAGS += -DENCX24J600_INT=$(ENC_INT)
#USEMODULE += encx24j600

# use this for USB ethernet
USEMODULE += usbus_cdc_ecm

#
# debug build
#
#CFLAGS += -DDEVELHELP -DENABLE_NANONET_DEBUG -DNANOCOAP_DEBUG -g

#
# headless, small-memory configuration
#
#CFLAGS += -DNDEBUG -DLOG_LEVEL=LOG_NONE
#CFLAGS += -DMAXTHREADS=2
#CFLAGS += -DSCHED_PRIO_LEVELS=2
#CFLAGS += -DTHREAD_STACKSIZE_IDLE=128

include $(RIOTBASE)/Makefile.include
