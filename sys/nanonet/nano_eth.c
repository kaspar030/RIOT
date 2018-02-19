#include <string.h>

#include "byteorder.h"

#include "nanonet.h"

#ifdef NANONET_ETH

#define ENABLE_DEBUG ENABLE_NANONET_DEBUG
#include "debug.h"

void nano_eth_handle(nano_dev_t *dev, uint8_t *buf, size_t len)
{
    /* setup crosslayer context struct */
    nano_ctx_t ctx = { .dev=dev, .buf=buf, .len=len};

    ctx.l3_hdr_start = buf + sizeof(eth_hdr_t);

    eth_hdr_t *pkt = (eth_hdr_t *) buf;

    int (*handler)(nano_ctx_t *, size_t) = NULL;

    switch (ntohs(pkt->ethertype)) {
#ifdef NANONET_IPV4
        case 0x0800:
            handler = ipv4_handle;
            break;
        case 0x0806:
            handler = arp_handle;
            break;
#endif
#ifdef NANONET_IPV6
        case 0x86DD:
            handler = ipv6_handle;
            break;
#endif
        default:
            DEBUG("unknown ethertype 0x%04x\n", ntohs(pkt->ethertype));
    }

    int res = 0;
    if (handler) {
        res = handler(&ctx, sizeof(eth_hdr_t));
    }

    if (res > 0) {
        DEBUG("nanonet: replying with pkt of len %u\n", res);
        dev->send_raw(dev, buf, res);
    }
}

int nano_eth_reply(nano_ctx_t *ctx)
{
    eth_hdr_t *pkt = (eth_hdr_t *) ctx->buf;

    /* set new dst mac address to old src mac address */
    memcpy(pkt->dst, pkt->src, 6);

    /* set our address ass src address */
    memcpy(pkt->src, ctx->dev->l2_addr, 6);

    /* send the packet */
    ctx->dev->send_raw(ctx->dev, ctx->buf, ctx->len);

    return 0;
}

void nano_eth_get_iid(uint8_t *eui64, const uint8_t *mac)
{
    eui64[0] = mac[0] ^ 0x02;
    eui64[1] = mac[1];
    eui64[2] = mac[2];
    eui64[3] = 0xff;
    eui64[4] = 0xfe;
    eui64[5] = mac[3];
    eui64[6] = mac[4];
    eui64[7] = mac[5];
}

#endif /* NANONET_ETH */
