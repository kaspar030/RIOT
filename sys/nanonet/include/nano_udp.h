#ifndef NANO_UDP_H
#define NANO_UDP_H

#include <stdint.h>
#include <stddef.h>

#include "nano_ctx.h"
#include "nano_eth.h"
#include "nano_ipv4.h"
#include "nano_ipv6.h"

typedef struct __attribute__((packed)) udp_hdr {
    uint16_t src_port;
    uint16_t dst_port;
    uint16_t length;
    uint16_t chksum;
} udp_hdr_t;

typedef int (*nano_udp_handler_t) (nano_ctx_t *ctx, size_t offset);

typedef struct nano_udp_bind {
    uint16_t port;
    nano_udp_handler_t handler;
} nano_udp_bind_t;

extern nano_udp_bind_t nano_udp_binds[];

int udp_handle(nano_ctx_t *ctx, size_t offset);
int udp_send(nano_sndbuf_t *buf, uint32_t dst_ip, uint16_t dst_port, uint16_t src_port);
int udp6_send(nano_sndbuf_t *buf, uint8_t* dst_ip, uint16_t dst_port, uint16_t src_port, nano_dev_t *dev);

static inline int udp_needed(uint32_t dest_ip) {
    /* TODO: actually get value from lower layers */
    (void)dest_ip;

    return sizeof(eth_hdr_t) + sizeof(ipv4_hdr_t) + sizeof(udp_hdr_t);
}

static inline int udp6_needed(uint8_t *dest_ip) {
    /* TODO: actually get value from lower layers */
    (void)dest_ip;

    return sizeof(eth_hdr_t) + sizeof(ipv6_hdr_t) + sizeof(udp_hdr_t);
}

#endif /* NANO_UDP_H */
