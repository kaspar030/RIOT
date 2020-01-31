#include <string.h>

#include "byteorder.h"

#include "net/nano/icmp.h"
#include "net/nano/ipv4.h"
#include "net/nano/util.h"
#include "net/nano/config.h"

#define ENABLE_DEBUG ENABLE_NANONET_DEBUG
#include "debug.h"

static int icmp_echo_reply(nano_ctx_t *ctx, icmp_hdr_t *request, size_t len);
static void icmp_hdr_set(icmp_hdr_t *hdr, uint8_t type, uint8_t code, uint32_t rest, size_t len);

int icmp_handle(nano_ctx_t *ctx, size_t offset)
{
    icmp_hdr_t *hdr = (icmp_hdr_t *) (ctx->buf+offset);

    DEBUG("icmp: got packet with type=%u code=%u\n", hdr->type, hdr->code);

    switch (hdr->type) {
        case 0:
            DEBUG("icmp echo reply.\n");
            break;
        case 8:
            DEBUG("icmp echo request.\n");
            icmp_echo_reply(ctx, hdr, ctx->len - offset);
            break;
        default:
            DEBUG("icmp: unknown type %u\n", hdr->type);
    }

    return 0;
}

int icmp_port_unreachable(nano_ctx_t *ctx)
{
    ipv4_hdr_t *ipv4_hdr = (ipv4_hdr_t *)ctx->l3_hdr_start;

    /* we have to send back the ipv4 header
     * and the first 8 bytes of a packet.
     */
    int ipv4_hdr_len = (ipv4_hdr->ver_ihl & ~0xF0) * 4; /* max 60 bytes */
    int send_back_len = nano_min(ipv4_hdr_len + 8, ntohs(ipv4_hdr->total_len));

    /* allocate icmp header + send_back_len bytes */
    uint8_t buf[sizeof(icmp_hdr_t) + 68];
    /* copy sendback bytes behind header */
    memcpy((buf + sizeof(icmp_hdr_t)), ipv4_hdr, send_back_len);

    /* setup hdr fields */
    icmp_hdr_t *hdr = (icmp_hdr_t *)buf;
    icmp_hdr_set(hdr, ICMP4_TYPE_DST_UNREACH, ICMP4_CODE_PORT_UNREACH,
                 ICMP4_NO_DATA, sizeof(icmp_hdr_t) + send_back_len);

    /* setup iolist */
    iolist_t iolist = { NULL, buf, (sizeof(icmp_hdr_t) + send_back_len) };

    return ipv4_send(&iolist, ctx->src_addr.ipv4, 0x1);
}

static void icmp_hdr_set(icmp_hdr_t *hdr, uint8_t type, uint8_t code, uint32_t rest, size_t len) {
    hdr->type = type;
    hdr->code = code;
    hdr->chksum = 0;
    hdr->rest = rest;

    hdr->chksum = ~nano_util_calcsum(0, (uint8_t*)hdr, len);
}

static int icmp_echo_reply(nano_ctx_t *ctx, icmp_hdr_t *request, size_t len)
{
    iolist_t iolist = { NULL, request, len };

    icmp_hdr_set(request, ICMP4_TYPE_ECHO_REPLY, ICMP4_CODE_ECHO_REPLY, request->rest, len);

    return ipv4_send(&iolist, ctx->src_addr.ipv4, 0x1);
}
