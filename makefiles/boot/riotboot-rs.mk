ifneq (,$(filter riotboot-rs,$(FEATURES_USED)))

.PHONY: riotboot-rs riotboot-rs/flash

RIOTBOOT_RS_FLASH_DIR = $(RIOTBASE)/bootloaders/riotboot

export RAM_START_ADDR
export ROM_START_ADDR
export RIOTBOOT_LEN
export SLOT0_OFFSET
export SLOT1_OFFSET

riotboot-rs:
	@$(Q)CC= CFLAGS= $(MAKE) -C $(RIOTTOOLS)/riotboot-rs

riotboot-rs/flash: riotboot-rs
	$(Q)/usr/bin/env -i \
		QUIET=$(QUIET) PATH="$(PATH)"\
		EXTERNAL_BOARD_DIRS="$(EXTERNAL_BOARD_DIRS)" BOARD="$(BOARD)"\
		DEBUG_ADAPTER_ID="$(DEBUG_ADAPTER_ID)" PROGRAMMER="$(PROGRAMMER)"\
		FLASHFILE="$(BINDIR)/riotboot-rs.bin"\
		$(MAKE) -C "$(RIOTBOOT_RS_FLASH_DIR)" --no-print-directory flash-only
endif # (,$(filter riotboot-rs,$(FEATURES_USED)))
