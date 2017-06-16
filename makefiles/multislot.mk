ifneq (,$(filter multislot combined flash-multislot, $(MAKECMDGOALS)))

ifndef SLOT0_SIZE
$(error Board $(BOARD) does not define multislot parameters!)
endif

ELFSLOT0 := $(ELFFILE:%.elf=%.slot0.elf)
ELFSLOT1 := $(ELFFILE:%.elf=%.slot1.elf)
ELFSLOT2 := $(ELFFILE:%.elf=%.slot2.elf)

BINSLOT0 := $(ELFSLOT0:%.elf=%.bin)
BINSLOT1 := $(ELFSLOT1:%.elf=%.bin)
BINSLOT2 := $(ELFSLOT2:%.elf=%.bin)

COMBINED_BIN := $(ELFFILE:%.elf=%.combined.bin)

FIRMWARE_METADATA_SIZE = 256

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

GENMETA = $(RIOTBASE)/dist/tools/firmware_metadata/bin/generate-metadata

multislot: all $(BOOTLOADER_BIN)
	$(Q)[ $(BOOTLOADER) -ne 1 ] || { \
		$(_LINK) $(LINKFLAGPREFIX)--defsym=offset=0x0 -Wl,--defsym=length=$(SLOT0_SIZE) -o $(ELFSLOT0) ; \
		$(OBJCOPY) -Obinary $(ELFSLOT0) $(ELFSLOT0:%.elf=%.bin) ; \
		truncate $(ELFSLOT0:%.elf=%.bin) --size=$$(($(SLOT0_SIZE))) ; \
	}

	$(Q)[ $(BOOTLOADER) -ne 0 ] || { \
		$(_LINK) \
			$(LINKFLAGPREFIX)--defsym=offset="$(SLOT0_SIZE)+$(FIRMWARE_METADATA_SIZE)" \
			$(LINKFLAGPREFIX)--defsym=length="$(SLOT1_SIZE)-$(FIRMWARE_METADATA_SIZE)" \
			-o $(ELFSLOT1) && \
		$(OBJCOPY) -Obinary $(ELFSLOT1) $(BINSLOT1) && \
		$(GENMETA) $(BINSLOT1) 0x1234 1 $(BINSLOT1).meta && \
		truncate $(BINSLOT1) --size=$$(($(SLOT1_SIZE))) && \
		\
		$(_LINK) \
			$(LINKFLAGPREFIX)--defsym=offset="$(SLOT0_SIZE)+$(SLOT1_SIZE)+$$(($(FIRMWARE_METADATA_SIZE)*2))" \
			$(LINKFLAGPREFIX)--defsym=length="$(SLOT2_SIZE)-$(FIRMWARE_METADATA_SIZE)" \
			-o $(ELFSLOT2) && \
		$(OBJCOPY) -Obinary $(ELFSLOT2) $(BINSLOT2) && \
		$(GENMETA) $(BINSLOT2) 0x1234 1 $(BINSLOT2).meta && \
		truncate $(BINSLOT2) --size=$$(($(SLOT2_SIZE))) ; \
	}

combined: multislot
	sh -c 'cat $(BOOTLOADER_BIN) $(BINSLOT1).meta $(BINSLOT1) $(BINSLOT2).meta $(BINSLOT2) > $(COMBINED_BIN)'

.PHONY: flash-multislot
flash-multislot: HEXFILE = $(COMBINED_BIN)
flash-multislot: combined $(FLASHDEPS)
	$(FLASHER) $(FFLAGS)

endif # multislot targets filter
