Module()

ctx.includes += "cpu/atmega_common/include"
ctx.includes += "sys/libc/include"
ctx.CFLAGS += "-isystemcpu/atmega_common/avr-libc-extra"
ctx.CFLAGS += "-fno-common -fno-delete-null-pointer-checks"

ctx.LINKFLAGS += "-Wl,--gc-sections -static -lgcc -e reset_handler"

ctx.CC="avr-gcc"
ctx.LINK="avr-gcc"
