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

#include <stdint.h>
#include <stddef.h>

#include "byteorder.h"
#include "nano_ctx.h"

#ifdef __cplusplus
 extern "C" {
#endif

#define NANO_ICMPV6_HDR_LEN                 (4U)

#define NANO_ICMPV6_NA_FLAG_ROUTER          (0x80)
#define NANO_ICMPV6_NA_FLAG_SOLICITED       (0x40)
#define NANO_ICMPV6_NA_FLAG_OVERRIDE        (0x20)

enum {
    NANO_ICMPV6_NDP_OPT_SRC_L2_ADDR         = 1,
    NANO_ICMPV6_NDP_OPT_DST_L2_ADDR         = 2,
    NANO_ICMPV6_NDP_OPT_PREFIX_INFO         = 3,
    NANO_ICMPV6_NDP_OPT_REDIRECT_HDR        = 4,
    NANO_ICMPV6_NDP_OPT_MTU                 = 5,
};

enum {
    NANO_ICMPV6_TYPE_DEST_UNREACHABLE       = 1,
    NANO_ICMPV6_TYPE_PKT_TOO_BIG            = 2,
    NANO_ICMPV6_TYPE_TIME_EXCEEDED          = 3,
    NANO_ICMPV6_TYPE_PARAM_PROBLEM          = 4,
    NANO_ICMPV6_TYPE_ECHO_REQ               = 128,
    NANO_ICMPV6_TYPE_ECHO_RESP              = 129,
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
} icmpv6_hdr_t;

typedef struct __attribute__((packed)) {
    network_uint16_t identifier;
    network_uint16_t seq_num;
} nano_icmpv6_echo_t;

typedef struct __attribute__((packed)) {
    uint32_t reserved;      /**< must be set to zero */
    uint8_t target_addr[IPV6_ADDR_LEN];
    uint8_t opt_type;
    uint8_t opt_length;
    uint8_t l2_addr[];
} nano_icmpv6_ns_t;

typedef struct __attribute__((packed)) {
    uint8_t flags;
    uint8_t reserved[3];      /**< must be set to zero */
    uint8_t target_addr[IPV6_ADDR_LEN];
    uint8_t opt_type;
    uint8_t opt_len;
    uint8_t l2_addr[];
} nano_icmpv6_na_t;

int icmpv6_handle(nano_ctx_t *ctx, size_t offset);

#ifdef __cplusplus
}
#endif

#endif /* NANO_ICMPV6_H */
/** @} */
