ModuleDir("fib", "network_layer/fib")\
        .needs(["universal_address", "xtimer"])\
        .uses(["ipv6_addr"])\
        .add_includes(["sys/posix/include", "sys/net/include"])\
        .auto_init()

ModuleDir("inet_csum", "crosslayer/inet_csum")\
        .uses("od")

ModuleDir("ipv4_addr", "network_layer/ipv4/addr")

ModuleDir("ipv6_addr", "network_layer/ipv6/addr")\
        .uses(["ipv4_addr"])

PseudoModule("ipv6_ext")
ModuleDir("ipv6_ext_rh", "network_layer/ipv6/ext/rh")\
        .uses(["gnrc_rpl_srh"])

ModuleDir("ipv6_hdr", "network_layer/ipv6/hdr").needs("inet_csum")
ModuleDir("netopt", "crosslayer/netopt").add_includes(["sys/net/include"])
ModuleDir("nhdp", "routing/nhdp").add_includes(["sys/net/include", "sys/net/routing/nhdp"])
ModuleDir("sixlowpan", "network_layer/sixlowpan")
ModuleDir("udp", "transport_layer/udp")

PseudoModule("netif")

subinclude("gnrc")
