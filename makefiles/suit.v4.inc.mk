#
SUIT_COAP_BASEPATH ?= fw/$(BOARD)
SUIT_COAP_SERVER ?= localhost
SUIT_COAP_ROOT ?= coap://$(SUIT_COAP_SERVER)/$(SUIT_COAP_BASEPATH)
SUIT_COAP_FSROOT ?= $(RIOTBASE)/coaproot

#
SUIT_MANIFEST ?= $(BINDIR_APP)-riot.suitv4.$(APP_VER).bin
SUIT_MANIFEST_LATEST ?= $(BINDIR_APP)-riot.suitv4.latest.bin
SUIT_MANIFEST_SIGNED ?= $(BINDIR_APP)-riot.suitv4_signed.$(APP_VER).bin
SUIT_MANIFEST_SIGNED_LATEST ?= $(BINDIR_APP)-riot.suitv4_signed.latest.bin

# allow notification of a specific APP_VER instead of latest
ifeq (,$(SUIT_NOTIFY_VERSION))
  SUIT_NOTIFY_MANIFEST ?= $(SUIT_MANIFEST_SIGNED_LATEST)
else
  SUIT_NOTIFY_MANIFEST = $(BINDIR_APP)-riot.suitv4_signed.$(SUIT_NOTIFY_VERSION).bin
endif

# Long manifest names require more buffer space when parsing
export CFLAGS += -DSOCK_URLPATH_MAXLEN=128

SUIT_VENDOR ?= "riot-os.org"
SUIT_SEQNR ?= $(APP_VER)
SUIT_DEVICE_ID ?= $(BOARD)

SUIT_KEY ?= $(BINDIR)/secret.key
SUIT_PUB ?= $(BINDIR)/public.key
SUIT_PUB_HDR ?= $(BINDIR)/public_key.h

# In the CI, building is done on a different host than running the tests.
# The tests re-publish different versions, so they require the keys.
TEST_EXTRA_FILES += $(SUIT_KEY) $(SUIT_PUB) $(SUIT_PUB_HDR)

$(SUIT_KEY) $(SUIT_PUB):
	@$(RIOTBASE)/dist/tools/suit_v4/gen_key.py

$(SUIT_PUB_HDR): $(SUIT_PUB)
	@xxd -i $(SUIT_PUB) > $@

suit/genkey: $(SUIT_KEY) $(SUIT_PUB) $(SUIT_PUB_HDR)

$(SUIT_MANIFEST): $(SLOT0_RIOT_BIN) $(SLOT1_RIOT_BIN)
	$(RIOTBASE)/dist/tools/suit_v4/gen_manifest.py \
	  --template $(RIOTBASE)/dist/tools/suit_v4/test-2img.json \
	  --urlroot $(SUIT_COAP_ROOT) \
	  --seqnr $(SUIT_SEQNR) \
	  --uuid-vendor $(SUIT_VENDOR) \
	  --uuid-class $(SUIT_DEVICE_ID) \
	  --offsets $(SLOT0_OFFSET),$(SLOT1_OFFSET) \
	  -o $@ \
	  $^

$(SUIT_MANIFEST_SIGNED): $(SUIT_MANIFEST)
	$(RIOTBASE)/dist/tools/suit_v4/sign-04.py \
	  $(SUIT_KEY) $(SUIT_PUB) $< $@

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
	@cp -t $(SUIT_COAP_FSROOT)/$(SUIT_COAP_BASEPATH) $^
	@for file in $^; do \
		echo "published \"$$file\""; \
		echo "       as \"$(SUIT_COAP_ROOT)/$$(basename $$file)\""; \
	done

suit/notify: | $(filter suit/publish, $(MAKECMDGOALS))
	@test -n "$(SUIT_CLIENT)" || { echo "error: SUIT_CLIENT unset!"; false; }
	aiocoap-client -m POST "coap://$(SUIT_CLIENT)/suit/trigger" \
		--payload "$(SUIT_COAP_ROOT)/$$(basename $(SUIT_NOTIFY_MANIFEST))" && \
		echo "Triggered $(SUIT_CLIENT) to update."

suit/delkeys:
	rm -f public_key.h public.key secret.key

suit/genkey:
	$(RIOTBASE)/dist/tools/suit_v4/gen_key.py

suit/keyhdr: suit/genkey $(SUIT_PUB_HDR)
