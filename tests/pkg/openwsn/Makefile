BOARD ?= iotlab-m3

include ../Makefile.pkg_common

# OpenWSN Modules
USEPKG += openwsn
USEMODULE += openwsn_openstack
USEMODULE += sock_udp
# This example uses async
USEMODULE += sock_async

# Optional OpenWSN Modules
## Enable Constrained Join Protocol (CoJP)
USEMODULE += openwsn_cjoin
## Enable 6lowpan fragmentation
USEMODULE += openwsn_6lo_fragmentation
## Enable link layer security
# USEMODULE += openwsn_iee802154e_security
## Enable MSF dynamic slot allocation
# USEMODULE += openwsn_adaptive_msf

## Optional Module, but required for root nodes
# USEMODULE += openwsn_serial

# Optional Modules
USEMODULE += openwsn_leds
USEMODULE += openwsn_debugpins

ifneq (,$(filter openwsn_serial,$(USEMODULE)))
# Uncomment to use STDIO_UART_DEV as the uart for OpenWSN openserial
# USEMODULE += stdio_null
  ifneq (,$(filter iotlab-% openmote-b,$(BOARD)))
    USEMODULE += stdio_null
  endif
  # OpenWSN serial module can't handle data at more than 115200 bauds/s,
  # depending on the platform this might be even lower (e.g. 57600 bauds
  # for iotlab-m3, 19200 for samr21-xpro).
  # NOTE: baudrate can'y be changed when testing over IotLab.
  OPENSERIAL_BAUD ?=
  ifneq (,$(OPENSERIAL_BAUD))
    CFLAGS += -DOPENWSN_UART_BAUDRATE=$(OPENSERIAL_BAUD)
    ifneq (,$(filter stdio_null,$(USEMODULE)))
      BAUD ?= $(OPENSERIAL_BAUD)
    endif
  endif
endif

# To enable debugging prints on OpenWSN code
# export OPENWSN_LOG_LEVEL ?= LOG_ERROR

## Test application Modules
USEMODULE += ps
USEMODULE += od_string
USEMODULE += shell_cmds_default

# ztimer is used instead of xtimer because it's a dependency of some
# OpenWSN modules.
USEMODULE += ztimer_usec

include $(RIOTBASE)/Makefile.include
