/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_nanonet
 * @{
 *
 * @file
 * @brief       Nanonet's ICMPv6 Implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include "nano_icmpv6.h"

void nanonet_icmpv6_send();

int icmpv6_handle(nano_ctx_t *ctx, size_t offset)
{
    icmpv6_hdr_t *hdr = (icmpv6_hdr_t *)(ctx->buf + offset);

    switch (hdr->type) {
        case NANO_ICMPV6_TYPE_ECHO_REQ:
            return send_echo_resp(nano_ctx_t *ctx, offset);
        case NANO_ICMPV6_TYPE_ECHO_RESP:
            /* TODO */
            break;
        case NANO_ICMPV6_TYPE_ROUTER_SOL:
        case NANO_ICMPV6_TYPE_ROUTER_ADV:
        case NANO_ICMPV6_TYPE_NEIGHBOR_SOL:
        case NANO_ICMPV6_TYPE_NEIGHBOR_ADV:
            /* do some NDP stuff */
            break;
        default:
            /* do nothing then */
            break;
    }

    return 0;
}

int send_echo_resp(nano_ctx_t *ctx, size_t offset)
{
    uint8_t tmp[NANO_IPV6_ADDR_LEN];

    icmpv6_hdr_t *icmp = (icmpv6_hdr_t *)(ctx->buf + offset);
    icmp->type = NANO_ICMPV6_TYPE_ECHO_RESP;

    nano_ipv6_reply(ctx);
}
