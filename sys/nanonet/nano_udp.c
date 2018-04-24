#include <errno.h>

#include "byteorder.h"
#include "iolist.h"

#include "nano_udp.h"
#include "nano_ctx.h"
#include "nano_icmp.h"
#include "nano_ipv4.h"
#include "nano_ipv6.h"

#define ENABLE_DEBUG ENABLE_NANONET_DEBUG
#include "debug.h"

clist_node_t nano_udp_binds;

/*static int _cmp_bind(clist_node_t *a, clist_node_t *b)
{
    nano_udp_bind_t *_a = (void *)a;
    nano_udp_bind_t *_b = (void *)b;
    return (int)(_a->port - _b->port);
}
*/

static int _find_udp_bind_helper(clist_node_t *bind, void *arg)
{
    uint16_t port = *(uint16_t *)arg;
    nano_udp_bind_t *_bind = (void *)bind;
    return port == _bind->port;
}

static nano_udp_bind_t *_find_udp_bind(uint16_t port)
{
    return (void *)clist_foreach(&nano_udp_binds, _find_udp_bind_helper, &port);
}

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

    nano_udp_bind_t *bind = _find_udp_bind(dst_port);
    if (bind) {
        return bind->handler(ctx, offset+sizeof(udp_hdr_t), bind);
    }
    else {
        if (is_ipv4_hdr(ctx->l3_hdr_start)) {
#ifdef NANONET_IPV4
            if (! (ctx->src_addr.ipv4 || (~ctx->dst_addr.ipv4))) {
                /* also filter broadcast */
                DEBUG("udp: unreachable port %u\n", dst_port);
                icmp_port_unreachable(ctx);
            }
#endif
        }
        else {
            DEBUG("udp: UDPv6: not filtering broadcast icmp reply\n");
            //icmpv6_reply_dst_unreachable(ctx, ICMPV6_DST_UNREACH_PORT);
        }
    }

    return 0;
}

static void udp_build_hdr(udp_hdr_t *hdr, uint16_t dst_port, uint16_t src_port, size_t len)
{

    hdr->src_port = htons(src_port);
    hdr->dst_port = htons(dst_port);

    hdr->length = htons(len);

    /* checksum will be calculated by IP layer */
    hdr->chksum = 0x0;
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

    switch (is_ipv4_hdr(ctx->l3_hdr_start)) {
#ifdef NANONET_IPV4
        case 1:
            return ipv4_reply(ctx);
#endif
#ifdef NANONET_IPV6
        case 0:
            return ipv6_reply(ctx);
#endif
        default:
            return 0;
    }
}

#ifdef NANONET_IPV4
int udp_send(const iolist_t *iolist, uint32_t dst_ip, uint16_t dst_port, uint16_t src_port)
{
    DEBUG("udp: sending packet to 0x%08x\n", (unsigned int)dst_ip);

    udp_hdr_t hdr;
    udp_build_hdr(&hdr, dst_port, src_port, iolist_size(iolist) + sizeof(hdr));
    iolist_t _iolist = { (iolist_t *)iolist, &hdr, sizeof(hdr) };

    return ipv4_send(&_iolist, dst_ip, 0x11);
}
#endif

#ifdef NANONET_IPV6
int udp6_send(const iolist_t *iolist, uint8_t *dst_ip, uint16_t dst_port, uint16_t src_port, nano_dev_t *dev)
{
    DEBUG("udp: sending udpv6 packet packet\n");

    udp_hdr_t hdr;
    udp_build_hdr(&hdr, dst_port, src_port, iolist_size(iolist) + sizeof(hdr));
    iolist_t _iolist = { (iolist_t *)iolist, &hdr, sizeof(hdr) };

    return ipv6_send(&_iolist, dst_ip, IPV6_NEXTHDR_UDP, dev);
}
#endif
