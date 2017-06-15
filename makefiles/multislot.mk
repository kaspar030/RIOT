BOOTLOADER_ELF  := $(RIOTBASE)/bootloader/bin/$(BOARD)/bootloader.elf
BINSLOT0        := $(BOOTLOADER_ELF:%.elf=%.slot0.bin)
SLOTAPP         := $(ELFFILE:.elf=-slot)
ELFSLOT1        := $(SLOTAPP)1.elf
ELFSLOT2        := $(SLOTAPP)2.elf
IMAGE_SLOT1     := $(SLOTAPP)1-$(APP_ID)-$(APP_VERSION).bin
COMBINED_BIN    := $(ELFFILE:%.elf=%.combined.bin)

FIRMWARE_TOOLS   = $(RIOTBASE)/dist/tools/firmware
FIRMWARE         = $(FIRMWARE_TOOLS)/bin/firmware
PUBKEY_DIR       = $(RIOTBASE)/sys/include
GENMETA          = $(FIRMWARE) genmeta
GENKEYS          = $(FIRMWARE) genkeys
VERIFY           = $(FIRMWARE) verify

SECKEY ?= $(BINDIR)/key.sec
PUBKEY ?= $(BINDIR)/key.pub

MAKETARGETS += combined flash-multislot

ifneq (1,$(RIOT_BOOTLOADER))
  ROM_LEN := $(SLOT_LEN)-$(FIRMWARE_METADATA_SIZE)
  link: $(PUBKEY_DIR)/ota_pubkey.h
endif

ROM_OFFSET_SLOT_1 := $(RIOT_BOOTLOADER_SIZE)+$(FIRMWARE_METADATA_SIZE)
ROM_OFFSET_SLOT_2 := $(RIOT_BOOTLOADER_SIZE)+$(SLOT_LEN)+$(FIRMWARE_METADATA_SIZE)

multislot: multislot-1 multislot-2
verify: verify-1 verify-2

# Force rebuild of bootloader
.PHONY: FORCE

FORCE:

$(BOOTLOADER_ELF): FORCE
	$(Q)env -i QUIET=$(QUIET) PATH=$(PATH) BOARD=$(BOARD) RIOT_BOOTLOADER=1 \
	make -C 	$(RIOTBASE)/bootloader all

$(FIRMWARE):
	$(Q)env -i PATH=$(PATH) CFLAGS+=-DFIRMWARE_METADATA_SIZE=$(FIRMWARE_METADATA_SIZE) \
	make clean all -C $(FIRMWARE_TOOLS)

$(PUBKEY_DIR)/ota_pubkey.h: $(PUBKEY)
	$(Q)cp $< $(BINDIR)/ota_public_key
	$(Q)cd $(BINDIR); xxd -i ota_public_key > $@

$(SECKEY) $(PUBKEY): $(FIRMWARE)
	$(Q)$(GENKEYS) $(SECKEY) $(PUBKEY)

$(BINSLOT0): $(BOOTLOADER_ELF)
	$(Q)$(OBJCOPY) -Obinary $< $@
	$(Q)truncate -s $$(($(RIOT_BOOTLOADER_SIZE))) $@

multislot-1 multislot-2: multislot-%: $(SLOTAPP)%-$(APP_ID)-$(APP_VERSION).bin
verify-1 verify-2: verify-%: $(SLOTAPP)%-$(APP_ID)-$(APP_VERSION).bin

$(SLOTAPP)%.bin: $(SLOTAPP)%.elf
	$(Q)$(OBJCOPY) -Obinary $< $@

$(SLOTAPP)%.bin.meta: $(SLOTAPP)%.bin $(SECKEY) $(FIRMWARE)
	$(Q)$(GENMETA) $< $(APP_VERSION) $(APP_ID) \
	    $$(($(ROM_START_ADDR)+$(ROM_OFFSET_SLOT_$*))) $(SECKEY) $@

$(SLOTAPP)%-$(APP_ID)-$(APP_VERSION).bin: $(SLOTAPP)%.bin.meta $(SLOTAPP)%.bin
	$(Q)cat $? > $@

$(SLOTAPP)%.elf: link
	$(eval ROM_OFFSET:=$(ROM_OFFSET_SLOT_$*))
	$(Q)$(_LINK) -o $@


combined: $(BINSLOT0) multislot-1
	$(Q)sh -c 'cat $(BINSLOT0) $(IMAGE_SLOT1) > $(COMBINED_BIN)'

.PHONY: flash-multislot verify-% firmware-tools-clean

ifneq (, $(filter $(BOARD),iotlab-m3))
  ifneq (, $(filter flash-multislot, $(MAKECMDGOALS)))
    export BINFILE = $(COMBINED_BIN)
    FFLAGS = flash-bin
    FLASH_ADDR = $(ROM_START_ADDR)
  endif
else
  flash-multislot: HEXFILE = $(COMBINED_BIN)
endif

flash-multislot: combined $(FLASHDEPS)
	$(FLASHER) $(FFLAGS)

verify-%: $(FIRMWARE) $(PUBKEY)
	$(Q)$(VERIFY) $(SLOTAPP)$*-$(APP_ID)-$(APP_VERSION).bin $(PUBKEY)

clean-firmware-tools:
	$(Q)env -i PATH=$(PATH) make -C $(FIRMWARE_TOOLS) clean

clean-bootloader:
	@env -i PATH=$(PATH) BOARD=$(BOARD) make -C $(RIOTBASE)/bootloader clean

clean-multislot: clean-firmware-tools clean-bootloader
