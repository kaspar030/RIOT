#include <errno.h>
#include <string.h>

#include "byteorder.h"

#include "nano_arp.h"
#include "nano_config.h"
#include "nano_icmp.h"
#include "nano_ipv4.h"
#include "nano_route.h"
#include "nano_sndbuf.h"
#include "nano_udp.h"
#include "nano_util.h"

#define ENABLE_DEBUG ENABLE_NANONET_DEBUG
#include "debug.h"

int ipv4_handle(nano_ctx_t *ctx, size_t offset) {
    ipv4_hdr_t *hdr = (ipv4_hdr_t*) (ctx->buf+offset);
    nano_dev_t *dev = ctx->dev;

    ctx->l3_hdr_start = (void*) hdr;

    if (ctx->len - offset < NTOHS(hdr->total_len)) {
        DEBUG("ipv4: truncated packet received.\n");
        return -1;
    }

    int hdr_len = ipv4_hdr_len(hdr);

    ctx->dst_addr.ipv4 = NTOHL(hdr->dst);
    ctx->src_addr.ipv4 = NTOHL(hdr->src);

    DEBUG("ipv4: got packet with protocol 0x%01x\n", (unsigned int) hdr->protocol);

    if (ctx->dst_addr.ipv4 == dev->ipv4 || ctx->dst_addr.ipv4 & 0xff) { /* TODO: fix broadcast stuff */
        switch (hdr->protocol) {
            case 0x1:
                icmp_handle(ctx, offset+hdr_len);
                break;
            case 0x11:
                udp_handle(ctx, offset+hdr_len);
                break;
            default:
                DEBUG("ipv4: unknown protocol.\n");
        }
    } else {
        // no routing implemented yet
    }
    return 0;
}

ipv4_route_t *ipv4_getroute(uint32_t dest_ip) {
    (void)dest_ip;
    ipv4_route_t *entry = &ipv4_routes[0];
    return entry;
#if 0
    while ( entry->dev ) {
    DEBUG("y\n");
        DEBUG("0x%08x 0x%08x 0x%08x\n", (unsigned int) entry->netmask, (unsigned int) dest_ip, (unsigned int) (entry->netmask & dest_ip));

        if (~(entry->netmask & dest_ip) == entry->netmask) {
            return entry;
        }

        entry++;
    }

    return NULL;
#endif
}

int ipv4_send(nano_sndbuf_t *buf, uint32_t dest_ip, int protocol) {
    ipv4_hdr_t *hdr;
    ipv4_route_t *route;
    nano_dev_t *dev;
    uint8_t dest_mac[6];

    if (! (route = ipv4_getroute(dest_ip))) {
        DEBUG("ipv4: no route to host 0x%08x\n", (unsigned int)dest_ip);
        return -1;
    }

    dev = route->dev;

    hdr = (ipv4_hdr_t *) nano_sndbuf_alloc(buf, sizeof(ipv4_hdr_t));
    if (!hdr) {
        DEBUG("ipv4: send buffer too small.\n");
        return -ENOSPC;
    }

    /* clear header */
    memset(hdr, '\0', sizeof(ipv4_hdr_t));

    /* set version to 4, IHL to 5 */
    hdr->ver_ihl = HTONS(0x4500);

    hdr->ttl = 64;
    hdr->protocol = protocol;
    hdr->src = HTONL(dev->ipv4);
    hdr->dst = HTONL(dest_ip);
    hdr->total_len = HTONS(buf->used);

    hdr->hdr_chksum = 0;

    hdr->hdr_chksum = ~nano_util_calcsum(0, (uint8_t*)hdr, sizeof(ipv4_hdr_t));

    if (! arp_cache_get(dev, dest_ip, dest_mac)) {
        DEBUG("ipv4: no ARP entry for 0x%08x\n", (unsigned int)dest_ip);
        return -2;
    }

    /* send packet */
    dev->send(dev, buf, dest_mac, 0x0800);

    return 0;
}
