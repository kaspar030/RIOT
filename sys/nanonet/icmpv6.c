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

#include <stdint.h>

#include "nano_config.h"
#include "nano_ctx.h"
#include "nano_ipv6.h"
#include "nano_icmpv6.h"

#define ENABLE_DEBUG ENABLE_NANONET_DEBUG
#include "debug.h"

int send_echo_resp(nano_ctx_t *ctx, size_t offset);


int icmpv6_handle(nano_ctx_t *ctx, size_t offset)
{
    icmpv6_hdr_t *hdr = (icmpv6_hdr_t *)(ctx->buf + offset);

    switch (hdr->type) {
        case NANO_ICMPV6_TYPE_ECHO_REQ:
            DEBUG("nanonet: icmpv6_handle(): got icmpv6 echo request\n");
            return send_echo_resp(ctx, offset);
            /*
        case NANO_ICMPV6_TYPE_ECHO_RESP:
        case NANO_ICMPV6_TYPE_ROUTER_SOL:
        case NANO_ICMPV6_TYPE_ROUTER_ADV:
        */
/*        case NANO_ICMPV6_TYPE_NEIGHBOR_SOL:
*/
            /*
        case NANO_ICMPV6_TYPE_NEIGHBOR_ADV:
        */
        default:
            DEBUG("nanonet: icmpv6_handle(): unhandled icmpv6 type=%u\n", (unsigned)hdr->type);
            break;
    }

    return 0;
}

int send_echo_resp(nano_ctx_t *ctx, size_t offset)
{
    icmpv6_hdr_t *icmp = (icmpv6_hdr_t *)(ctx->buf + offset);
    icmp->type = NANO_ICMPV6_TYPE_ECHO_RESP;

    return ipv6_reply(ctx);
}
