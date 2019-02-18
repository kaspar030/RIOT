#include <string.h>

#include "nano_config.h"

#ifdef NANONET_IPV4

#include "nano_dev.h"
#include "nano_arp.h"
#include "nano_eth.h"

#include "byteorder.h"

#define ENABLE_DEBUG ENABLE_NANONET_DEBUG
#include "debug.h"

static arp_cache_entry_t arp_cache[NANO_ARP_CACHE_SIZE] = { { 0 } };

static void arp_cache_put(nano_dev_t *dev, uint32_t dest_ip, uint8_t *mac_addr, unsigned n);
void arp_cache_update(nano_dev_t *dev, uint32_t dest_ip, uint8_t *mac);
static void arp_cache_clear(int n);
static void arp_request(nano_dev_t *dev, uint32_t ip);
static void arp_reply(nano_ctx_t *ctx, size_t offset);

int arp_handle(nano_ctx_t *ctx, size_t offset)
{
    arp_pkt_t *pkt;

    if ((ctx->len - offset) < (int)sizeof(arp_pkt_t)) {
        DEBUG("arp_handle(): pkt too short\n");
        return -1;
    }

    nano_dev_t *dev = ctx->dev;

    pkt = (arp_pkt_t *)(ctx->buf + offset);

    if (ntohl(pkt->arp_ipv4_types) != 0x00010800 || ntohs(pkt->arp_ipv4_lengths) != 0x0604) {
        DEBUG("arp_handle(): invalid types / lengths fields: types: 0x%x lengths: 0x%x\n",
              (unsigned int)ntohl(pkt->arp_ipv4_types),
              (unsigned int)ntohs(pkt->arp_ipv4_lengths));
        return -1;
    }

    uint32_t dst_ip = ntohl(pkt->dst_ip);
    uint32_t src_ip = ntohl(pkt->src_ip);
    int op = ntohs(pkt->arp_ipv4_op);
    switch (op) {
        case 1:
            DEBUG("arp: received request for 0x%08x\n", (unsigned int)dst_ip);
            if (dst_ip == dev->ipv4) {
                arp_cache_update(dev, src_ip, pkt->src_mac);
                arp_reply(ctx, offset);
            }
            break;
        case 2:
            DEBUG("arp: got reply for 0x%08x\n", (unsigned int)src_ip);
            arp_cache_update(dev, src_ip, pkt->src_mac);
            break;
        default:
            DEBUG("arp_handle(): invalid operation\n");
            return -1;
    }

    return 0;
}

static void arp_request(nano_dev_t *dev, uint32_t ip)
{
    DEBUG("arp_request: requesting MAC for 0x%08x\n", (unsigned int)ip);

    uint8_t buf[sizeof(eth_hdr_t) + sizeof(arp_pkt_t)] = { 0 };

    arp_pkt_t *pkt = (arp_pkt_t *)(buf + sizeof(eth_hdr_t));

    pkt->arp_ipv4_types = htonl(0x00010800);
    pkt->arp_ipv4_lengths = htons(0x0604);
    pkt->arp_ipv4_op = htons(0x0001);

    pkt->src_ip = htonl(dev->ipv4);
    memcpy(pkt->src_mac, dev->l2_addr, 6);

    pkt->dst_ip = htonl(ip);

    uint8_t broadcast[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

    iolist_t iolist = { NULL, buf, sizeof(buf) };
    dev->send(dev, &iolist, broadcast, 0x0806);
}

static void arp_reply(nano_ctx_t *ctx, size_t offset)
{
    nano_dev_t *dev = ctx->dev;

    /* we reuse the request packet, so most of the header is
     * already set up */
    arp_pkt_t *pkt = (arp_pkt_t *)(ctx->buf + offset);

    /* check if requested IP matches the device it came from */
    if (pkt->dst_ip != htonl(dev->ipv4)) {
        DEBUG("arp_reply: dst addr of request invalid.\n");
    }

    DEBUG("arp_reply: replying to 0x%08x\n", (unsigned int)ntohl(pkt->src_ip));

    /* set reply op */
    pkt->arp_ipv4_op = htons(0x0002);

    /* set new dst mac address to old src mac address */
    memcpy(pkt->dst_mac, pkt->src_mac, 6);

    /* copy our mac from dev struct to arp hdr */
    memcpy(pkt->src_mac, dev->l2_addr, 6);

    /* set IP addresses */
    pkt->dst_ip = pkt->src_ip;
    pkt->src_ip = htonl(dev->ipv4);

    dev->reply(ctx);
}

static int arp_cache_find(uint32_t dest_ip)
{
    for (int i = 0; i < NANO_ARP_CACHE_SIZE; i++) {
        if (arp_cache[i].ip == dest_ip) {
            return i;
        }
    }
    return -1;
}

static void arp_cache_clear(int n)
{
    assert((n >= 0) && n < NANO_ARP_CACHE_SIZE);
    memset(&arp_cache[n], 0, sizeof(arp_cache_entry_t));
}

static void arp_cache_put(nano_dev_t *dev, uint32_t dest_ip, uint8_t *mac_addr, unsigned n)
{
    assert(n < NANO_ARP_CACHE_SIZE);

#if 0 /* disabled for lack of data that shows any improvement */
    if (n != 0) {
        /* if the first entry is older than the *last* time
         * the updated entry was accessed, swap them.
         * This way, old entries should eventually move to the bottom of the
         * list.
         */
        if (arp_cache[0].since < arp_cache[n].since) {
            arp_cache[n] = arp_cache[0];
            n = 0;
        }
    }
#endif

    DEBUG("arp cache adding/updating 0x%08x to entry %i\n", (unsigned int)dest_ip, n);

    arp_cache[n].ip = dest_ip;
    arp_cache[n].dev = dev;
    memcpy(arp_cache[n].mac, mac_addr, 6);
    arp_cache[n].since = 0 /* TODO: use proper time */;
}

int arp_cache_get(nano_dev_t *dev, uint32_t dest_ip, uint8_t *mac_addr_out)
{
    int res = 0;

    int n = arp_cache_find(dest_ip);
    if (n != -1) {
        memcpy(mac_addr_out, arp_cache[n].mac, 6);
        res = 1;
        DEBUG("arp: found 0x%08x as entry %i\n", (unsigned int)dest_ip, n);
    }

    if (!res) {
        /* IP not found in entry. start ARP request. */
        arp_request(dev, dest_ip);
    }

    return res;
}

void arp_cache_update(nano_dev_t *dev, uint32_t dest_ip, uint8_t *mac)
{
    int n;
    unsigned now = 0;
    uint16_t max_age = 0;
    int oldest = -1;
    int free = -1;
    int match = -1;

    for (unsigned i = 0; i < NANO_ARP_CACHE_SIZE; i++) {
        if (arp_cache[i].ip == dest_ip) {
            /* check for exact match of the IP address */
            match = i;
            break;
        }
        else if (free == -1) {
            /* if we didn't find a free slot yet, check if this one is */
            if (arp_cache[i].ip == 0) {
                free = i;
            }
            else {
                /* check if this is the oldest entry. */
                uint16_t age = now - arp_cache[i].since;
                if (age >= max_age) {
                    max_age = age;
                    oldest = i;
                }
            }
        }
    }

    /* use exact match, first free slot or oldest entry in that order */
    if ((n = match) < 0) {
        if ((n = free) < 0) {
            n = oldest;
            arp_cache_clear(n);
            DEBUG("arp: using evicted entry %i\n", n);
        }
        else {
            DEBUG("arp: using free entry %i\n", n);
        }
    }
    else {
        if (memcmp(arp_cache[n].mac, mac, 6) == 0)  {
            /* there was a match for the IP address.
             * If the mac address is still the same, just update
             * the last used time and return.
             */
            DEBUG("arp: updating entry %i time\n", n);
            arp_cache[n].since = 0 /* TODO: use proper time */;
            return;
        }
        else {
            DEBUG("arp: changing MAC address for entry %i\n", n);
        }
    }

    assert (n >= 0);

    arp_cache_put(dev, dest_ip, mac, n);
}

#endif /* NANONET_IPV4 */
