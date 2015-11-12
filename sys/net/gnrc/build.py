ModuleDir("gnrc_conn", "conn")\
        .uses(["gnrc_ipv6", "gnrc_udp"])

ModuleDir("gnrc_conn_ip", "conn/ip").needs("gnrc_conn")
ModuleDir("gnrc_conn_udp", "conn/ip").needs(["gnrc_conn", "gnrc_udp"])

ModuleDir("gnrc_icmpv6", "network_layer/icmpv6")\
        .needs(["inet_csum", "gnrc_ipv6"])\
        .uses(["gnrc_icmpv6_echo", "gnrc_ndp"])


ModuleDir("gnrc_icmpv6_echo", "network_layer/icmpv6/echo")\
        .needs("gnrc_icmpv6")

ModuleDir("gnrc_ipv6", "network_layer/ipv6")\
        .needs(["inet_csum", "ipv6_addr", "gnrc_ipv6_hdr", "gnrc_ipv6_nc",
            "gnrc_ipv6_netif", "gnrc_pkt"])\
        .uses(["fib", "gnrc_icmpv6", "gnrc_ipv6_ext", "gnrc_ndp",
            "gnrc_ndp_router", "gnrc_ndp_host", "gnrc_sixlowpan_nd",
            "gnrc_sixlowpan_nd_router", "gnrc_sixlowpan", "gnrc_ndp_node",
            "gnrc_ipv6_whitelist", "gnrc_ipv6_router"])\
        .auto_init()

ModuleDir("gnrc_ipv6_ext", "network_layer/ipv6/ext")\
        .needs(["ipv6_ext", "gnrc_ipv6"])

ModuleDir("gnrc_ipv6_hdr", "network_layer/ipv6/hdr")\
        .needs(["ipv6_hdr", "gnrc_pktbuf"])\
        .uses(["gnrc_ipv6", "ipv6_addr"])

ModuleDir("gnrc_ipv6_nc", "network_layer/ipv6/nc")\
        .needs("ipv6_addr")\
        .uses(["gnrc_ndp_node", "gnrc_sixlowpan_nd_router"])

ModuleDir("gnrc_ipv6_netif", "network_layer/ipv6/netif")\
        .needs(["ipv6_addr", "gnrc_netif", "gnrc_netif_hdr", "bitfield",
            "vtimer"])\
        .uses(["gnrc_ndp", "gnrc_ndp_host", "gnrc_ndp_router",
            "gnrc_sixlowpan", "gnrc_sixlowpan_nd",
            "gnrc_sixlowpan_nd_border_router", "gnrc_sixlowpan_nd_router"])\
        .auto_init()

ModuleDir("gnrc_ipv6_whitelist", "network_layer/ipv6/whitelist")\
        .needs("ipv6_addr")

ModuleDir("gnrc_ndp", "network_layer/ndp")\
        .needs(["gnrc_icmpv6", "timex", "vtimer"])\
        .uses(["gnrc_ndp_node", "gnrc_sixlowpan_nd",
            "gnrc_sixlowpan_nd_router"])

ModuleDir("gnrc_ndp_host", "network_layer/ndp/host")\
        .needs(["gnrc_ndp_node", "random", "vtimer"])

ModuleDir("gnrc_ndp_internal", "network_layer/ndp/internal")\
        .needs(["gnrc_ndp", "gnrc_netapi", "random"])\
        .uses(["gnrc_sixlowpan_nd", "gnrc_sixlowpan_nd_router"])

ModuleDir("gnrc_ndp_node", "network_layer/ndp/node")\
        .needs(["gnrc_ndp_internal", "gnrc_ndp"])\
        .uses(["fib", "ipv6_ext_rh"])

ModuleDir("gnrc_ndp_router", "network_layer/ndp/router")\
        .needs(["gnrc_ndp_internal", "random", "gnrc_ndp_host"])

ModuleDir("gnrc_netapi", "netapi")\
        .needs("gnrc_pktbuf")

Module("gnrc_netdev2", sources="link_layer/netdev2/gnrc_netdev2.c")\
        .use_if("gnrc and netdev2")\
        .needs("gnrc_pktbuf")

Module("gnrc_netdev2_eth", sources="link_layer/netdev2/gnrc_netdev2_eth.c")\
        .needs(["gnrc_netdev2", "gnrc_ipv6"])\
        .use_if("netdev2_eth")

ModuleDir("gnrc_netif", "netif")\
        .uses("gnrc_ipv6_netif")

ModuleDir("gnrc_netif_hdr", "netif/hdr")
ModuleDir("gnrc_netreg", "netreg")\
        .uses(["gnrc_icmpv6", "gnrc_ipv6", "gnrc_tcp", "gnrc_udp"])

ModuleDir("gnrc_nettest", "nettest")\
        .needs(["gnrc_netapi", "gnrc_netreg", "gnrc_netif", "gnrc_pktbuf",
        "vtimer"])
ModuleDir("gnrc_nomac", "link_layer/nomac")

ModuleDir("gnrc_pkt", "pkt")

ModuleDir("gnrc_pktbuf_static", "pktbuf_static")\
        .uses("od")

ModuleDir("gnrc_pktdump", "pktdump")\
        .needs(["gnrc_pktbuf", "od"])\
        .uses(["gnrc_icmpv6", "gnrc_ipv6", "gnrc_netif", "gnrc_sixlowpan",
            "gnrc_tcp", "gnrc_udp"])\
        .auto_init()

ModuleDir("gnrc_rpl", "routing/rpl")\
        .needs(["fib", "gnrc_ipv6_router_default", "trickle", "xtimer", "gnrc_ipv6"])

ModuleDir("gnrc_rpl_srh", "routing/rpl/srh").uses("ipv6_ext_rh")

ModuleDir("gnrc_sixlowpan", "network_layer/sixlowpan")\
        .needs(["gnrc_ipv6", "gnrc_sixlowpan_netif", "sixlowpan"])\
        .uses(["gnrc_sixlowpan_frag", "gnrc_sixlowpan_iphc"])\
        .use_if("ieee802154 and gnrc_ipv6")\
        .auto_init()

ModuleDir("gnrc_sixlowpan_ctx", "network_layer/sixlowpan/ctx")\
        .needs(["ipv6_addr", "vtimer"])

ModuleDir("gnrc_sixlowpan_frag", "network_layer/sixlowpan/frag")\
        .needs(["gnrc_sixlowpan", "vtimer"])\
        .uses(["gnrc_sixlowpan_iphc"])

ModuleDir("gnrc_sixlowpan_iphc", "network_layer/sixlowpan/iphc")\
        .needs(["gnrc_sixlowpan", "gnrc_sixlowpan_ctx", "gnrc_sixlowpan_iphc_nhc"])

ModuleDir("gnrc_sixlowpan_nd", "network_layer/sixlowpan/nd")\
        .needs(["gnrc_ndp", "gnrc_ndp_internal", "gnrc_sixlowpan_ctx",
            "random", "vtimer"])\
        .uses(["fib", "ipv6_ext_rh", "gnrc_sixlowpan_nd_border_router",
            "gnrc_sixlowpan_nd_router"])

ModuleDir("gnrc_sixlowpan_nd_router", "network_layer/sixlowpan/nd/router")\
        .needs("gnrc_sixlowpan_nd")\
        .uses(["gnrc_ndp_router", "gnrc_sixlowpan_nd_border_router"])

ModuleDir("gnrc_sixlowpan_netif", "network_layer/sixlowpan/netif")\
        .uses(["gnrc_sixlowpan_iphc"])

ModuleDir("gnrc_slip", "link_layer/slip")\
        .auto_init()

ModuleDir("gnrc_udp", "transport_layer/udp")\
        .needs(["inet_csum", "udp", "gnrc"])\
        .uses(["gnrc_ipv6"])\
        .auto_init()

ModuleDir("gnrc_zep", "application_layer/zep")\
        .needs(["hashes", "ieee802154", "gnrc_udp", "vtimer"])\
        .needs("gnrc_sixlowpan")

PseudoModule("gnrc")\
        .needs(["gnrc_netapi", "gnrc_netreg", "gnrc_netif", "gnrc_netif_hdr",
            "gnrc_pktbuf"])

PseudoModule("gnrc_ipv6_default")\
        .needs(["gnrc_ipv6", "gnrc_icmpv6", "gnrc_ndp_host"])

PseudoModule("gnrc_ipv6_router")\
        .needs(["ipv6_addr"])

PseudoModule("gnrc_pktbuf")\
        .needs("gnrc_pktbuf_static")\
        .auto_init()

PseudoModule("gnrc_sixlowpan_router")\
        .needs("gnrc_sixlowpan_nd_router")\
        .use_if("ieee802154 and gnrc_ipv6_router")

PseudoModule("gnrc_ipv6_router_default")\
        .needs(["gnrc_ipv6_router", "gnrc_icmpv6", "gnrc_ndp_router"])

PseudoModule("gnrc_netif_default").needs("gnrc_netif")

PseudoModule("gnrc_sixlowpan_default")\
        .needs([ "gnrc_ipv6_default", "gnrc_sixlowpan", "gnrc_sixlowpan_nd",
        "gnrc_sixlowpan_frag", "gnrc_sixlowpan_iphc" ])\
        .use_if("ieee802154 and gnrc_ipv6_default")

PseudoModule("gnrc_sixlowpan_iphc_nhc")

PseudoModule("gnrc_sixlowpan_router_default")\
        .needs(["gnrc_ipv6_router_default", "gnrc_sixlowpan_router",\
               "gnrc_sixlowpan_frag", "gnrc_sixlowpan_iphc"])\
        .use_if("ieee802154 and gnrc_ipv6_router_default")

PseudoModule("gnrc_sixlowpan_border_router_default")\
        .needs(["gnrc_ipv6_router_default", "gnrc_sixlowpan_nd_border_router",
        "gnrc_sixlowpan_router", "gnrc_sixlowpan_frag", "gnrc_sixlowpan_iphc"])

PseudoModule("gnrc_sixlowpan_nd_border_router")\
        .needs("gnrc_sixlowpan_nd_router")

PseudoModule("gnrc_tcp") # there's no tcp module, this used to silence warnings from dependees
PseudoModule("ieee802154")
