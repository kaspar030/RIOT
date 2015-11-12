Module().needs([ "atmega_common", "uart_stdio" ])

ctx.includes += relpath("include")

machine_flags = "-mmcu=atmega2560"
ctx.CFLAGS += machine_flags
ctx.LINKFLAGS += machine_flags

ctx.defines += "COREIF_NG=1"

ModuleDir("periph")
subinclude("../atmega_common")
