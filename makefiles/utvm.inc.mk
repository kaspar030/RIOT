## Example: UTVM_TAR += file.tar
UTVM_DIR_BASE ?= $(BUILD_DIR)/utvm
UTVM_NAME = $(UTVM_TAR:%.tar=%)
UTVM_MODEL_DIR = $(UTVM_DIR_BASE)/$(UTVM_NAME)
UTVM_METADATA_JSON = $(UTVM_TAR:%.tar=$(UTVM_DIR_BASE)/%/metadata.json)
UTVM_BIN = $(UTVM_NAME:%=$(BINDIR)/%/%.o)
UTVM_INCLUDES = $(UTVM_NAME:%=-I$(UTVM_DIR_BASE)/%/runtime/include)
UTVM_INCLUDES += $(UTVM_NAME:%=-I$(UTVM_DIR_BASE)/%/codegen/host/include)

CFLAGS += $(UTVM_INCLUDES)

.PRECIOUS: $(UTVM_MODEL_DIR)/.

$(UTVM_MODEL_DIR)/.:
	@mkdir -p $@

$(UTVM_METADATA_JSON): $(UTVM_TAR) $(UTVM_MODEl_DIR) | $(UTVM_TAR) $$(@D)/.
	tar --extract --file=$< --directory $(UTVM_MODEL_DIR) --touch

export $(UTVM_MODEL_DIR)
$(UTVM_BIN): $(UTVM_METADATA_JSON)
	$(QQ)"$(MAKE)" -C $(UTVM_MODEL_DIR)/codegen/host/src -f $(RIOTBASE)/makefiles/utvm/Makefile.utvm UTVM_MODULE_NAME=$(UTVM_NAME) UTVM_MODEL_DIR=$(UTVM_MODEL_DIR)

$(BASELIBS): | $(UTVM_BIN)
$(OBJC) $(OBJCXX): $(UTVM_BIN)
link: | $(UTVM_BIN)
