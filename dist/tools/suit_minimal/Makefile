all: mkmanifest

CFLAGS += -Wall

RIOTBASE ?= .
S := $(RIOTBASE)/sys/suit/minimal/
SRCS := mkmanifest.c $(RIOTBASE)/sys/suit/minimal/manifest.c

mkmanifest: $(SRCS) $(RIOTBASE)/sys/include/suit/minimal/manifest.h
	gcc $(CFLAGS) -I$(RIOTBASE)/sys/include $(SRCS) -o $@
