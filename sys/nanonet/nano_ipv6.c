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

#include "net/ng_ipv6/addr.h"

#define ENABLE_DEBUG ENABLE_NANONET_DEBUG
#include "debug.h"

static uint16_t calcsum(ipv6_hdr_t *hdr);
static void set_csum(ipv6_hdr_t *hdr);

static inline int ipv6_is_for_us(nano_ctx_t *ctx)
{
    return ipv6_addr_equal(ctx->dst_addr.ipv6, ctx->dev->ipv6_ll)
        || ipv6_addr_equal(ctx->dst_addr.ipv6, ctx->dev->ipv6_global)
        || (ctx->dst_addr.ipv6[0] == 0xFF);
}

static inline int ipv6_addr_is_link_local(const uint8_t *addr)
{
    (void)addr;
    printf("%s:%ul:%s(): forcing link-local.\n", RIOT_FILE_RELATIVE, __LINE__, __func__);
    return 1;
}

int ipv6_handle(nano_ctx_t *ctx, size_t offset) {
    ipv6_hdr_t *hdr = (ipv6_hdr_t*) (ctx->buf+offset);

    ctx->l3_hdr_start = (void*) hdr;

    if (ctx->len - offset < sizeof(ipv6_hdr_t)) {
        DEBUG("ipv6: truncated packet received.\n");
        return -1;
    }

    size_t hdr_len = sizeof(ipv6_hdr_t);

    ctx->dst_addr.ipv6 = hdr->dst;
    ctx->src_addr.ipv6 = hdr->src;

    DEBUG("ipv6: got packet with protocol 0x%01x\n", (unsigned int) hdr->next_header);

    if (ipv6_is_for_us(ctx)) { /* TODO: fix broadcast stuff */
        switch (hdr->next_header) {
            case IPV6_NEXTHDR_ICMP:
                DEBUG("ipv6: icmpv6.\n");
                icmpv6_handle(ctx, offset+hdr_len);
                break;
            case IPV6_NEXTHDR_UDP:
                udp_handle(ctx, offset+hdr_len);
                break;

            default:
                DEBUG("ipv6: unknown protocol.\n");
        }
    } else {
        DEBUG("ipv6: packet dropped, it's not for us.\n");
        // no routing implemented yet
    }
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

int ipv6_reply(nano_ctx_t *ctx)
{
    memcpy(ctx->dst_addr.ipv6, ctx->src_addr.ipv6, IPV6_ADDR_LEN);
    ipv6_get_src_addr(ctx->src_addr.ipv6, ctx->dev, ctx->dst_addr.ipv6);

    set_csum((ipv6_hdr_t *)ctx->l3_hdr_start);

    return ctx->dev->reply(ctx);
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
    csum = nano_util_calcsum(csum, (const uint8_t *)hdr+1,
                             (size_t)NTOHS(hdr->payload_len));
    return csum;
}

static void set_csum(ipv6_hdr_t *hdr)
{
    uint16_t csum = ~calcsum(hdr);

    /* calculate the checksum depending on the next header type */
    switch (hdr->next_header) {
        case IPV6_NEXTHDR_ICMP: {
            icmpv6_hdr_t *icmp;
            icmp = (icmpv6_hdr_t *)(hdr+1);
            memcpy(&(icmp->checksum), &csum, 2);
            break;
        }
        case IPV6_NEXTHDR_UDP: {
            udp_hdr_t *udp;
            udp = (udp_hdr_t *)(hdr+1);
            memcpy(&(udp->chksum), &csum, 2);
        }
        default:
            /* don't know how to calculate, so we return 0 */
            csum = 0;
            break;
    }
}

static ipv6_route_t *ipv6_getroute(uint8_t *dest_ip) {
    (void)dest_ip;
/*    ipv6_route_t *entry = &ipv6_routes[0];
    return entry;
    */
    return NULL;
}

int ipv6_send(nano_sndbuf_t *buf, uint8_t *dst_ip, int protocol, nano_dev_t *dev)
{
    ipv6_hdr_t *hdr;
    uint8_t *l2_addr;
    size_t l2_addr_len
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

    int l2_addr_len = nano_ndp_lookup(dev, next_hop ? next_hop : dst_ip, &dst_mac);
    if (!l2_addr_len) {
        return -EAGAIN;
    }

    /* allocate our header, check what l2 needs, bail out if not enough */
    hdr = (ipv6_hdr_t *) nano_sndbuf_alloc(buf, sizeof(ipv6_hdr_t));

    if (!hdr) {
        DEBUG("ipv6: send buffer too small.\n");
        return -ENOSPC;
    }

    /* clear header */
    memset(hdr, '\0', sizeof(ipv6_hdr_t));

    /* set version to 6, traffic class + flow label to 0 */
    hdr->ver_tc_fl = HTONL(0x60000000U);

    hdr->hop_limit = 64;
    hdr->next_header = protocol;

    ipv6_get_src_addr(hdr->src, dev, dst_ip);
    memcpy(hdr->dst, dst_ip, IPV6_ADDR_LEN);

    /* send packet */
    return dev->send(dev, buf, l2_addr, 0x86DD);
}

void ipv6_addr_print(const uint8_t *addr)
{
    for (int i = 0; i < 16; i++) {
        printf("%02x", (unsigned)addr[i]);
        if (i != 15) {
            printf(":");
        }
    }
}
