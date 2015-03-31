#ifndef NANO_ICMP_H
#define NANO_ICMP_H

#include <stdint.h>
#include "nano_ctx.h"

typedef struct __attribute__((packed)) icmp_hdr {
    uint8_t type;
    uint8_t code;
    uint16_t chksum;
    uint32_t rest;
} icmp_hdr_t;

int icmp_handle(nano_ctx_t *ctx, char *buf, int len, int offset);
int icmp_port_unreachable(nano_ctx_t *ctx);

#endif /* NANO_ICMP_H */
