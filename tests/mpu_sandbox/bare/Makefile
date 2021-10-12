# clang config
CC = clang
OBJCOPY = llvm-objcopy
CFLAGS_ARCH = -target thumbv7m-none-eabi -g
CFLAGS = $(CFLAGS_ARCH) -Os -fropi -frwpi $(INCLUDES)
LINKFLAGS = $(CFLAGS_ARCH) -fpie -nostdlib -Wl,-Tcortexm_bare.ld -Wl,-Map=bare.map -Wl,--gc-sections
INCLUDES += -I$(RIOTBASE)/cpu/cortexm_common/include
INCLUDES += -I$(RIOTBASE)/sys/include/uapi
RIOTBASE ?= ../../..

all: bare.bin

clean:
	rm -f bare.elf bare.o

bare.o: bare.c Makefile
	$(CC) $(CFLAGS) -o $@ -c $<

bare.elf: bare.o cortexm_bare.ld Makefile
	$(CC) $(CFLAGS) $(LINKFLAGS) -o $@ $<

bare.bin: bare.elf
	$(OBJCOPY) -O binary $< $@
