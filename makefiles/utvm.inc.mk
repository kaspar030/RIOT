## Example: UTVM_TAR += file.tar
UTVM_DIR_BASE ?= $(BUILD_DIR)/utvm
UTVM_NAME = $(UTVM_TAR:%.tar=%)
UTVM_MODEL_DIR = $(UTVM_DIR_BASE)/$(UTVM_NAME)
UTVM_METADATA_JSON := $(UTVM_TAR:%.tar=$(UTVM_DIR_BASE)/%/metadata.json)

.PRECIOUS: $(UTVM_MODEL_DIR)/.

$(UTVM_MODEL_DIR)/.:
	@mkdir -p $@

$(UTVM_METADATA_JSON): $(UTVM_TAR) $(UTVM_MODEl_DIR) | $(UTVM_TAR) $$(@D)/.
	tar --extract --file=$< --directory $(UTVM_MODEL_DIR) --touch

all: $(UTVM_METADATA_JSON)

$(OBJC) $(OBJCXX): $(UTVM_METADATA_JSON)
