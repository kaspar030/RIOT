#ifndef NANO_IPV4_H
#define NANO_IPV4_H

#include <stdint.h>
#include "nano_ctx.h"

int ipv4_handle(nano_ctx_t *ctx, size_t offset);
int ipv4_send(nano_sndbuf_t *buf, uint32_t dest_ip, int protocol);
int ipv4_reply(nano_ctx_t *ctx);

typedef struct __attribute__((packed)) ipv4_hdr {
    uint8_t ver_ihl;
    uint8_t dscp_ecn;
    uint16_t total_len;
    uint16_t identification;
    uint16_t flags_fragment_offset;
    uint8_t  ttl;
    uint8_t  protocol;
    uint16_t hdr_chksum;
    uint32_t src;
    uint32_t dst;
} ipv4_hdr_t;

static inline int is_ipv4_hdr(uint8_t *buf)
{
    ipv4_hdr_t* hdr = (ipv4_hdr_t*)buf;
    return (hdr->ver_ihl>>4) == 4;
}

static inline int ipv4_hdr_len(ipv4_hdr_t* hdr)
{
    /* hdr is split in half.
     * first half is IP version
     * second half is IHL (header length in 32bit words)
     *
     * So here we mask first half of byte and
     * multiply the result by 32bit word length.
     */
    return (hdr->ver_ihl & ~0xF0) * 4;
}


#endif /* NANO_IPV4_H */
