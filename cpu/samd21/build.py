ctx.CPU_ARCH = "cortex-m0plus"

ctx.includes += relpath("include")

Module().needs(["cortexm_common", "sam21_periph_common"])

subinclude("periph")
subinclude("../cortexm_common")
subinclude("../sam21_common")
