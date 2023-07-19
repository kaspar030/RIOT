APPDIR_REL:=$(APPDIR:$(RIOTBASE)/%=%)
NINJA_TARGET:=$(APPDIR_REL)/bin/$(BOARD)/$(APPLICATION).elf

ifeq ($(QUIET), 0)
	LAZE_ARGS += -v
endif

ifneq ($(filter $(MAKECMDGOALS),clean),)
	CLEAN=clean
endif

LAZE_TOOLS = flash term reset cleanterm

.PHONY: clean all $(LAZE_TOOLS)

all: | $(CLEAN)
	laze build $(LAZE_ARGS) -b $(BOARD)

clean:
	@laze clean $(LAZE_ARGS)

$(LAZE_TOOLS):
	laze build $(LAZE_ARGS) -b $(BOARD) $@

buildtest: | $(CLEAN)
	laze build $(LAZE_ARGS)
