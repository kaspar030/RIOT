app = Application()

# Uncomment this to enable scheduler statistics for ps:
#ctx.defines += SCHEDSTATISTICS

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
ctx.defines += "DEVELHELP"

# Modules to include:
app.needs([
    "auto_init",
    "ps",
    "shell",
    "shell_commands",
    # include and auto-initialize all available sensors
    "saul_reg", "saul_default", "auto_init_saul",
    ])

# modules that will be used if selected, but app doesn't depend on:
app.uses(["config"])

BOARD_PROVIDES_NETIF = {
        "airfy-beacon", "fox", "iotlab-m3", "mulle", "native", "nrf51dongle",
        "nrf6310", "pba-d-01-kw2x", "pca10000", "pca10005", "saml21-xpro",
        "samr21-xpro", "spark-core", "yunjia-nrf51822" }

if str(ctx.BOARD) in BOARD_PROVIDES_NETIF:
    app.needs([
  # Use modules for networking
  # gnrc is a meta module including all required, basic gnrc networking modules
  "gnrc",
  # use the default network interface for the board
  "gnrc_netif_default",
  # automatically initialize the network interface
  "auto_init_gnrc_netif",
  # the application dumps received packets to stdout
  "gnrc_pktdump",
  ])

#FEATURES_OPTIONAL += config
#FEATURES_OPTIONAL += periph_rtc

# include board-specific default modules
board_extra_modules = {
    "msb-430"   : [ "sht11" ],
    "msba2"     : [ "sht11", "mci", "random" ],
    }

app.needs(board_extra_modules.get(str(ctx.BOARD)))
