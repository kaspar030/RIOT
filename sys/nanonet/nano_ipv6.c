#include <string.h>

#include "byteorder.h"

#include "nano_ipv6.h"
#include "nano_icmpv6.h"
#include "nano_route.h"
#include "nano_util.h"
//#include "nano_udp.h"
#include "nano_config.h"

#include "net/ng_ipv6/addr.h"

#define ENABLE_DEBUG ENABLE_NANONET_DEBUG
#include "debug.h"

static inline int ipv6_is_for_us(nano_ctx_t *ctx)
{
    return ipv6_addr_equal(ctx->dst_addr.ipv6, ctx->dev->ipv6_ll)
        || ipv6_addr_equal(ctx->dst_addr.ipv6, ctx->dev->ipv6_global)
        || (ctx->dst_addr.ipv6[0] == 0xFF);
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
            case 0x3A:
                DEBUG("ipv6: icmpv6.\n");
                icmpv6_handle(ctx, offset+hdr_len);
                break;
/*            case 0x11:
                udp_handle(ctx, buf, len, offset+hdr_len);
                break;
*/
            default:
                DEBUG("ipv6: unknown protocol.\n");
        }
    } else {
        DEBUG("ipv6: packet dropped, it's not for us.\n");
        // no routing implemented yet
    }
    return 0;
}

/*int ipv6_get_src_addr_for(uint8_t *tgt_buf, const uint8_t *tgt_addr)
{
}
*/
int ipv6_reply(nano_ctx_t *ctx)
{
    memcpy(ctx->dst_addr.ipv6, ctx->src_addr.ipv6, 16);
 //   ipv6_get_src_addr_for(ctx->src_addr.ipv6, ctx->dst_addr.ipv6);
    memcpy(ctx->src_addr.ipv6, ctx->dev->ipv6_ll, 16);

    return ctx->dev->reply(ctx);
}

#if 0
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

int ipv4_send(uint32_t dest_ip, int protocol, char *buf, int len, int used) {
    ipv4_hdr_t *hdr;
    ipv4_route_t *route;
    nano_dev_t *dev;
    uint8_t dest_mac[6];

    if (! (route = ipv4_getroute(dest_ip))) {
        DEBUG("ipv4: no route to host 0x%08x\n", (unsigned int)dest_ip);
        return -1;
    }

    dev = route->dev;

    /* allocate our header, check what l2 needs, bail out if not enough */
    hdr = (ipv4_hdr_t *)(buf + len - used - sizeof(ipv4_hdr_t));
    if ((char*)hdr < (buf+dev->l2_needed(dev))) {
        DEBUG("ipv4: send buffer too small.\n");
        return -1;
    }

    /* clear header */
    memset(hdr, '\0', sizeof(ipv4_hdr_t));

    /* set version to 4, IHL to 5 */
    hdr->ver_ihl = HTONS(0x4500);

    hdr->ttl = 64;
    hdr->protocol = protocol;
    hdr->src = HTONL(dev->ipv4);
    hdr->dst = HTONL(dest_ip);
    hdr->total_len = HTONS(sizeof(ipv4_hdr_t) + used);

    hdr->hdr_chksum = 0;

    hdr->hdr_chksum = nano_calcsum((uint16_t*)hdr, sizeof(ipv4_hdr_t));

    if (! arp_cache_get(dev, dest_ip, dest_mac)) {
        DEBUG("ipv4: no ARP entry for 0x%08x\n", (unsigned int)dest_ip);
        return -2;
    }

    /* send packet */
    dev->send(dev, dest_mac, 0x0800, buf, len, sizeof(ipv4_hdr_t) + used);

    return 0;
}
#endif

void ipv6_addr_print(const uint8_t *addr)
{
    for (int i = 0; i < 16; i++) {
        printf("%02x", (unsigned)addr[i]);
        if (i != 15) {
            printf(":");
        }
    }
}
