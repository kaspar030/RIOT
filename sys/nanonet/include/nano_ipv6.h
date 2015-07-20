#ifndef NANO_IPV6_H
#define NANO_IPV6_H

#include <stdint.h>
#include <string.h>
#include "nano_ctx.h"

#define IPV6_ADDR_LEN   (16)

#define IPV6_HDR_LEN    (40)

#define IPV6_NEXTHDR_UDP        (17U)
#define IPV6_NEXTHDR_ICMP       (58U)

int ipv6_handle(nano_ctx_t *ctx, size_t offset);
int ipv6_send(uint8_t *dest_ip, int protocol, uint8_t *buf, size_t len, size_t used);
int ipv6_reply(nano_ctx_t *ctx);
void ipv6_addr_print(const uint8_t *addr);

static inline int ipv6_addr_equal(const uint8_t *a, const uint8_t* b) {
    return memcmp(a,b,16) == 0;
}

typedef struct __attribute__((packed)) ipv6_hdr {
    uint32_t ver_tc_fl;
    uint16_t payload_len;
    uint8_t next_header;
    uint8_t hop_limit;
    uint8_t src[IPV6_ADDR_LEN];
    uint8_t dst[IPV6_ADDR_LEN];
} ipv6_hdr_t;


#endif /* NANO_IPV6_H */
