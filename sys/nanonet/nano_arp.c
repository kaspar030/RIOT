#include <string.h>

#include "nano_dev.h"
#include "nano_arp.h"
#include "nano_eth.h"
#include "nano_config.h"

#include "byteorder.h"

#define ENABLE_DEBUG ENABLE_NANONET_DEBUG
#include "debug.h"

static arp_cache_entry_t arp_cache[NANO_ARP_CACHE_SIZE] = {{0}};

static void arp_cache_put(nano_dev_t *dev, uint32_t dest_ip, uint8_t *mac_addr);
static void arp_cache_maybe_add(nano_dev_t *dev, uint32_t dest_ip, uint8_t *mac);

int arp_handle(nano_ctx_t *ctx, size_t offset) {
    arp_pkt_t* pkt;

    if ((ctx->len - offset) < (int)sizeof(arp_pkt_t)) {
        DEBUG("arp_handle(): pkt too short\n");
        return -1;
    }

    nano_dev_t *dev = ctx->dev;

    pkt = (arp_pkt_t*) (ctx->buf+offset);

    if (NTOHL(pkt->arp_ipv4_types) != 0x00010800 || NTOHS(pkt->arp_ipv4_lengths) != 0x0604) {
        DEBUG("arp_handle(): invalid types / lengths fields: types: 0x%x lengths: 0x%x\n",
                (unsigned int) NTOHL(pkt->arp_ipv4_types),
                (unsigned int) NTOHS(pkt->arp_ipv4_lengths));
        return -1;
    }

    uint32_t dst_ip = NTOHL(pkt->dst_ip);
    uint32_t src_ip = NTOHL(pkt->src_ip);
    int op = NTOHS(pkt->arp_ipv4_op);
    switch (op) {
        case 1:
            DEBUG("arp: request for 0x%08x\n", (unsigned int) dst_ip);
            if (dst_ip == dev->ipv4) {
                arp_cache_maybe_add(dev, src_ip, pkt->src_mac);
                arp_reply(ctx, offset);
            }
            break;
        case 2:
            DEBUG("arp: reply for 0x%08x\n", (unsigned int) src_ip);
            arp_cache_maybe_add(dev, src_ip, pkt->src_mac);
            break;
        default:
            DEBUG("arp_handle(): invalid operation\n");
            return -1;
    }

    return 0;
}

void arp_request(nano_dev_t *dev, uint32_t ip) {
    DEBUG("arp_request: requesting MAC for 0x%08x\n", (unsigned int)ip);
    uint8_t buf[sizeof(eth_hdr_t)+sizeof(arp_pkt_t)];
    nano_sndbuf_t sndbuf = { .buf=buf, .size=sizeof(buf), .used=sizeof(arp_pkt_t)};

    memset(buf, '\0', sizeof(eth_hdr_t)+sizeof(arp_pkt_t));

    arp_pkt_t* pkt = (arp_pkt_t*) (buf+sizeof(eth_hdr_t));
    pkt->arp_ipv4_types = HTONL(0x00010800);
    pkt->arp_ipv4_lengths = HTONS(0x0604);
    pkt->arp_ipv4_op = HTONS(0x0001);

    pkt->src_ip = HTONL(dev->ipv4);
    memcpy(pkt->src_mac, dev->mac_addr, 6);

    pkt->dst_ip = HTONL(ip);

    uint8_t broadcast[] = { 0xff,0xff,0xff,0xff,0xff,0xff };

    dev->send(dev, &sndbuf, broadcast, 0x0806);
}

void arp_reply(nano_ctx_t *ctx, size_t offset)
{
    nano_dev_t *dev = ctx->dev;

    /* we reuse the request packet, so most of the header is
     * already set up */
    arp_pkt_t* pkt = (arp_pkt_t*) (ctx->buf+offset);

    /* check if requested IP matches the device it came from */
    if (pkt->dst_ip != HTONL(dev->ipv4)) {
        DEBUG("arp_reply: dst addr of request invalid.\n");
    }

    DEBUG("arp_reply: replying to 0x%08x\n", (unsigned int)NTOHL(pkt->src_ip));

    /* set reply op */
    pkt->arp_ipv4_op = HTONS(0x0002);

    /* set new dst mac address to old src mac address */
    memcpy(pkt->dst_mac, pkt->src_mac, 6);

    /* copy our mac from dev struct to arp hdr */
    memcpy(pkt->src_mac, dev->mac_addr, 6);

    /* set IP addresses */
    pkt->dst_ip = pkt->src_ip;
    pkt->src_ip = HTONL(dev->ipv4);

    dev->reply(ctx);
}

int arp_cache_find(uint32_t dest_ip) {
    for (int i = 0; i < NANO_ARP_CACHE_SIZE; i++) {
        if (arp_cache[i].ip == dest_ip) {
            return i;
        }
    }
    return -1;
}

int arp_cache_get(nano_dev_t *dev, uint32_t dest_ip, uint8_t *mac_addr_out)
{
    DEBUG("arp_cache_get: looking for 0x%08x\n", (unsigned int)dest_ip);

    int res = 0;

    int n = arp_cache_find(dest_ip);
    if (n != -1) {
        memcpy(mac_addr_out, arp_cache[n].mac, 6);
        res = 1;
    }

    if(!res) {
        /* IP not found in entry. start ARP request. */
        arp_request(dev, dest_ip);
    }

    return res;
}

static void arp_cache_put(nano_dev_t *dev, uint32_t dest_ip, uint8_t *mac_addr)
{
    int n = arp_cache_find(0x0);

    if (n != -1) {
        DEBUG("arp cache adding 0x%08x\n", (unsigned int)dest_ip);
        arp_cache[n].ip = dest_ip;
        arp_cache[n].dev = dev;
        memcpy(arp_cache[n].mac, mac_addr, 6);
    } else {
        DEBUG("arp cache full\n");
    }
}

static void arp_cache_maybe_add(nano_dev_t *dev, uint32_t dest_ip, uint8_t *mac)
{
    if (arp_cache_find(dest_ip) == -1) {
        arp_cache_put(dev, dest_ip, mac);
    }
}
