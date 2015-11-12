ctx.CPU_ARCH = "cortex-m3"
ctx.CPU_FAM = "stm32f1"

ctx.includes += relpath("include")

Module().needs(["cortexm_common", "periph_common"])

subinclude("periph")
subinclude("../cortexm_common")
subinclude("../stm32_common")
