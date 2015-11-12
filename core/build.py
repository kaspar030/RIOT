Module().uses([ str(ctx.BOARD), str(ctx.CPU), "schedstatistics", "auto_init"])
PseudoModule("schedstatistics").needs("xtimer")
