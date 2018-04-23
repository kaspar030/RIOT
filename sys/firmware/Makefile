CFLAGS += -DFIRMWARE_NUM_SLOTS=3
CFLAGS += -DSLOT0_SIZE=$(RIOTBOOT_SLOT0_SIZE)
CFLAGS += -DSLOT1_SIZE=$(RIOTBOOT_SLOT1_SIZE)
CFLAGS += -DSLOT2_SIZE=$(RIOTBOOT_SLOT2_SIZE)

SRC = firmware.c update.c

ifneq (,$(filter firmware_riotboot,$(USEMODULE)))
  SRC += firmware_riotboot.c
endif

include $(RIOTBASE)/Makefile.base
