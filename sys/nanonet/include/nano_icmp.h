#ifndef NANO_ICMP_H
#define NANO_ICMP_H

#include <stdint.h>
#include <stddef.h>

#include "nano_ctx.h"

typedef struct __attribute__((packed)) icmp_hdr {
    uint8_t type;
    uint8_t code;
    uint16_t chksum;
    uint32_t rest;
} icmp_hdr_t;

int icmp_handle(nano_ctx_t *ctx, size_t offset);
int icmp_port_unreachable(nano_ctx_t *ctx);

#define ICMP4_TYPE_ECHO_REPLY       (0)
#define ICMP4_TYPE_DST_UNREACH      (3)
#define ICMP4_TYPE_ECHO_REQ         (8)

#define ICMP4_CODE_ECHO_REPLY       (0)
#define ICMP4_CODE_PORT_UNREACH     (3)

#define ICMP4_NO_DATA               (0)

#endif /* NANO_ICMP_H */
