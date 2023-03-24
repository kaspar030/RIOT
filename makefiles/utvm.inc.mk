## Example: UTVM_TAR += file.tar
UTVM_DIR_BASE ?= $(BUILD_DIR)/utvm
UTVM_NAME = $(UTVM_TAR:%.tar=%)
UTVM_MODEL_DIR = $(UTVM_DIR_BASE)/$(UTVM_NAME)
UTVM_METADATA_JSON := $(UTVM_TAR:%.tar=$(UTVM_DIR_BASE)/%/metadata.json)
UTVM_BIN = $(UTVM_NAME:%=$(BINDIR)/%/%.o)

CFLAGS += -I$(UTVM_MODEL_DIR)/runtime/include
CFLAGS += -I$(UTVM_MODEL_DIR)/codegen/host/include

.PRECIOUS: $(UTVM_MODEL_DIR)/.

$(UTVM_MODEL_DIR)/.:
	@mkdir -p $@

$(UTVM_METADATA_JSON): $(UTVM_TAR) $(UTVM_MODEl_DIR) | $(UTVM_TAR) $$(@D)/.
	tar --extract --file=$< --directory $(UTVM_MODEL_DIR) --touch

$(UTVM_BIN): $(UTVM_METADATA_JSON)
	$(QQ)"$(MAKE)" -C $(UTVM_MODEL_DIR)/codegen/host/src -f $(RIOTBASE)/makefiles/utvm/Makefile.utvm UTVM_MODULE_NAME=$(UTVM_NAME) UTVM_MODEL_DIR=$(UTVM_MODEL_DIR)

$(OBJC) $(OBJCXX): $(UTVM_BIN)
$(BASELIBS): | $(UTVM_METADATA_JSON)
all: $(UTVM_BIN)
