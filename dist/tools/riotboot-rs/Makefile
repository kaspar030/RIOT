PKG_NAME=riotboot-rs
PKG_URL=https://github.com/future-proof-iot/riotboot-rs.git
PKG_VERSION=c2bb21f0281dbf403f888de8ae0c8802b4f583ce
PKG_LICENSE=LGPL-2.1

PKG_SOURCE_LOCAL ?= /home/francisco/workspace/riotboot-rs/

include $(RIOTBASE)/pkg/pkg.mk

ifneq (,$(filter cortex-m0%,$(CPU_CORE)))
  CARGO_TARGET ?= thumbv6m-none-eabi
else ifneq (,$(filter cortex-m3,$(CPU_CORE)))
  CARGO_TARGET ?= thumbv7m-none-eabi
else ifneq (,$(filter cortex-m4 cortex-m7,$(CPU_CORE)))
  CARGO_TARGET ?= thumbv7em-none-eabi
else ifneq (,$(filter cortex-m4f cortex-m7f,$(CPU_CORE)))
  CARGO_TARGET ?= thumbv7em-none-eabihf
else ifneq (,$(filter cortex-m23,$(CPU_CORE)))
  CARGO_TARGET ?= thumbv8m.base-none-eabi
else ifneq (,$(filter cortex-m33,$(CPU_CORE)))
  CARGO_TARGET ?= thumbv8m.main-none-eabi
else ifneq (,$(filter cortex-m33f,$(CPU_CORE)))
  CARGO_TARGET ?= thumbv8m.main-none-eabihf
else
  $(error unsupported CPU_CORE)
endif

CARGO_BUILD_FLAGS ?= --features verified

all:
	@$(Q)mkdir -p $(BINDIR)
	@$(Q) cd $(PKG_SOURCE_DIR) \
	      && rustup target add $(CARGO_TARGET) \
	      && cargo build --release  --target $(CARGO_TARGET) $(CARGO_BUILD_FLAGS)
	@$(Q) cp $(PKG_SOURCE_DIR)/target/$(CARGO_TARGET)/release/riotboot-rs $(BINDIR)/riotboot-rs.elf
	@$(Q) $(OBJCOPY) -O binary $(BINDIR)/riotboot-rs.elf $(BINDIR)/riotboot-rs.bin

