export APPLICATION = driver_sim8xx
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_uart

USEMODULE += xtimer sim8xx

CFLAGS += -DSIM8XX_UART=1
CFLAGS += -DSIM8XX_BAUDRATE=115200U

CFLAGS += -DSIM8XX_APN=\"internet.t-mobile\"
# CFLAGS += -DSIM_PIN=xxxx

include $(RIOTBASE)/Makefile.include
