# atomics cause trouble with LTO
SRC_NOLTO := atomic_c11.c

# exclude submodule sources from *.c wildcard source selection
SRC := $(filter-out mbox.c msg.c thread_flags.c atomic_c11.c,$(wildcard *.c))

# enable submodules
SUBMODULES := 1

include $(RIOTBASE)/Makefile.base
