include ../Makefile.tests_common

USEMODULE += suit_v3
USEMODULE += riotboot_hdr
USEMODULE += embunit

CFLAGS += -DTHREAD_STACKSIZE_MAIN=\(6*THREAD_STACKSIZE_DEFAULT\)

CFLAGS += -g -DDEVELHELP -DLOG_LEVEL=LOG_DEBUG

PSEUDOMODULES += suit_v3_test
USEMODULE += suit_v3_test

BLOBS += $(wildcard manifests/manifest*.bin)

ifeq (native, $(BOARD))
  CFLAGS += -DFLASHPAGE_SIZE=256 -DFLASHPAGE_NUMOF=16
endif

BUILDDEPS += create_test_data

include $(RIOTBASE)/Makefile.include

export SUIT_TOOL
export SUIT_SEC

create_test_data: $(SUIT_SEC)
	@mkdir -p manifests
	sh create_test_data.sh
