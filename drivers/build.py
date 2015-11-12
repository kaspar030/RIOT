ModuleDir("at86rf2xx")\
        .needs(["gnrc_nomac", "periph", "xtimer", "ieee802154"])\
        .uses(["at86rf233", "at86rf212b"])\
        .add_includes("drivers/at86rf2xx/include")

PseudoModule("at86rf233").needs("at86rf2xx")
PseudoModule("at86rf212b").needs("at86rf2xx")

ModuleDir("isl29020").add_includes("drivers/isl29020/include")
ModuleDir("l3g4200d").add_includes("drivers/l3g4200d/include")
ModuleDir("lps331ap").add_includes("drivers/lps331ap/include")
ModuleDir("lsm303dlhc").add_includes("drivers/lsm303dlhc/include")
ModuleDir("periph_common").uses(ctx.BOARD)

ModuleDir("encx24j600").needs("netdev2_eth")
ModuleDir("netdev2_eth").uses("gnrc_netdev2_eth")

drivers = [
    "adt7310",
    "at30tse75x",
    "cc110x",
    "hdc1000",
    "hih6130",
    "ina220",
    "isl29125",
    "kw2xrf",
    "lis3dh",
    "lm75a",
    "ltc4150",
    "mag3110",
    "mma8652",
    "mpl3115a2",
    "mpu9150",
    "mq3",
    "nrf24l01p",
    "nvram_spi",
    "pcd8544",
    "pir",
    "rgbled",
    "servo",
    "sht11",
    "srf02",
    "srf08",
    "tcs37727",
    "tmp006",
    "xbee",
]

for driver in drivers:
    ModuleDir(driver)

subinclude("saul")
