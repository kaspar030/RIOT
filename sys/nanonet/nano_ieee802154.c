#include <string.h>

#include "byteorder.h"
#include "nanonet.h"

#ifdef NANONET_6LP
#include "nano_6lp.h"
#endif

#define ENABLE_DEBUG ENABLE_NANONET_DEBUG
#include "debug.h"

#if ENABLE_DEBUG
#include "fmt.h"
#endif

#ifdef NANONET_IEEE802154

/*
 * IEEE 802.15.4 MAC header
 *
 * Frame Control field (FC):
 *
 * |-------+----------+---------+----+--------+-----+---------+-------+---------|
 * | 0-2   | 3        | 4       | 5  | 6      | 7-9 | 10-11   | 12-13 | 14-15   |
 * |-------+----------+---------+----+--------+-----+---------+-------+---------|
 * | Frame | Security | Frame   | AR | PAN ID | res | dstaddr | ver   | srcaddr |
 * | Type  | Enabled  | pending |    | compr  |     | mode    |       | mode    |
 * |-------+----------+---------+----+--------+-----+---------+-------+---------|
*/

enum {
    FC_TYPE_BEACON,
    FC_TYPE_DATA,
    FC_TYPE_ACK,
    FC_TYPE_MAC_COMMAND
};

enum {
    FC_ADDR_ABSENT,
    FC_ADDR_RESERVED,
    FC_ADDR_SHORT,
    FC_ADDR_LONG
};

#define BIT(n) (1U << n)

#define FC_FLAG_SECURITY_ENABLED    BIT(3)
#define FC_FLAG_FRAME_PENDING       BIT(4)
#define FC_FLAG_AR                  BIT(5)
#define FC_FLAG_PANID_COMPRESSED    BIT(6)

#define FC_TYPE(fc)                 (fc & 0x7)
#define FC_DST_ADDRMODE(fc)         ((fc >> 10) & 0x3)
#define FC_SRC_ADDRMODE(fc)         ((fc >> 14) & 0x3)

static unsigned _parse_addrlen(unsigned mode)
{
    switch (mode) {
        case FC_ADDR_ABSENT:
            {
                DEBUG(",absent");
                return 0;
            }
        case FC_ADDR_RESERVED:
            {
                DEBUG(",reserved");
                return 0;
            }
        case FC_ADDR_SHORT:
            {
                DEBUG(",short");
                return 2;
            }
        case FC_ADDR_LONG:
            {
                DEBUG(",long");
                return 8;
            }
        default:
            return 0;
    }
}

static unsigned _parse_hdr(nano_ctx_t *ctx)
{
    /* TODO: bail out on invalid FC */

    unsigned dlen, slen;
    uint8_t *buf = ctx->buf;

    /* get FC field */
    uint16_t fc;
    memcpy(&fc, buf, sizeof(fc));
    buf += 2;

    DEBUG("FC=0x%04x frame type:", (unsigned)fc);
    switch (FC_TYPE(fc)) {
        case FC_TYPE_BEACON: {
                    DEBUG("beacon");
                    break;
                }
        case FC_TYPE_DATA:
                {
                    DEBUG("data");
                    /* data */
                    break;
                }
        case FC_TYPE_ACK:
                {
                    DEBUG("ACK");
                    break;
                }
        case FC_TYPE_MAC_COMMAND:
                {
                    DEBUG("MAC_command");
                    break;
                }
        default:
                {
                    DEBUG("reserved");
                }
    }

    if (fc & FC_FLAG_SECURITY_ENABLED) {
        DEBUG(",sec");
    }

    if (fc & FC_FLAG_FRAME_PENDING) {
        DEBUG(",pend");
    }

    if (fc & FC_FLAG_AR) {
        DEBUG(",AR");
    }

    /* sequence number */
    /*uint8_t seq = **/buf++;

    /* copy destination pan */
    memcpy(&ctx->dst_pan, buf, sizeof(ctx->dst_pan));
    buf += 2;

    /* handle destination address mode */
    dlen = _parse_addrlen(FC_DST_ADDRMODE(fc));
    if (dlen) {
        ctx->dst_mac = buf;
        buf += dlen;
    }
    ctx->dst_mac_len = dlen;

    /* handle PAN ID compression */
    if (fc & FC_FLAG_PANID_COMPRESSED) {
        DEBUG(",panid_compressed");
        ctx->src_pan = ctx->dst_pan;
    }
    else {
        memcpy(&ctx->src_pan, buf, sizeof(ctx->src_pan));
        buf += 2;
    }

    /* handle source address mode */
    slen = _parse_addrlen(FC_SRC_ADDRMODE(fc));
    if (slen) {
        ctx->src_mac = buf;
        buf += slen;
    }
    ctx->src_mac_len = slen;

    DEBUG("\n");

    return (buf - ctx->buf);
}

static uint16_t _gen_fc(unsigned type, unsigned flags, unsigned dstaddrmode, unsigned srcaddrmode)
{
    return (type & 0x7) | flags | (dstaddrmode >> 10) & 0x3 | (srcaddrmode >> 14) & 0x3;
}


static int nano_ieee80154_send(nano_dev_t *dev, const iolist_t *iolist, uint8_t* dest_l2addr, size_t l2addrlen)
{
    DEBUG("nano_ieee80154_send(): Sending packet with len %u\n", (unsigned)iolist_size(iolist));

    uint8_t hdr[2 + 2 + 32]; /* FC + DST_PAN + 2 times full length address */
    iolist_t _iolist = { (iolist_t *)iolist, hdr, 4 };

    unsigned flags = 0;

    if (l2addrlen == 4) {
        /* target has a PAN + short adddress */
        flags &=
    }
    else {
        /* target is using long address */
    }

    _iolist->next = iolist;

    netdev->driver->send(netdev, &_iolist);

    return 0;
}

int nano_ieee802154_reply(nano_ctx_t *ctx)
{
    (void)ctx;
    DEBUG("nano_ieee802154_reply()\n");
    return 0;
}

void nano_ieee802154_handle(nano_dev_t *dev, uint8_t *buf, size_t len)
{
    DEBUG("nano_ieee802154_handle() buf=0x%08x len=%u\n", (unsigned)buf, (unsigned)len);

    /* setup crosslayer context struct */
    nano_ctx_t ctx = { .dev=dev, .buf=buf, .len=len};

    size_t hdrlen = _parse_hdr(&ctx);
    if (hdrlen) {
#if ENABLE_DEBUG
        print_str("src_mac: ");
        char addrbuf[16];
        fmt_bytes_hex_reverse(addrbuf, ctx.src_mac, ctx.src_mac_len);
        print(addrbuf, ctx.src_mac_len * 2);
        print_str(" dst_mac: ");
        fmt_bytes_hex_reverse(addrbuf, ctx.dst_mac, ctx.dst_mac_len);
        print(addrbuf, ctx.dst_mac_len * 2);
        print_str("\n");
#endif
#ifdef NANONET_6LP
        ctx.l3_hdr_start = ctx.buf + hdrlen;
        nano_6lp_handle(&ctx, hdrlen);
#endif
    }
    else {
        DEBUG("nano_ieee802154_handle(): dropping pkt due to hdr parsing error.\n");
    }
}

#endif /* NANONET_IEEE802154 */
