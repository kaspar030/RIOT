/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *               2015 Martin Landsmann <Martin.Landsmann@HAW-Hamburg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_shell_commands
 * @{
 *
 * @file
 * @brief       Provides shell commands to manage and show the IPV6 routing table
 *
 * @author      2016 Kaspar Schleiser <kaspar@schleiser.de>
 * @author      2015 Martin Landsmann <Martin.Landsmann@HAW-Hamburg.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "thread.h"
#include "net/af.h"
#ifdef MODULE_GNRC_NETIF
#include "net/gnrc/netif.h"
#endif
#include "net/ipv6/rt.h"
#include "net/gnrc/ipv6.h"

#define INFO1_TXT "route add <destination> [via <next hop>] [dev <device>]"
#define INFO2_TXT " [lifetime <lifetime>]"
#define INFO3_TXT "       <destination> - the destination address\n" \
                  "       <next hop>    - the address of the next-hop towards the <destination>\n" \
                  "                       (optional when specifying on-link routes)\n" \
                  "       <device>      - the device id of the Interface to use.\n"
#define INFO4_TXT "       <lifetime>    - optional lifetime in ms when the entry automatically invalidates\n"
#define INFO5_TXT "route del <destination>\n" \
                  "       <destination> - the destination address of the entry to be deleted\n"

static void _routing_usage(int info)
{
    switch (info) {
        case 1: {
            puts("\nbrief: adds a new entry to the RT.\nusage: "
                 INFO1_TXT "\n" INFO3_TXT);
            break;
        }

        case 2: {
            puts("\nbrief: adds a new entry to the RT.\nusage: "
                 INFO1_TXT INFO2_TXT "\n" INFO3_TXT INFO4_TXT);
            break;
        }

        case 3: {
            puts("\nbrief: deletes an entry from the RT.\nusage: "
                 INFO5_TXT);
            break;
        }

        default:
            break;
    };
}

static int _split_prefix_len(char *dest)
{
    char *prefix_start = dest;
    while(*++prefix_start) {
        if (*prefix_start == '/') {
            break;
        }
    }
    *prefix_start = '\0';
    prefix_start++;

    return (*prefix_start) ? atoi(prefix_start) : 128;
}

static void _routing_add(char *dest, const char *next, kernel_pid_t pid, uint32_t lifetime)
{
    ipv6_addr_t dest_addr;
    ipv6_addr_t next_hop_addr;
    unsigned prefix_len = _split_prefix_len(dest);

    ipv6_addr_from_str(&dest_addr, dest);

    if (next) {
        ipv6_addr_from_str(&next_hop_addr, next);
    }

    /* Set the prefix flag for a network */
#if 0
    dst_flags |= RT_FLAG_NET_PREFIX;
    for (size_t i = 0; i < dst_size; ++i) {
        if (dst[i] != 0) {
            /* and clear the bit if its not the default route */
            dst_flags = (dst_flags & ~RT_FLAG_NET_PREFIX);
            break;
        }
    }
#endif

    ipv6_rt_put(&dest_addr, prefix_len, next ? &next_hop_addr : NULL, pid, lifetime);
}

int _routing_route_handler(int argc, char **argv)
{
    /* e.g. route right now dont care about the adress/protocol family */
    if (argc == 1) {
        ipv6_rt_print();
        return 0;
    }

    /* e.g. firoute [add|del] */
    if (argc == 2) {
        if ((strcmp("add", argv[1]) == 0)) {
            _routing_usage(2);
        }
        else if ((strcmp("del", argv[1]) == 0)) {
            _routing_usage(3);
        }
        else {
            puts("\nunrecognized parameter1.\nPlease enter route [add|del] for more information.");
        }

        return 1;
    }

    if (argc > 2 && !((strcmp("add", argv[1]) == 0) || (strcmp("del", argv[1]) == 0))) {
        puts("\nunrecognized parameter2.\nPlease enter route [add|del] for more information.");
        return 1;
    }

    /* e.g. route del <destination> */
    if (argc == 3) {
        unsigned prefix_len = _split_prefix_len(argv[2]);
        ipv6_addr_t addr;
        ipv6_addr_from_str(&addr, argv[2]);

        ipv6_rt_del(&addr, prefix_len);

        return 0;
    }

#ifdef MODULE_GNRC_NETIF
    /* e.g. route add <destination> via <next hop> */
    if ((argc == 5) && (strcmp("add", argv[1]) == 0) && (strcmp("via", argv[3]) == 0)) {
        kernel_pid_t ifs[GNRC_NETIF_NUMOF];
        size_t ifnum = gnrc_netif_get(ifs);
        if (ifnum == 1) {
            _routing_add(argv[2], argv[4], ifs[0], (uint32_t)IPV6_RT_LIFETIME_NOEXPIRE);
        }
        else {
            _routing_usage(1);
            return 1;
        }

        return 0;
    }

    /* e.g. route add <destination> dev <iface> */
    if ((argc == 5) && (strcmp("add", argv[1]) == 0) && (strcmp("dev", argv[3]) == 0)) {
        _routing_add(argv[2], NULL, (kernel_pid_t)atoi(argv[4]), (uint32_t)IPV6_RT_LIFETIME_NOEXPIRE);
        return 0;
    }

    /* e.g. route add <destination> via <next hop> lifetime <lifetime> */
    if ((argc == 7) && (strcmp("add", argv[1]) == 0) && (strcmp("via", argv[3]) == 0)
            && (strcmp("lifetime", argv[5]) == 0)) {
        kernel_pid_t ifs[GNRC_NETIF_NUMOF];
        size_t ifnum = gnrc_netif_get(ifs);
        if (ifnum == 1) {
            _routing_add(argv[2], argv[4], ifs[0], (uint32_t)atoi(argv[6]));
        }
        else {
            _routing_usage(1);
            return 1;
        }

        return 0;
    }
#endif

    /* e.g. route add <destination> via <next hop> dev <device> */
    if (argc == 7) {
        if ((strcmp("add", argv[1]) == 0) && (strcmp("via", argv[3]) == 0)
            && (strcmp("dev", argv[5]) == 0)) {
            _routing_add(argv[2], argv[4], (kernel_pid_t)atoi(argv[6]), (uint32_t)IPV6_RT_LIFETIME_NOEXPIRE);
        }
        else {
            _routing_usage(1);
            return 1;
        }

        return 0;
    }

    /* e.g. route add <destination> via <next hop> dev <device> lifetime <lifetime> */
    if (argc == 9) {
        if ((strcmp("add", argv[1]) == 0) && (strcmp("via", argv[3]) == 0)
            && (strcmp("dev", argv[5]) == 0)
            && (strcmp("lifetime", argv[7]) == 0)) {
            _routing_add(argv[2], argv[4], (kernel_pid_t )atoi(argv[6]), (uint32_t)atoi(argv[8]));
        }
        else {
            _routing_usage(2);
            return 1;
        }

        return 0;
    }

    puts("\nunrecognized parameters.\nPlease enter route [add|del] for more information.");
    return 1;
}
