include ../Makefile.tests_common

USEMODULE += nanonet
USEMODULE += core_thread_flags
USEMODULE += nanocoap

USEMODULE += netdev_default

# uncomment this to use ethos
#USEMODULE += ethos
#CFLAGS += '-DETHOS_UART=UART_DEV(0)' -DETHOS_BAUDRATE=115200 -DUSE_ETHOS_FOR_STDIO

# uncomment this to use encx26j600
ENC_SPI ?= SPI_DEV\(0\)
ENC_CS  ?= GPIO_PIN\(PORT_C,10\)
ENC_INT ?= GPIO_PIN\(PORT_D,2\)
CFLAGS += -DENCX24J600_SPI=$(ENC_SPI)
CFLAGS += -DENCX24J600_CS=$(ENC_CS)
CFLAGS += -DENCX24J600_INT=$(ENC_INT)
USEMODULE += encx24j600

CFLAGS += -DNANOCOAP_DEBUG -g
DISABLE_MODULE += core_msg

include $(RIOTBASE)/Makefile.include
