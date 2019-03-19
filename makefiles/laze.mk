APPDIR_REL:=$(APPDIR:$(RIOTBASE)/%=%)
NINJA_TARGET:=$(APPDIR_REL)/bin/$(BOARD)/$(APPLICATION).elf

ifeq ($(QUIET), 0)
	LAZE_ARGS += -v
endif

ifneq ($(filter $(MAKECMDGOALS),clean),)
	CLEAN=clean
endif

.PHONY: clean all

all: | $(CLEAN)
	laze build $(LAZE_ARGS) -b $(BOARD)

clean:
	@echo "clean unsupported"

flash:
	laze build $(LAZE_ARGS) -b $(BOARD) -t flash

term:
	laze build $(LAZE_ARGS) -b $(BOARD) -t term

buildtest: | $(CLEAN)
	laze build $(LAZE_ARGS)
