Module().uses(ctx.BOARD).needs("newlib")

ctx.includes += relpath("include")
ctx.includes += "sys/libc/include"

if str(ctx.CPU_ARCH) == "cortex-m0plus":
    ctx.defines += "CPU_ARCH_CORTEX_M0PLUS"
elif str(ctx.CPU_ARCH) == "cortex-m3":
    ctx.defines += "CPU_ARCH_CORTEX_M3"
elif str(ctx.CPU_ARCH) == "cortex-m4":
    ctx.defines += "CPU_ARCH_CORTEX_M4"

ctx.defines += "CPU_MODEL_%s" % str(ctx.CPU_MODEL).upper()
ctx.defines += "COREIF_NG=1"

machine_flags = "-mthumb -mno-thumb-interwork -mfloat-abi=soft -mlittle-endian -mcpu=%s" % ctx.CPU_ARCH

ctx.CFLAGS += machine_flags
ctx.LINKFLAGS += machine_flags

ctx.LINKFLAGS += "-Lcpu/cortexm_common/ldscripts"
ctx.LINKFLAGS += "-Tcpu/%s/ldscripts/%s.ld -Wl,--fatal-warnings" % (ctx.CPU, ctx.CPU_MODEL)
ctx.LINKFLAGS += "-static -lgcc -nostartfiles"
ctx.LINKFLAGS += "-Wl,--gc-sections"

ctx.CC ="arm-none-eabi-gcc"
ctx.LINK = "arm-none-eabi-gcc"
