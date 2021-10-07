CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
CFLAGS_ARCH = -mcpu=cortex-m4 -mlittle-endian -mthumb -mfloat-abi=soft
CFLAGS = $(CFLAGS_ARCH) -Os -fpie -msingle-pic-base -mpic-register=r9 -mno-pic-data-is-text-relative
LINKFLAGS = $(CFLAGS_ARCH) -nostdlib -Wl,-Tcortexm_bare.ld

all: bare.bin

clean:
	rm -f bare.elf bare.o

bare.o: bare.c Makefile
	$(CC) $(CFLAGS) -o $@ -c $<

bare.elf: bare.o cortexm_bare.ld Makefile
	$(CC) $(CFLAGS) $(LINKFLAGS) -o $@ $<

bare.bin: bare.elf
	$(OBJCOPY) -O binary $< $@
