PKG_NAME=libcose
PKG_URL=https://github.com/bergzand/libcose
PKG_VERSION=0227a1dc2428bd90cb5cbc491ccca7d1369a1357
PKG_LICENSE=LGPL

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR)/src -f $(CURDIR)/Makefile.libcose
	"$(MAKE)" -C $(PKG_BUILDDIR)/src/crypt -f $(CURDIR)/Makefile.libcose_crypt

include $(RIOTBASE)/pkg/pkg.mk
