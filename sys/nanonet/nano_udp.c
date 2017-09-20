#include <errno.h>

#include "nano_udp.h"
#include "nano_ctx.h"
#include "nano_icmp.h"
#include "nano_ipv4.h"
#include "nano_ipv6.h"
#include "nano_sndbuf.h"

#include "byteorder.h"

#define ENABLE_DEBUG ENABLE_NANONET_DEBUG
#include "debug.h"

int udp_handle(nano_ctx_t *ctx, size_t offset)
{
    udp_hdr_t *hdr = (udp_hdr_t*) (ctx->buf+offset);

    if ((ctx->len-offset) < (int)sizeof(udp_hdr_t)) {
        DEBUG("udp: truncated packet received.\n");
        return -1;
    }

    ctx->src_port = ntohs(hdr->src_port);

    uint16_t dst_port = ntohs(hdr->dst_port);
    ctx->dst_port = dst_port;

#if ENABLE_DEBUG
    if (is_ipv4_hdr(ctx->l3_hdr_start)) {
        DEBUG("udp: received packet from 0x%08x, src_port %u, dst_port %u\n",
                (unsigned int) ctx->src_addr.ipv4, ctx->src_port, ctx->dst_port);
    } else {
        DEBUG("udp: received UDPv6 packet src_port %u, dst_port %u\n",
                ctx->src_port, ctx->dst_port);
    }
#endif

    nano_udp_handler_t handler = NULL;
    nano_udp_bind_t *bind = &nano_udp_binds[0];
    while (bind) {
        if (bind->port == dst_port) {
            handler = bind->handler;
            break;
        }
        else if (bind->port > dst_port) {
            break;
        }
        bind++;
    }

    if (handler) {
        return handler(ctx, offset+sizeof(udp_hdr_t));
    }
    else {
        if (is_ipv4_hdr(ctx->l3_hdr_start)) {
            if (! (ctx->src_addr.ipv4 || (~ctx->dst_addr.ipv4))) {
                /* also filter broadcast */
                DEBUG("udp: unreachable port %u\n", dst_port);
                icmp_port_unreachable(ctx);
            }
        }
        else {
            DEBUG("udp: UDPv6: not filtering broadcast icmp reply\n");
            //icmpv6_reply_dst_unreachable(ctx, ICMPV6_DST_UNREACH_PORT);
        }
    }

    return 0;
}

static size_t udp_build_hdr(nano_sndbuf_t *buf, uint16_t dst_port, uint16_t src_port) {
    udp_hdr_t *hdr = (udp_hdr_t *) nano_sndbuf_alloc(buf, sizeof(udp_hdr_t));

    if (!hdr) {
        DEBUG("udp_build_hdr: buffer too small.\n");
        return 0;
    }

    hdr->src_port = htons(src_port);
    hdr->dst_port = htons(dst_port);

    hdr->length = htons(nano_sndbuf_used(buf));

    /* checksum will be calculated by IP layer */
    hdr->chksum = 0x0;

    return sizeof(udp_hdr_t);
}

int udp_reply(nano_ctx_t *ctx)
{
    udp_hdr_t *hdr;
    if (is_ipv4_hdr(ctx->l3_hdr_start)) {
        hdr = (udp_hdr_t *) (ctx->l3_hdr_start + sizeof(ipv4_hdr_t));
    }
    else {
        hdr = (udp_hdr_t *) (ctx->l3_hdr_start + sizeof(ipv6_hdr_t));
    }

    /* swap ports */
    uint16_t tmp = hdr->dst_port;
    hdr->dst_port = hdr->src_port;
    hdr->src_port = tmp;
    hdr->length = htons(ctx->len - ((uint8_t*)hdr - ctx->buf));
    hdr->chksum = 0x0;

    if (is_ipv4_hdr(ctx->l3_hdr_start)) {
        return ipv4_reply(ctx);
    }
    else {
        return ipv6_reply(ctx);
    }
}

#ifdef NANONET_IPV4
int udp_send(nano_sndbuf_t *buf, uint32_t dst_ip, uint16_t dst_port, uint16_t src_port)
{
    DEBUG("udp: sending packet to 0x%08x\n", (unsigned int) dst_ip);

    if (!udp_build_hdr(buf, dst_port, src_port)) {
        return -ENOSPC;
    }

    return ipv4_send(buf, dst_ip, 0x11);
}
#endif

int udp6_send(nano_sndbuf_t *buf, uint8_t* dst_ip, uint16_t dst_port, uint16_t src_port, nano_dev_t *dev)
{
    DEBUG("udp: sending udpv6 packet packet\n");

    if (!udp_build_hdr(buf, dst_port, src_port)) {
        return -ENOSPC;
    }

    return ipv6_send(buf, dst_ip, IPV6_NEXTHDR_UDP, dev);
}
