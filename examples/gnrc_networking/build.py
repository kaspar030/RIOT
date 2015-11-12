#InsufficientMemory({"airfy-beacon", "chronos", "msb-430", "msb-430h", "nrf51dongle",
#                          "nrf6310", "nucleo-f334", "pca10000", "pca10005", "spark-core",
#                          "stm32f0discovery", "telosb", "weio", "wsn430-v1_3b", "wsn430-v1_4",
#                          "yunjia-nrf51822", "z1"})

Application().needs([
    # automatically initialize modules:
    "auto_init",

    # Include packages that pull up and auto-init the link layer.
    # NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
    "gnrc_netif_default",
    "auto_init_gnrc_netif",

    # Specify the mandatory networking modules for IPv6 and UDP
    "gnrc_ipv6_router_default",
    "gnrc_udp",

    # Add a routing protocol
    "gnrc_rpl",

    # This application dumps received packets to STDIO using the pktdump module
    "gnrc_pktdump",

    # Additional networking modules that can be dropped if not needed
    "gnrc_icmpv6_echo",

    # Add also the shell, some shell commands
    "shell",
    "shell_commands",
    "ps",
     ])


# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
ctx.defines += "DEVELHELP"
