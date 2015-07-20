/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_nanonet
 *
 * @{
 *
 * @file
 * @brief       ICMPv6 type definitions
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef NANO_ICMPV6_H
#define NANO_ICMPV6_H

#ifdef __cplusplus
 extern "C" {
#endif

enum {
    NANO_ICMPV6_TYPE_DEST_UNREACHABLE       = 1,
    NANO_ICMPV6_TYPE_PKT_TOO_BIG            = 2,
    NANO_ICMPV6_TYPE_TIME_EXCEEDED          = 3,
    NANO_ICMPV6_TYPE_PARAM_PROBLEM          = 4,
    NANO_ICMPV6_TYPE_ECHO_REQ               = 128,
    NANO_ICMPV6_TYPE_ECHO_REPL              = 129,
    NANO_ICMPV6_TYPE_ROUTER_SOL             = 133,
    NANO_ICMPV6_TYPE_ROUTER_ADV             = 134,
    NANO_ICMPV6_TYPE_NEIGHBOR_SOL           = 135,
    NANO_ICMPV6_TYPE_NEIGHBOR_ADV           = 136,
    NANO_ICMPV6_TYPE_REDIRECT               = 137,
};

typedef struct __attribute__((packed)) {
    uint8_t type;
    uint8_t code;
    network_uint16_t checksum;
} nano_icmpv6_hdr_t;

typedef struct __attribute__((packed)) {
    network_uint16_t identifier;
    network_uint16_t seq_num;
} nano_icmpv6_echo_t;

#ifdef __cplusplus
}
#endif

#endif /* NANO_ICMPV6_H */
/** @} */
