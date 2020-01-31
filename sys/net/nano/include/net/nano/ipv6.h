#ifndef NANO_IPV6_H
#define NANO_IPV6_H

#include <stdint.h>
#include <string.h>

#include "iolist.h"
#include "net/nano/ctx.h"

#define IPV6_ADDR_LEN   (16)

#define IPV6_HDR_LEN    (40)

#define IPV6_NEXTHDR_UDP        (17U)
#define IPV6_NEXTHDR_ICMP       (58U)

int ipv6_handle(nano_ctx_t *ctx, size_t offset);
void ipv6_dispatch(nano_ctx_t *ctx, size_t l4offset, uint8_t next_header);
int ipv6_send(const iolist_t *iolist, uint8_t *dest_ip, int protocol, nano_dev_t *dev);
int ipv6_reply(nano_ctx_t *ctx);
void ipv6_addr_print(const uint8_t *addr);
int ipv6_get_src_addr(uint8_t *tgt_buf, const nano_dev_t *dev, const uint8_t *tgt_addr);

static inline int ipv6_addr_equal(const uint8_t *a, const uint8_t* b) {
    return memcmp(a, b, 16) == 0;
}

static inline int ipv6_is_for_us(nano_ctx_t *ctx)
{
    return ipv6_addr_equal(ctx->dst_addr.ipv6, ctx->dev->ipv6_ll)
        || ipv6_addr_equal(ctx->dst_addr.ipv6, ctx->dev->ipv6_global)
        || (ctx->dst_addr.ipv6[0] == 0xFF);
}

static inline int ipv6_addr_is_link_local(const uint8_t *addr)
{
    return addr[0]==0xfe && addr[1]==0x80;
}

static inline int ipv6_addr_is_multicast(const uint8_t *addr)
{
    return addr[0]==0xFF;
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
