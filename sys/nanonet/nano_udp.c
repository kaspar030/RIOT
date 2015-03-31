#include "nano_udp.h"
#include "nano_ctx.h"
#include "nano_icmp.h"
#include "nano_ipv4.h"

#include "byteorder.h"

#define ENABLE_DEBUG 1
#include "debug.h"

int udp_handle(nano_ctx_t *ctx, char *buf, int len, int offset)
{
    udp_hdr_t *hdr = (udp_hdr_t*) (buf+offset);

    if ((len-offset) < (int)sizeof(udp_hdr_t)) {
        DEBUG("udp: truncated packet received.\n");
        return -1;
    }

    ctx->src_port = NTOHS(hdr->src_port);

    uint16_t dst_port = NTOHS(hdr->dst_port);
    ctx->dst_port = dst_port;

    DEBUG("udp: received packet from 0x%08x, src_port %u, dst_port %u\n",
            (unsigned int) ctx->src_ip, ctx->src_port, ctx->dst_port);

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
        return handler(ctx, buf, len, offset+sizeof(udp_hdr_t));
    }
    else {
        if (! (ctx->src_ip || (~ctx->dst_ip))) {
            /* also filter broadcast */
            DEBUG("udp: unreachable port %u\n", dst_port);
            icmp_port_unreachable(ctx);
        }
    }

    return 0;
}

int udp_send(uint32_t dest_ip, uint16_t dst_port, uint16_t src_port, char *buf, int buflen, int used) {
    udp_hdr_t *hdr;

    DEBUG("udp: sending packet to 0x%08x\n", (unsigned int) dest_ip);

    /* allocate our header at the end of buf, but before used bytes */
    hdr = (udp_hdr_t *)(buf + buflen - used - sizeof(udp_hdr_t));

    if ((char*) hdr < buf) {
        DEBUG("udp_send: buffer too small.\n");
        return -1;
    }

    hdr->src_port = HTONS(src_port);
    hdr->dst_port = HTONS(dst_port);

    hdr->length = HTONS(sizeof(udp_hdr_t) + used);

    /* zero checksum is fine with UDPv4 */
    hdr->chksum = 0x0;

    return ipv4_send(dest_ip, 0x11, buf, buflen, sizeof(udp_hdr_t) + used);
}
