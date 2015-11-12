ctx.includes += relpath("include")
ctx.defines += "CPU_FAM_%s" % (str(ctx.CPU_FAM).upper().replace('-','_'))
ctx.defines += "CPU_MODEL_" + str(ctx.CPU_MODEL).upper()
subinclude("periph")
