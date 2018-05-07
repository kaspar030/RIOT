export FLASHER ?= $(RIOTBASE)/dist/tools/bossa/bossac
FFLAGS  ?= -p $(PORT) -e -i -w -v -b -R $(FLASHFILE)

FLASHFILE ?= $(BINFILE)

# some arduino boards need to toggle the serial interface a little bit to get
# them ready for flashing...
ifneq (,$(BOSSA_ARDUINO_PREFLASH))
  OS := $(shell uname)
  ifeq ($(OS),Linux)
    STTY_FLAG = -F
  else ifeq ($(OS),Darwin)
    STTY_FLAG = -f
  endif

  PREFLASHER ?= stty
  PREFFLAGS  ?= $(STTY_FLAG) $(PORT) raw ispeed 1200 ospeed 1200 cs8 -cstopb ignpar eol 255 eof 255
  FLASHDEPS += preflash
endif

# if we go with the default (BOSSA shipped with RIOT), we download and build
# the tool if not already done
ifeq ($(RIOTBASE)/dist/tools/bossa/bossac,$(FLASHER))
  FLASHDEPS += $(RIOTBASE)/dist/tools/bossa/bossac
endif
