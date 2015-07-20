#include "byteorder.h"

#include "nanonet.h"

#define ENABLE_DEBUG ENABLE_NANONET_DEBUG
#include "debug.h"

void nano_eth_handle(nano_dev_t *dev, uint8_t *buf, size_t len)
{
    /* setup crosslayer context struct */
    nano_ctx_t ctx = { .dev=dev, .buf=buf, .len=len};

    eth_hdr_t *pkt = (eth_hdr_t *) buf;

    int (*handler)(nano_ctx_t *, size_t) = NULL;

    switch (NTOHS(pkt->ethertype)) {
        case 0x0800:
            handler = ipv4_handle;
            break;
        case 0x0806:
            handler = arp_handle;
            break;
        default:
            DEBUG("unknown ethertype 0x%04x\n", NTOHS(pkt->ethertype));
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
