#ifndef NANO_UDP_H
#define NANO_UDP_H

#include <stdint.h>
#include <stddef.h>

#include "clist.h"

#include "net/nano/ctx.h"
#include "net/nano/ipv4.h"
#include "net/nano/ipv6.h"

typedef struct __attribute__((packed)) udp_hdr {
    uint16_t src_port;
    uint16_t dst_port;
    uint16_t length;
    uint16_t chksum;
} udp_hdr_t;

typedef int (*nano_udp_handler_t) (nano_ctx_t *ctx, size_t offset, ...);

typedef struct nano_udp_bind {
    clist_node_t next;
    uint16_t port;
    nano_udp_handler_t handler;
} nano_udp_bind_t;

extern clist_node_t nano_udp_binds;

int udp_handle(nano_ctx_t *ctx, size_t offset);
int udp_send(const iolist_t *iolist, uint32_t dst_ip, uint16_t dst_port, uint16_t src_port);
int udp6_send(const iolist_t *iolist, uint8_t* dst_ip, uint16_t dst_port, uint16_t src_port, nano_dev_t *dev);
int udp_reply(nano_ctx_t *ctx);

#endif /* NANO_UDP_H */
