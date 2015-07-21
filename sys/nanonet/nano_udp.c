#include <errno.h>

#include "nano_udp.h"
#include "nano_ctx.h"
#include "nano_icmp.h"
#include "nano_ipv4.h"
#include "nano_ipv6.h"

#include "byteorder.h"

#define ENABLE_DEBUG 1
#include "debug.h"

int udp_handle(nano_ctx_t *ctx, size_t offset)
{
    udp_hdr_t *hdr = (udp_hdr_t*) (ctx->buf+offset);

    if ((ctx->len-offset) < (int)sizeof(udp_hdr_t)) {
        DEBUG("udp: truncated packet received.\n");
        return -1;
    }

    ctx->src_port = NTOHS(hdr->src_port);

    uint16_t dst_port = NTOHS(hdr->dst_port);
    ctx->dst_port = dst_port;

#if ENABLE_DEBUG == 1
    if (is_ipv4_hdr(ctx->l3_hdr_start)) {
        DEBUG("udp: received packet from 0x%08x, src_port %u, dst_port %u\n",
                (unsigned int) ctx->src_addr.ipv4, ctx->src_port, ctx->dst_port);
    } else {
        DEBUG("udp: received UDPv6 packet src_port %u, dst_port %u\n",
                ctx->src_port, ctx->dst_port);
    }
#endif

    nano_udp_handler_t handler = NULL;
    nano_udp_bind_t *bind = &nano_udp_binds[0];
    while (bind) {
        if (bind->port == dst_port) {
            handler = bind->handler;
            break;
        }
        else if (bind->port > dst_port) {
            break;
        }
        bind++;
    }

    if (handler) {
        return handler(ctx, offset+sizeof(udp_hdr_t));
    }
    else {
        if (is_ipv4_hdr(ctx->l3_hdr_start)) {
            if (! (ctx->src_addr.ipv4 || (~ctx->dst_addr.ipv4))) {
                /* also filter broadcast */
                DEBUG("udp: unreachable port %u\n", dst_port);
                icmp_port_unreachable(ctx);
            }
        }
        else {
            puts("udp: UDPv6: not filtering broadcast icmp reply");
        }
    }

    return 0;
}


static size_t udp_build_hdr(uint16_t dst_port, uint16_t src_port, uint8_t *buf, size_t buflen, size_t used) {
    udp_hdr_t *hdr;

    /* allocate our header at the end of buf, but before used bytes */
    hdr = (udp_hdr_t *)(buf + buflen - used - sizeof(udp_hdr_t));

    if ((uint8_t*) hdr < buf) {
        DEBUG("udp_build_hdr: buffer too small.\n");
        return 0;
    }

    hdr->src_port = HTONS(src_port);
    hdr->dst_port = HTONS(dst_port);

    hdr->length = HTONS(sizeof(udp_hdr_t) + used);

    /* checksum will be calculated by IP layer */
    hdr->chksum = 0x0;

    return used + sizeof(udp_hdr_t);
}

int udp_send(uint32_t dst_ip, uint16_t dst_port, uint16_t src_port, uint8_t *buf, size_t buflen, size_t used) {

    DEBUG("udp: sending packet to 0x%08x\n", (unsigned int) dst_ip);

    if (!udp_build_hdr(dst_port, src_port, buf, buflen, used)) {
            return -ENOSPC;
    }

    return ipv4_send(dst_ip, 0x11, buf, buflen, sizeof(udp_hdr_t) + used);
}

int udp6_send(uint8_t* dst_ip, uint16_t dst_port, uint16_t src_port, uint8_t *buf, size_t buflen, size_t used) {
    DEBUG("udp: sending udpv6 packet packet\n");

    if (!udp_build_hdr(dst_port, src_port, buf, buflen, used)) {
            return -ENOSPC;
    }

    return ipv6_send(dst_ip, IPV6_NEXTHDR_UDP, buf, buflen, sizeof(udp_hdr_t) + used);
}
