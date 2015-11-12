x = Module("shell_commands", ["shell_commands.c", "sc_sys.c"])

# ( <module_name>, <sources>, <use_if-condition>, [uses])
commands = [
        ("sc_config", ["sc_id.c"], "config"),
        ("sc_mci", ["sc_disk.c"], "mci"),
        ("sc_ltc4150", ["sc_ltc4150.c"], "ltc4150"),
        ("sc_ps", ["sc_ps.c"], "ps"),
        ("sc_sht11", ["sc_sht11.c"], "sht11"),
        ("sc_lpc2387", ["sc_heap.c"], "lpc2387"),
        ("sc_random", ["sc_random.c"], "random", "xtimer"),
        ("sc_isl29020", ["sc_isl29020.c"], "isl29020"),
        ("sc_lps331ap", ["sc_lps331ap.c"], "lps331ap"),
        ("sc_l3g4200d", ["sc_l3g4200d.c"], "l3g4200d"),
        ("sc_lsm303dlhc", ["sc_lsm303dlhc.c"], "lsm303dlhc"),
        ("sc_at30tse75x", ["sc_at30tse75x.c"], "at30tse75x"),
        ("sc_gnrc_netif", ["sc_netif.c"], "gnrc_netif", ["gnrc_sixlowpan_netif", "gnrc_sixlowpan_iphc", "gnrc_ipv6_netif"]),
        ("sc_fib", ["sc_fib.c"], "fib", ["fib", "gnrc_netif", "gnrc_ipv6"]),
        ("sc_gnrc_ipv6_nc", ["sc_ipv6_nc.c"], "gnrc_ipv6_nc", "gnrc_netif"),
        ("sc_gnrc_ipv6_whitelist", ["sc_whitelist.c"], "gnrc_ipv6_whitelist"),
        ("sc_gnrc_icmpv6_echo", ["sc_icmpv6_echo.c"], "gnrc_icmpv6_echo and vtimer", "gnrc_icmpv6"),
        ("sc_gnrc_zep ipv6_addr", ["sc_zep.c"], "gnrc_zep ipv6_addr"),
        ("sc_gnrc_rpl", ["sc_gnrc_rpl.c"], "gnrc_rpl"),
        ("sc_gnrc_sixlowpan_nd_border_router", ["sc_gnrc_6ctx.c"],
            "gnrc_sixlowpan_nd_border_router and gnrc_sixlowpan_ctx",
            [ "gnrc_ndp_internal" ]),
        ("sc_saul_reg", ["sc_saul_reg.c"], "saul_reg"),
        ]

for tuple in commands:
    _deps = []
    if len(tuple)==3:
        module_name, sources, condition = tuple
        deps = condition
    else:
        module_name, sources, condition, deps = tuple
        _deps = deps
    Module(module_name, sources).use_if(condition).uses(deps)
    x.uses(module_name).uses(_deps or None)

## TODO
# sc_rtc.c sc_x86_lspci.c
