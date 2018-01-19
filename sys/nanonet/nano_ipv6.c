#include <errno.h>
#include <string.h>

#include "byteorder.h"

#include "nano_dev.h"
#include "nano_ipv6.h"
#include "nano_icmpv6.h"
#include "nano_route.h"
#include "nano_util.h"
#include "nano_udp.h"
#include "nano_ndp.h"
#include "nano_config.h"
#include "nano_6lp.h"
#include "nano_eth.h"
#include "nano_ieee802154.h"

#define ENABLE_DEBUG ENABLE_NANONET_DEBUG
#include "debug.h"

static uint16_t calcsum(ipv6_hdr_t *hdr);
static void set_csum(ipv6_hdr_t *hdr);

void ipv6_dispatch(nano_ctx_t *ctx, size_t l4offset, uint8_t next_header)
{
    if (ipv6_is_for_us(ctx)) { /* TODO: fix broadcast stuff */
        switch (next_header) {
            case IPV6_NEXTHDR_ICMP:
                DEBUG("ipv6: icmpv6.\n");
                icmpv6_handle(ctx, l4offset);
                break;
            case IPV6_NEXTHDR_UDP:
                DEBUG("ipv6: got UDP packet\n");
                udp_handle(ctx, l4offset);
                break;

            default:
                DEBUG("ipv6: unknown protocol.\n");
        }
    } else {
        DEBUG("ipv6: packet dropped, it's not for us.\n");
        // no routing implemented yet
    }
}

int ipv6_handle(nano_ctx_t *ctx, size_t offset) {
    ipv6_hdr_t *hdr = (ipv6_hdr_t*) (ctx->buf+offset);

    if (ctx->len - offset < sizeof(ipv6_hdr_t)) {
        DEBUG("ipv6: truncated packet received.\n");
        return -1;
    }

    size_t hdr_len = sizeof(ipv6_hdr_t);

    ctx->dst_addr.ipv6 = hdr->dst;
    ctx->src_addr.ipv6 = hdr->src;

    DEBUG("ipv6: got packet with protocol 0x%01x\n", (unsigned int) hdr->next_header);

    ipv6_dispatch(ctx, offset+hdr_len, hdr->next_header);

    return 0;
}

int ipv6_get_src_addr(uint8_t *tgt_buf, const nano_dev_t *dev, const uint8_t *tgt_addr)
{
    const uint8_t *src;
    if (ipv6_addr_is_link_local(tgt_addr)) {
        src = dev->ipv6_ll;
    }
    else {
        src = dev->ipv6_global;
    }

    memcpy(tgt_buf, src, IPV6_ADDR_LEN);

    return 0;
}

static int _ipv6_reply(nano_ctx_t *ctx)
{
    ipv6_hdr_t *hdr = (ipv6_hdr_t*) ctx->l3_hdr_start;

    hdr->payload_len = htons(ctx->len - (((uint8_t*)hdr) - ctx->buf) - sizeof(ipv6_hdr_t));

    set_csum(hdr);

    return ctx->dev->reply(ctx);
}

#ifdef NANONET_6LP
static int is_6lowpan(nano_ctx_t *ctx)
{
    /* TODO: hacky*/
    return (ctx->dev->handle_rx == nano_ieee802154_handle);
}
#else
static int is_6lowpan(nano_ctx_t *ctx)
{
    (void)ctx;
    return 0;
}
#endif

int ipv6_reply(nano_ctx_t *ctx)
{
    memcpy(ctx->dst_addr.ipv6, ctx->src_addr.ipv6, IPV6_ADDR_LEN);
    ipv6_get_src_addr(ctx->src_addr.ipv6, ctx->dev, ctx->dst_addr.ipv6);

    if (is_6lowpan(ctx)) {
#ifdef NANONET_6LP
        return nano_6lp_reply(ctx);
#else
        return 0;
#endif
    }
    else {
        return _ipv6_reply(ctx);
    }
}

static uint16_t calcsum(ipv6_hdr_t *hdr)
{
    uint16_t csum = 0;

    /* calculate checksum for IPv6 pseudo header */
    if (((uint32_t)(hdr->payload_len) + hdr->next_header) > 0xffff) {
        csum = 1;
    }
    csum = nano_util_calcsum(csum + hdr->payload_len + (hdr->next_header << 8),
                             hdr->src, (2 * IPV6_ADDR_LEN));
    /* add actual data fields */
    csum = nano_util_calcsum(csum, (const uint8_t *)(hdr + 1),
                             (size_t)ntohs(hdr->payload_len));
    return csum;
}

static void set_csum(ipv6_hdr_t *hdr)
{
    uint16_t csum = ~calcsum(hdr);

    /* calculate the checksum depending on the next header type */
    switch (hdr->next_header) {
        case IPV6_NEXTHDR_ICMP: {
            icmpv6_hdr_t *icmp;
            icmp = (icmpv6_hdr_t *)(hdr + 1);
            memcpy(&(icmp->checksum), &csum, 2);
            break;
        }
        case IPV6_NEXTHDR_UDP: {
            udp_hdr_t *udp;
            udp = (udp_hdr_t *)(hdr + 1);
            memcpy(&(udp->chksum), &csum, 2);
            break;
        }
        default:
            DEBUG("ipv6: don't know how to calculate checksum\n");
    }
}

static ipv6_route_t *ipv6_getroute(uint8_t *dest_ip) {
    (void)dest_ip;
/*    ipv6_route_t *entry = &ipv6_routes[0];
    return entry;
    */
    return NULL;
}

int ipv6_send(const iolist_t *iolist, uint8_t *dst_ip, int protocol, nano_dev_t *dev)
{
    uint8_t *l2_addr;
    size_t l2_addr_len;
    uint8_t *next_hop = NULL;

    if (dev && !ipv6_addr_is_link_local(dst_ip)) {
        DEBUG("ipv6_send(): device given, but address is not link-local.\n");
        return -EINVAL;
    }

    if (!ipv6_addr_is_link_local(dst_ip)) {
        ipv6_route_t *route = NULL;
        if (! (route = ipv6_getroute(dst_ip))) {
            DEBUG("ipv6: no route to host\n");
            return -1;
        }
        next_hop = route->next_hop;
        dev = route->dev;
    }

    l2_addr_len = nano_ndp_lookup(dev, next_hop ? next_hop : dst_ip, &l2_addr);
    if (!l2_addr_len) {
        return -EAGAIN;
    }

    if (0) {}
#ifdef NANONET_IEEE802154
    else if (dev->handle_rx == nano_ieee802154_handle) {
        DEBUG("ipv6_send(): 6lp send not implemented.\n");
        (void)iolist;
        (void)protocol;
        return -ENOSPC;
    }
#endif
#ifdef NANONET_ETH
    else if (dev->handle_rx == nano_eth_handle) {
        ipv6_hdr_t hdr = {
            /* set version to 6, traffic class + flow label to 0 */
            .ver_tc_fl = htonl(0x60000000U),
            .hop_limit = 64,
            .next_header = protocol
        };

        ipv6_get_src_addr(hdr.src, dev, dst_ip);
        memcpy(hdr.dst, dst_ip, IPV6_ADDR_LEN);

        /* send packet */
        return dev->send(dev, iolist, l2_addr, 0x86DD);
    }
#endif
    else {
        return -ENOTSUP;
    }
}

void ipv6_addr_print(const uint8_t *addr)
{
    const uint16_t* _addr = (const uint16_t*) addr;
    for (int i = 0; i < 8; i++) {
        printf("%04x", (unsigned)ntohs(_addr[i]));
        if (i != 7) {
            printf(":");
        }
    }
}
