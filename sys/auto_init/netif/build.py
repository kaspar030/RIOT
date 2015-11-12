x = PseudoModule("auto_init_gnrc_netif").auto_init()

#Module("auto_init_netdev2_tap", "auto_init_netdev2_tap.c")\
#        .uses(["netdev2_tap", "gnrc_netdev2_eth"])\
#        .use_if("auto_init and auto_init_gnrc_netif and netdev2_tap")\
#        .auto_init()

devs = [
        "at86rf2xx",
        "cc110x",
        "enc28j60",
        "encx24j600",
        "kw2xrf",
        "netdev2_tap",
        "slip",
        "xbee",
        ]

for dev in devs:
    module_name = "auto_init_" + dev
    source = module_name + ".c"
    Module(module_name, source)\
            .uses(dev)\
            .use_if("auto_init_gnrc_netif and " + dev)\
            .auto_init()

    x.uses(module_name)
