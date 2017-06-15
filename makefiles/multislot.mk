ifneq (,$(filter multislot combined flash-multislot, $(MAKECMDGOALS)))

ifndef SLOT0_LENGTH
	$(error Board $(BOARD) does not define multislot parameters!)
endif

ELFSLOT0 := $(ELFFILE:%.elf=%.slot0.elf)
ELFSLOT1 := $(ELFFILE:%.elf=%.slot1.elf)
ELFSLOT2 := $(ELFFILE:%.elf=%.slot2.elf)

COMBINED_BIN := $(ELFFILE:%.elf=%.combined.bin)

ifneq (, $(filter $(USEMODULE),bootloader))
$(info BOOTLOADER BUILD)
BOOTLOADER = 1
else
$(info NON BOOTLOADER BUILD)
BOOTLOADER = 0
BOOTLOADER_BIN = $(RIOTBASE)/examples/bootloader/bin/$(BOARD)/bootloader.slot0.bin
$(BOOTLOADER_BIN):
	@env -i PATH=$(PATH) BOARD=$(BOARD) make -C $(RIOTBASE)/examples/bootloader clean multislot
endif

multislot: all $(BOOTLOADER_BIN)
	$(Q)[ $(BOOTLOADER) -ne 1 ] || { \
		$(_LINK) $(LINKFLAGPREFIX)--defsym=offset=0x0 -Wl,--defsym=length=$(SLOT0_LENGTH) -o $(ELFSLOT0) ; \
		$(OBJCOPY) -Obinary $(ELFSLOT0) $(ELFSLOT0:%.elf=%.bin) ; \
		truncate $(ELFSLOT0:%.elf=%.bin) --size=$$(($(SLOT0_LENGTH))) ; \
	}

	$(Q)[ $(BOOTLOADER) -ne 0 ] || { \
		$(_LINK) $(LINKFLAGPREFIX)--defsym=offset=$(SLOT0_LENGTH) -Wl,--defsym=length=$(SLOT1_LENGTH) -o $(ELFSLOT1) && \
		$(OBJCOPY) -Obinary $(ELFSLOT1) $(ELFSLOT1:%.elf=%.bin) && \
		truncate $(ELFSLOT1:%.elf=%.bin) --size=$$(($(SLOT1_LENGTH))) && \
		\
		$(_LINK) $(LINKFLAGPREFIX)--defsym=offset=$(SLOT0_LENGTH)+$(SLOT1_LENGTH) -Wl,--defsym=length=$(SLOT2_LENGTH) -o $(ELFSLOT2) && \
		$(OBJCOPY) -Obinary $(ELFSLOT2) $(ELFSLOT2:%.elf=%.bin) && \
		truncate $(ELFSLOT2:%.elf=%.bin) --size=$$(($(SLOT2_LENGTH))) ; \
	}

combined: multislot
	sh -c 'cat $(BOOTLOADER_BIN) $(ELFSLOT1:%.elf=%.bin) $(ELFSLOT2:%.elf=%.bin) > $(COMBINED_BIN)'

.PHONY: flash-multislot
flash-multislot: combined $(FLASHDEPS)
	$(FLASHER) $(FFLAGS)

endif # multislot targets filter
