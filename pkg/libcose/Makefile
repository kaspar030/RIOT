PKG_NAME=libcose
PKG_URL=https://github.com/bergzand/libcose
PKG_VERSION=c70be65fea2ba804dd28b7089d9f6c4312ce8452
PKG_LICENSE=LGPL

.PHONY: all

all:
	"$(MAKE)" -C $(PKG_BUILDDIR)/src -f $(CURDIR)/Makefile.libcose
	"$(MAKE)" -C $(PKG_BUILDDIR)/src/crypt -f $(CURDIR)/Makefile.libcose_crypt

include $(RIOTBASE)/pkg/pkg.mk
