Module("cpu_native").uses([ctx.BOARD, "netdev2_tap"])

ctx.CFLAGS += "-m32"
ctx.LINKFLAGS += "-m32"

ctx.libs += "dl"
ctx.defines += "NATIVE_INCLUDES"
ctx.includes += relpath("include")

ModuleDir("periph")

ModuleDir("netdev2_tap")\
        .needs(["netif", "netdev2_eth"])\
        .use_if("gnrc_netif_default")
