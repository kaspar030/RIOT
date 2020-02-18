#
SUIT_COAP_BASEPATH ?= fw/$(BOARD)
SUIT_COAP_SERVER ?= localhost
SUIT_COAP_ROOT ?= coap://$(SUIT_COAP_SERVER)/$(SUIT_COAP_BASEPATH)
SUIT_COAP_FSROOT ?= $(RIOTBASE)/coaproot

#
SUIT_MANIFEST ?= $(BINDIR_APP)-riot.suitv3.$(APP_VER).bin
SUIT_MANIFEST_LATEST ?= $(BINDIR_APP)-riot.suitv3.latest.bin
SUIT_MANIFEST_SIGNED ?= $(BINDIR_APP)-riot.suitv3_signed.$(APP_VER).bin
SUIT_MANIFEST_SIGNED_LATEST ?= $(BINDIR_APP)-riot.suitv3_signed.latest.bin

SUIT_NOTIFY_VERSION ?= latest
SUIT_NOTIFY_MANIFEST ?= $(APPLICATION)-riot.suitv3_signed.$(SUIT_NOTIFY_VERSION).bin

# Long manifest names require more buffer space when parsing
export CFLAGS += -DCONFIG_SOCK_URLPATH_MAXLEN=128

SUIT_VENDOR ?= "riot-os.org"
SUIT_SEQNR ?= $(APP_VER)
SUIT_CLASS ?= $(BOARD)

# path to suit-tool
SUIT_TOOL ?= python $(RIOTBASE)/dist/tools/suit_v3/suit-manifest-generator/bin/suit-tool

#
# SUIT encryption keys
#

# Specify key to use.
# Will use $(SUIT_KEY_DIR)/$(SUIT_KEY) $(SUIT_KEY_DIR)/$(SUIT_KEY).pub as
# private/public key files, similar to how ssh names its key files.
SUIT_KEY ?= default

ifeq (1, $(RIOT_CI_BUILD))
  SUIT_KEY_DIR ?= $(BINDIR)
else
  SUIT_KEY_DIR ?= $(RIOTBASE)/keys
endif

SUIT_SEC ?= $(SUIT_KEY_DIR)/$(SUIT_KEY).pem

SUIT_PUB_HDR = $(BINDIR)/riotbuild/public_key.h
SUIT_PUB_HDR_DIR = $(dir $(SUIT_PUB_HDR))
CFLAGS += -I$(SUIT_PUB_HDR_DIR)
BUILDDEPS += $(SUIT_PUB_HDR)

$(SUIT_SEC): $(CLEAN)
	@echo suit: generating key in $(SUIT_KEY_DIR)
	@mkdir -p $(SUIT_KEY_DIR)
	@$(RIOTBASE)/dist/tools/suit_v3/gen_key.py $(SUIT_SEC)

# set FORCE so switching between keys using "SUIT_KEY=foo make ..."
# triggers a rebuild even if the new key would otherwise not (because the other
# key's mtime is too far back).
$(SUIT_PUB_HDR): $(SUIT_SEC) FORCE | $(CLEAN)
	@mkdir -p $(SUIT_PUB_HDR_DIR)
	@$(SUIT_TOOL) pubkey -k $(SUIT_SEC) \
	  | '$(LAZYSPONGE)' $(LAZYSPONGE_FLAGS) '$@'

suit/genkey: $(SUIT_SEC)

#
$(SUIT_MANIFEST): $(SLOT0_RIOT_BIN) $(SLOT1_RIOT_BIN)
	$(RIOTBASE)/dist/tools/suit_v3/gen_manifest.py \
	  --urlroot $(SUIT_COAP_ROOT) \
	  --seqnr $(SUIT_SEQNR) \
	  --uuid-vendor $(SUIT_VENDOR) \
	  --uuid-class $(SUIT_CLASS) \
	  -o $@.tmp \
	  $(SLOT0_RIOT_BIN):$(SLOT0_OFFSET) \
	  $(SLOT1_RIOT_BIN):$(SLOT1_OFFSET)
	
	$(SUIT_TOOL) create -f suit -i $@.tmp -o $@

	rm -f $@.tmp


$(SUIT_MANIFEST_SIGNED): $(SUIT_MANIFEST) $(SUIT_SEC)
	$(SUIT_TOOL) sign -k $(SUIT_SEC) -m $(SUIT_MANIFEST) -o $@

$(SUIT_MANIFEST_LATEST): $(SUIT_MANIFEST)
	@ln -f -s $< $@

$(SUIT_MANIFEST_SIGNED_LATEST): $(SUIT_MANIFEST_SIGNED)
	@ln -f -s $< $@

SUIT_MANIFESTS := $(SUIT_MANIFEST) \
                  $(SUIT_MANIFEST_LATEST) \
                  $(SUIT_MANIFEST_SIGNED) \
                  $(SUIT_MANIFEST_SIGNED_LATEST)

suit/manifest: $(SUIT_MANIFESTS)

suit/publish: $(SUIT_MANIFESTS) $(SLOT0_RIOT_BIN) $(SLOT1_RIOT_BIN)
	@mkdir -p $(SUIT_COAP_FSROOT)/$(SUIT_COAP_BASEPATH)
	@cp $^ $(SUIT_COAP_FSROOT)/$(SUIT_COAP_BASEPATH)
	@for file in $^; do \
		echo "published \"$$file\""; \
		echo "       as \"$(SUIT_COAP_ROOT)/$$(basename $$file)\""; \
	done

suit/notify: | $(filter suit/publish, $(MAKECMDGOALS))
	@test -n "$(SUIT_CLIENT)" || { echo "error: SUIT_CLIENT unset!"; false; }
	aiocoap-client -m POST "coap://$(SUIT_CLIENT)/suit/trigger" \
		--payload "$(SUIT_COAP_ROOT)/$(SUIT_NOTIFY_MANIFEST)" && \
		echo "Triggered $(SUIT_CLIENT) to update."
