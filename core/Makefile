# exclude submodule sources from *.c wildcard source selection
SRC := $(filter-out mbox.c mbox_sync.c msg.c msg_bus.c thread.c thread_flags.c,$(wildcard *.c))

# enable submodules
SUBMODULES := 1

include $(RIOTBASE)/Makefile.base
