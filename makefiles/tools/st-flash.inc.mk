RIOT_STFLASH = $(RIOTBASE)/dist/tools/st-flash/bin/st-flash
STFLASH ?= $(RIOT_STFLASH)
FLASHER ?= $(STFLASH)
OFLAGS ?= -O ihex
HEXFILE = $(ELFFILE:.elf=.hex)
# Use USB serial number to select device when more than one is connected
# Use /dist/tools/usb-serial/list-ttys.sh to find out serial number.
#   Usage:
# export DEBUG_ADAPTER_ID="ATML..."
# BOARD=<board> make flash
ifneq (,$(DEBUG_ADAPTER_ID))
  STFLASH_ARGS += --serial $(DEBUG_ADAPTER_ID)
endif
FFLAGS ?= $(STFLASH_ARGS) --format ihex write $(HEXFILE)

ifeq ($(RIOT_STFLASH),$(FLASHER))
  FLASHDEPS += $(RIOT_STFLASH)
endif
RESET ?= $(STFLASH)
RESET_FLAGS ?= $(STFLASH_ARGS) -t $(STFLASH_DEVICE_TYPE)
