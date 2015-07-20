#ifndef NANO_ARP_H
#define NANO_ARP_H

#include <stdint.h>
#include "nano_ctx.h"
#include "nano_dev.h"

#define NANO_ARP_CACHE_SIZE 16

typedef struct  __attribute__((packed)) arp_pkt {
    uint32_t arp_ipv4_types;
    uint16_t arp_ipv4_lengths;
    uint16_t arp_ipv4_op;
    uint8_t src_mac[6];
    uint32_t src_ip;
    uint8_t dst_mac[6];
    uint32_t dst_ip;
} arp_pkt_t;

typedef struct arp_cache_entry {
    uint32_t ip;
    uint8_t mac[6];
    nano_dev_t* dev;
} arp_cache_entry_t;

int arp_handle(nano_ctx_t *ctx, size_t offset);
void arp_reply(nano_ctx_t *ctx, size_t offset);

int arp_cache_get(nano_dev_t *dev, uint32_t dest_ip, uint8_t *mac_addr_out);
void arp_request(nano_dev_t *dev, uint32_t dest_ip);

#endif /* NANO_ARP_H */
