#
SUIT_COAP_BASEPATH ?= fw/$(BOARD)
SUIT_COAP_SERVER ?= localhost
SUIT_COAP_ROOT ?= coap://$(SUIT_COAP_SERVER)/$(SUIT_COAP_BASEPATH)
SUIT_COAP_FSROOT ?= $(RIOTBASE)/coaproot

#
SLOT0_SUIT_MANIFEST_LATEST ?= $(BINDIR_APP)-slot0.riot.suit.latest.bin
SLOT1_SUIT_MANIFEST_LATEST ?= $(BINDIR_APP)-slot1.riot.suit.latest.bin

$(SLOT0_SUIT_MANIFEST_LATEST): $(SLOT0_SUIT_MANIFEST)
	@ln -f -s $< $@

$(SLOT1_SUIT_MANIFEST_LATEST): $(SLOT1_SUIT_MANIFEST)
	@ln -f -s $< $@

SUIT_MANIFESTS := $(SLOT0_SUIT_MANIFEST) \
                  $(SLOT1_SUIT_MANIFEST) \
			      $(SLOT0_SUIT_MANIFEST_LATEST) \
                  $(SLOT1_SUIT_MANIFEST_LATEST)

define manifest-recipe
  $(RIOTBASE)/dist/tools/suit_v1/gen_manifest.py \
	  --raw -k $(SUIT_KEY) \
	  -u $(SUIT_COAP_ROOT)/$(notdir $<) \
	  -d $(SUIT_DEVICE_ID) \
	  -V $(SUIT_VERSION) \
	  -e $(SUIT_VENDOR) \
	  --no-sign \
	  -o $@ \
	  $<
endef

$(SLOT0_SUIT_MANIFEST): $(SLOT0_RIOT_BIN) $(SUIT_KEY)
	$(Q)$(manifest-recipe)

$(SLOT1_SUIT_MANIFEST): $(SLOT1_RIOT_BIN) $(SUIT_KEY)
	$(Q)$(manifest-recipe)

$(info $(SUIT_MANIFESTS))

suit/manifest: $(SUIT_MANIFESTS)

suit/publish: $(SUIT_MANIFESTS) $(SLOT0_RIOT_BIN) $(SLOT1_RIOT_BIN)
	@mkdir -p $(SUIT_COAP_FSROOT)/$(SUIT_COAP_BASEPATH)
	@cp -t $(SUIT_COAP_FSROOT)/$(SUIT_COAP_BASEPATH) $^
	@for file in $^; do \
		echo "published \"$$file\""; \
		echo "       as \"$(SUIT_COAP_ROOT)/$$(basename $$file)\""; \
	done

suit/notify:
	@test -n "$(SUIT_CLIENT)"
	$(Q)TARGET_SLOT=$$(aiocoap-client "coap://$(SUIT_CLIENT)/suit/slot/inactive") && \
	if [ "$$TARGET_SLOT" = 0 ]; then \
		export PAYLOAD="$(SUIT_COAP_ROOT)/$$(basename $(SLOT0_SUIT_MANIFEST_LATEST))"; \
	else \
		export PAYLOAD="$(SUIT_COAP_ROOT)/$$(basename $(SLOT1_SUIT_MANIFEST_LATEST))"; \
	fi && \
	aiocoap-client -m POST "coap://$(SUIT_CLIENT)/suit/trigger" \
		--payload "$${PAYLOAD}" && \
		echo "Triggered $(SUIT_CLIENT) to update slot $$TARGET_SLOT"
