Board(cpu="samd21", cpu_model="samr21g18a")

ctx._flasher = OpenocdFlash
ctx.OPENOCD_CONFIG = relpath("dist/openocd.cfg")

PseudoModule("saul_gpio_board")\
        .needs("saul_gpio")\
        .use_if("saul_default")

PseudoModule("netif_board")\
        .needs(["at86rf233", "gnrc_nomac"])\
        .use_if("gnrc_netif_default")
