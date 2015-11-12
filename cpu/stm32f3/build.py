ctx.CPU_ARCH = "cortex-m4"
#ctx.CPU_ARCH = "cortex-m4f"

ctx.includes += relpath("include")

Module().needs(["cortexm_common", "periph_common"])

ModuleDir("periph")
subinclude("../cortexm_common")
subinclude("../stm32_common")
