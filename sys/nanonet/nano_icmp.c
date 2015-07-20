#include <string.h>

#include "byteorder.h"

#include "nano_icmp.h"
#include "nano_ipv4.h"
#include "nano_util.h"
#include "nano_config.h"

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
    uint8_t buf[256];

    ipv4_hdr_t *ipv4_hdr = (ipv4_hdr_t *) ctx->l3_hdr_start;

    /* we have to send back the ipv4 header
     * and the first 8 bytes of a packet.
     */
    int ipv4_hdr_len = (ipv4_hdr->ver_ihl & ~0xF0) * 4;
    int send_back_len = nano_min(ipv4_hdr_len + 8, NTOHS(ipv4_hdr->total_len));

    /* allocate icmp header + send_back_len bytes at the end of buf */
    icmp_hdr_t *hdr = (icmp_hdr_t *) (buf+sizeof(buf)-sizeof(icmp_hdr_t)-send_back_len);

    /* copy sendback bytes behind header */
    memcpy(((char*)hdr) + sizeof(icmp_hdr_t), (char*)ipv4_hdr, send_back_len);

    /* setup hdr fields */
    icmp_hdr_set(hdr, 3, 3, 0, sizeof(icmp_hdr_t) + send_back_len);

    return ipv4_send(ctx->src_ip, 0x1, buf, sizeof(buf), sizeof(icmp_hdr_t) + send_back_len);
}

static void icmp_hdr_set(icmp_hdr_t *hdr, uint8_t type, uint8_t code, uint32_t rest, size_t len) {
    hdr->type = type;
    hdr->code = code;
    hdr->chksum = 0;
    hdr->rest = rest;

    hdr->chksum = nano_calcsum((uint16_t*)hdr, len);
}

static int icmp_echo_reply(nano_ctx_t *ctx, icmp_hdr_t *request, size_t len)
{
    uint8_t buf[256];

    icmp_hdr_t *reply = (icmp_hdr_t *) (buf+sizeof(buf)-len);

    if ((uint8_t*)reply < buf) {
        DEBUG("icmp_echo_reply(): buffer too small.\n");
        return -1;
    }

    memcpy(reply, request, len);

    icmp_hdr_set(reply, 0, 0, request->rest, len);

    return ipv4_send(ctx->src_ip, 0x1, buf, sizeof(buf), len);
}
