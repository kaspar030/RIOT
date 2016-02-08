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
#include "nano_util.h"
#include "nano_ndp.h"

#define ENABLE_DEBUG ENABLE_NANONET_DEBUG
#include "debug.h"

int send_echo_resp(nano_ctx_t *ctx, size_t offset);
int handle_neighbor_solicitation(nano_ctx_t *ctx, size_t offset);
void handle_neighbor_advertisement(nano_ctx_t *ctx, size_t offset);


int icmpv6_handle(nano_ctx_t *ctx, size_t offset)
{
    icmpv6_hdr_t *hdr = (icmpv6_hdr_t *)(ctx->buf + offset);

    switch (hdr->type) {
        case NANO_ICMPV6_TYPE_ECHO_REQ:
            DEBUG("nanonet: icmpv6_handle(): got ICMPv6 echo request\n");
            return send_echo_resp(ctx, offset);
        case NANO_ICMPV6_TYPE_NEIGHBOR_SOL:
            DEBUG("nanonet: icmpv6_handle(): got ICMPv6 neighbor sol\n");
            return handle_neighbor_solicitation(ctx, offset);
        case NANO_ICMPV6_TYPE_NEIGHBOR_ADV:
            DEBUG("nanonet: icmpv6_handle(): got ICMPv6 neighbor adv\n");
            handle_neighbor_advertisement(ctx, offset);
            break;
        /*
        case NANO_ICMPV6_TYPE_ECHO_RESP:
        case NANO_ICMPV6_TYPE_ROUTER_ADV:
        case NANO_ICMPV6_TYPE_ROUTER_SOL:
        */
        default:
            DEBUG("nanonet: icmpv6_handle(): unhandled icmpv6 type=%u\n", (unsigned)hdr->type);
            break;
    }

    return 0;
}

/*void icmpv6_reply_dst_unreachable(nano_ctx_t *ctx, uint8_t code)
{

}
*/
int send_echo_resp(nano_ctx_t *ctx, size_t offset)
{
    icmpv6_hdr_t *icmp = (icmpv6_hdr_t *)(ctx->buf + offset);
    /* set type to echo response and reset checksum */
    icmp->type = NANO_ICMPV6_TYPE_ECHO_RESP;
    icmp->checksum = byteorder_htons(0);
    /* and send the thing back (for good) */
    return ipv6_reply(ctx);
}

int handle_neighbor_solicitation(nano_ctx_t *ctx, size_t offset)
{
    icmpv6_hdr_t *icmp = (icmpv6_hdr_t *)(ctx->buf + offset);
    nano_icmpv6_ns_t *ns = (nano_icmpv6_ns_t *)(ctx->buf + offset +
                                                NANO_ICMPV6_HDR_LEN);

    /* TODO: move this into the IPv6 module and do it on every received packet */
    //nano_ndp_update(ctx);

    if (ipv6_addr_equal(ctx->dev->ipv6_ll, ns->target_addr)) {
        if ((ctx->buf_size - ctx->len) < 6) {
            DEBUG("nanonet: neighbor advertisement handle: no room for l2"
                  "address in buffer\n");
            return 0;
        }
        icmp->type = NANO_ICMPV6_TYPE_NEIGHBOR_ADV;
        icmp->checksum = byteorder_htons(0);
        ns->opt_type = NANO_ICMPV6_NDP_OPT_DST_L2_ADDR;
        memcpy(ns->l2_addr, ctx->dev->l2_addr, 6);

        if (!ipv6_addr_is_multicast(ctx->dst_addr.ipv6)) {
            nano_icmpv6_na_t *na = (nano_icmpv6_na_t*) ns;
            na->flags |= NANO_ICMPV6_NA_FLAG_SOLICITED;
        }

        /* TODO: handle neighbor advertisement flags */
        return ipv6_reply(ctx);
    }
    return 0;
}

void handle_neighbor_advertisement(nano_ctx_t *ctx, size_t offset)
{
    size_t l2_addr_len;
    // nano_icmpv6_na_t *na = (nano_icmpv6_na_t *)(ctx->buf + offset +
    //                                             NANO_ICMPV6_HDR_LEN);

    /* see if the neighbor advertisement contains a link layer address */
    l2_addr_len = (ctx->len - offset) + NANO_ICMPV6_HDR_LEN + 4 + IPV6_ADDR_LEN;
    if (l2_addr_len == 0) {
        DEBUG("nanonet: neighbor advertisement handle: no l2 address found\n");
        return;
    }
    /* TODO: handle flags! */
    //nano_ndp_cache_add(na->tartget_addr, na->l2_addr, l2_addr_len);
}
