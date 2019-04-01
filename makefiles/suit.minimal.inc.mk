#
SUIT_COAP_BASEPATH ?= fw/$(BOARD)
SUIT_COAP_SERVER ?= localhost
SUIT_COAP_ROOT ?= coap://$(SUIT_COAP_SERVER)/$(SUIT_COAP_BASEPATH)
SUIT_COAP_FSROOT ?= $(RIOTBASE)/coaproot

#
SUIT_MANIFEST ?= $(BINDIR_APP)-$(APP_VER).bin
SUIT_MANIFEST_LATEST ?= $(BINDIR_APP)-latest.bin

SUIT_SEQNR ?= $(APP_VER)
SUIT_VENDOR_ID ?= "riot-os.org"
SUIT_CLASS_ID ?= $(BOARD)
SUIT_KEY ?= secret.key

$(SUIT_MANIFEST): $(SLOT0_RIOT_BIN) $(SLOT1_RIOT_BIN)
	cd $(BINDIR) && $(RIOTBASE)/dist/tools/suit_minimal/mkmanifest \
	  $@ \
	  $(SUIT_SEQNR) \
	  $(SUIT_COAP_ROOT)/ \
	  $(notdir $(SLOT0_RIOT_BIN)) \
	  $(notdir $(SLOT1_RIOT_BIN)) \
	  $(SUIT_VENDOR_ID) \
	  $(SUIT_CLASS_ID)

$(SUIT_MANIFEST_LATEST): $(SUIT_MANIFEST)
	@ln -f -s $< $@

SUIT_MANIFESTS := $(SUIT_MANIFEST) \
                  $(SUIT_MANIFEST_LATEST) \

suit/manifest: $(SUIT_MANIFESTS)

suit/publish: $(SUIT_MANIFESTS) $(SLOT0_RIOT_BIN) $(SLOT1_RIOT_BIN)
	@mkdir -p $(SUIT_COAP_FSROOT)/$(SUIT_COAP_BASEPATH)
	@cp -t $(SUIT_COAP_FSROOT)/$(SUIT_COAP_BASEPATH) $^
	@for file in $^; do \
		echo "published \"$$file\""; \
		echo "       as \"$(SUIT_COAP_ROOT)/$$(basename $$file)\""; \
	done

suit/notify: | $(filter suit/publish, $(MAKECMDGOALS))
	@test -n "$(SUIT_CLIENT)"
	aiocoap-client -m POST "coap://$(SUIT_CLIENT)/suit/trigger" \
		--payload "$(SUIT_COAP_ROOT)/$$(basename $(SUIT_MANIFEST_LATEST))" && \
		echo "Triggered $(SUIT_CLIENT) to update."

suit/genkey:
	$(RIOTBASE)/dist/tools/suit_v1/gen_key.py
