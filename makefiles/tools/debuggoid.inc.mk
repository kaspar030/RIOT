FLASHER ?= $(RIOTBASE)/dist/tools/debuggoid/debuggoid.sh

PYOCD_FLASH_TARGET_TYPE ?=
FLASHFILE ?= $(ELFFILE)
FFLAGS ?= flash $(FLASHFILE)
# DEBUGGER_FLAGS ?= debug $(ELFFILE)
# DEBUGSERVER_FLAGS ?= debug-server
# RESET_FLAGS ?= reset

