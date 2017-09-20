#ifndef NANO_TCP_H
#define NANO_TCP_H

#include <stdint.h>
#include <stddef.h>

#include "nano_ctx.h"
#include "nano_eth.h"
#include "nano_ipv4.h"
#include "nano_ipv6.h"

typedef struct __attribute__((packed)) {
    uint16_t src_port;
    uint16_t dst_port;
    uint32_t seq_nr;
    uint32_t ack_nr;
    uint8_t data_offset; /* (NS not used) */
    uint8_t flags;
    uint16_t window_size;
    uint16_t urgent_ptr;
    uint8_t options[];
} tcp_hdr_t;

typedef int (*nano_tcp_handler_t) (nano_ctx_t *ctx, size_t offset);

static enum {
    LISTEN,
    SYN_SENT,
    SYN_RECEIVED,
    ESTABLISHED,
    FIN_WAIT_1,
    FIN_WAIT_2,
    CLOSE_WAIT,
    CLOSING,
    LAST_ACK,
    TIME_WAIT,
    CLOSED
};


typedef struct nano_tcp_bind {
    uint16_t port;
    nano_tcp_handler_t handler;
} nano_tcp_bind_t;

extern nano_tcp_bind_t nano_tcp_binds[];

int tcp_handle(nano_ctx_t *ctx, size_t offset);
int tcp_send(nano_sndbuf_t *buf, uint32_t dst_ip, uint16_t dst_port, uint16_t src_port);
int tcp6_send(nano_sndbuf_t *buf, uint8_t* dst_ip, uint16_t dst_port, uint16_t src_port, nano_dev_t *dev);
int tcp_reply(nano_ctx_t *ctx);

static inline int tcp_needed(uint32_t dest_ip) {
    /* TODO: actually get value from lower layers */
    (void)dest_ip;

    return sizeof(eth_hdr_t) + sizeof(ipv4_hdr_t) + sizeof(tcp_hdr_t);
}

static inline int tcp6_needed(uint8_t *dest_ip) {
    /* TODO: actually get value from lower layers */
    (void)dest_ip;

    return sizeof(eth_hdr_t) + sizeof(ipv6_hdr_t) + sizeof(tcp_hdr_t);
}

#endif /* NANO_TCP_H */
