all: mkmanifest

CFLAGS += -Wall -g

RIOTBASE ?= .
S := $(RIOTBASE)/sys/suit/minimal/
SRCS := mkmanifest.c \
			$(RIOTBASE)/sys/suit/minimal/manifest.c \
			$(RIOTBASE)/sys/uuid/uuid.c \
			$(RIOTBASE)/sys/fmt/fmt.c \
			$(RIOTBASE)/sys/hashes/md5.c \
			$(RIOTBASE)/sys/hashes/sha1.c \
			$(RIOTBASE)/sys/random/random.c \
			$(RIOTBASE)/sys/random/xorshift.c \
			$(RIOTBASE)/core/bitarithm.c \
			#

mkmanifest: $(SRCS) $(RIOTBASE)/sys/include/suit/minimal/manifest.h
	gcc $(CFLAGS) -I. -I$(RIOTBASE)/sys/include -I$(RIOTBASE)/core/include $(SRCS) -o $@
