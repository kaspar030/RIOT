ctx.defines += "DONT_USE_CMSIS_INIT"
ctx.includes += relpath("include")
subinclude("periph")
