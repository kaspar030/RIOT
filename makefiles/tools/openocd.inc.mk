export FLASHER ?= $(RIOTBASE)/dist/tools/openocd/openocd.sh
export DEBUGGER = $(RIOTBASE)/dist/tools/openocd/openocd.sh
export DEBUGSERVER = $(RIOTBASE)/dist/tools/openocd/openocd.sh
export RESET ?= $(RIOTBASE)/dist/tools/openocd/openocd.sh

FLASHFILE ?= $(ELFFILE)
FFLAGS ?= flash $(FLASHFILE)
export DEBUGGER_FLAGS ?= debug $(ELFFILE)
export DEBUGSERVER_FLAGS ?= debug-server
export RESET_FLAGS ?= reset

ifneq (,$(DEBUG_ADAPTER))
  include $(RIOTMAKE)/tools/openocd-adapters/$(DEBUG_ADAPTER).inc.mk
endif
