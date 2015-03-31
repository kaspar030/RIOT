#ifndef NANO_UDP_H
#define NANO_UDP_H

#include <stdint.h>

#include "nano_ctx.h"
#include "nano_eth.h"
#include "nano_ipv4.h"

typedef struct __attribute__((packed)) udp_hdr {
    uint16_t src_port;
    uint16_t dst_port;
    uint16_t length;
    uint16_t chksum;
} udp_hdr_t;

typedef int (*nano_udp_handler_t) (nano_ctx_t *ctx, char *buf, int len, int offset);

typedef struct nano_udp_bind {
    uint16_t port;
    nano_udp_handler_t handler;
} nano_udp_bind_t;

extern nano_udp_bind_t nano_udp_binds[];

int udp_handle(nano_ctx_t *ctx, char *buf, int len, int offset);
int udp_send(uint32_t dest_ip, uint16_t dst_port, uint16_t src_port, char *buf, int buflen, int used);

static inline int udp_needed(uint32_t dest_ip) {
    /* TODO: actually get value from lower layers */
    (void)dest_ip;

    return sizeof(eth_hdr_t) + sizeof(ipv4_hdr_t) + sizeof(udp_hdr_t);
}

#endif /* NANO_UDP_H */
