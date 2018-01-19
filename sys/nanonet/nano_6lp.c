#include <assert.h>
#include <errno.h>

#include "byteorder.h"
#include "nanonet.h"

#include "nano_icmpv6.h"

#define ENABLE_DEBUG ENABLE_NANONET_DEBUG
#include "debug.h"

#ifdef NANONET_6LP

/*
 * rfc6282 LOWPAN_IPHC header
 *
 *   0                                       1
 *   0   1   2   3   4   5   6   7   8   9   0   1   2   3   4   5
 *   5   4   3   2   1   0   9   8   7   6   5   4   3   2   1   0
 * +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 * | 0 | 1 | 1 |  TF   |NH | HLIM  |CID|SAC|  SAM  | M |DAC|  DAM  |
 * +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---
 */

#define BIT(n) (0x8000 >> n)

#define SIXLP_NH_COMPRESSED     BIT(5)
#define SIXLP_CID_PRESENT       BIT(8)
#define SIXLP_SAC_STATEFUL      BIT(9)
#define SIXLP_MULTICAST         BIT(12)
#define SIXLP_DAC_STATEFUL      BIT(13)

//#define SIXLP_HL(dispatch)      ((dispatch >> 6) & 0x3)
//#define SIXLP_SAM(dispatch)     ((dispatch >> 10) & 0x3)
//#define SIXLP_DAM(dispatch)     ((dispatch >> 14) & 0x3)
#define SIXLP_HL(dispatch)      ((dispatch >> 8) & 0x3)
#define SIXLP_SAM(dispatch)     ((dispatch >> 4) & 0x3)
#define SIXLP_DAM(dispatch)     (0x3)

#define SIXLP_BADADDR           (0xffff)

enum {
    SIXLP_HL_INLINE,
    SIXLP_HL_1,
    SIXLP_HL_64,
    SIXLP_HL_255
};

enum {
    SIXLP_STATELESS_ADDR_LEN_128,
    SIXLP_STATELESS_ADDR_LEN_64,
    SIXLP_STATELESS_ADDR_LEN_16,
    SIXLP_STATELESS_ADDR_LEN_0
};

enum {
    SIXLP_STATELESS_MADDR_LEN_128,
    SIXLP_STATELESS_MADDR_LEN_48,
    SIXLP_STATELESS_MADDR_LEN_32,
    SIXLP_STATELESS_MADDR_LEN_8
};

enum {
    SIXLP_STATEFUL_ADDR_UNSPEC,
    SIXLP_STATEFUL_ADDR_LEN_64,
    SIXLP_STATEFUL_ADDR_LEN_16,
    SIXLP_STATEFUL_ADDR_LEN_0
};

enum {
    SIXLP_DECOMP_SRC,
    SIXLP_DECOMP_DST
};

void nano_ieee802154_get_iid(const uint8_t *addr_in,
                             size_t addr_len, uint8_t *addr_out, int reverse)
{
    int i = 0;

    uint8_t addr_in_reversed[8];
    if (reverse) {
        addr_in += 7;
        for (int i = 0; i < 8; i++) {
            addr_in_reversed[i] = *addr_in--;
        }
        addr_in = addr_in_reversed;
    }

    addr_out[0] = addr_out[1] = 0;

    switch (addr_len) {
        case 8:
            memcpy(addr_out, addr_in, addr_len);
            addr_out[0] ^= 0x02;
            break;

        case 4:
            addr_out[0] = addr_in[i++] ^ 0x02;
            addr_out[1] = addr_in[i++];
            /* falls through */
        case 2:
            addr_out[2] = 0;
            addr_out[3] = 0xff;
            addr_out[4] = 0xfe;
            addr_out[5] = 0;
            addr_out[6] = addr_in[i++];
            addr_out[7] = addr_in[i++];
            break;

        default:
            assert(false);
    }
}

static unsigned _stateless(uint8_t *addr_out, uint8_t *pktpos, unsigned addr_mode)
{
    switch (addr_mode) {
        case SIXLP_STATELESS_ADDR_LEN_128:
        {
            memcpy(addr_out, pktpos, IPV6_ADDR_LEN);
            return IPV6_ADDR_LEN;
        }
        case SIXLP_STATELESS_ADDR_LEN_64:
        {
            memset(addr_out + 2, '\0', 6);
            addr_out[0] = 0xfe;
            addr_out[1] = 0x80;
            memcpy(addr_out + (IPV6_ADDR_LEN / 2), pktpos, IPV6_ADDR_LEN / 2);
            return IPV6_ADDR_LEN / 2;
        }
        case SIXLP_STATELESS_ADDR_LEN_16:
        {
            memset(addr_out + 2, '\0', 12);
            addr_out[0] = 0xfe;
            addr_out[1] = 0x80;
            addr_out[11] = 0xff;
            addr_out[12] = 0xfe;
            addr_out[14] = pktpos[0];
            addr_out[15] = pktpos[1];
            return 2;
        }
        case SIXLP_STATELESS_ADDR_LEN_0:
        {
            addr_out[0] = 0xfe;
            addr_out[1] = 0x80;
            memset(addr_out + 2, '\0', 6);
            return 0;
        }
        default:
            return 0;
    }
}

static unsigned _stateless_mcast(uint8_t *addr_out, uint8_t *pktpos, unsigned addr_mode)
{
    switch (addr_mode) {
        case SIXLP_STATELESS_MADDR_LEN_128:
        {
            memcpy(addr_out, pktpos, IPV6_ADDR_LEN);
            return IPV6_ADDR_LEN;
        }
        case SIXLP_STATELESS_MADDR_LEN_48:
        {
            memset(addr_out + 2, '\0', 7);
            addr_out[0] = 0xff;
            addr_out[1] = pktpos[0];
            memcpy(addr_out + 9, pktpos + 1, 6);
            return 6;
        }
        case SIXLP_STATELESS_MADDR_LEN_32:
        {
            memset(addr_out + 2, '\0', 9);
            addr_out[0] = 0xff;
            addr_out[1] = pktpos[0];
            memcpy(addr_out + 12, pktpos + 1, 3);
            return 4;
        }
        case SIXLP_STATELESS_MADDR_LEN_8:
        {
            memset(addr_out + 2, '\0', 13);
            addr_out[0] = 0xff;
            addr_out[1] = 0x02;
            addr_out[1] = *pktpos;
            return 1;
        }
        default:
            return 0;
    }
}

static unsigned _stateful(uint8_t *addr_out, uint8_t *pktpos, unsigned addr_mode, unsigned multicast)
{
    (void)pktpos;
    DEBUG("_stateful(): addr_mode=0x%02x (%u)\n", addr_mode, (multicast != 0));
    if (!multicast) {
        return SIXLP_BADADDR;
    }
    else {
        switch (addr_mode) {
            case SIXLP_STATEFUL_ADDR_UNSPEC:
                {
                    memset(addr_out, '\0', IPV6_ADDR_LEN);
                    return 0;
                }
            default:
                return SIXLP_BADADDR;
        }
    }
}

static size_t _decomp_addr(nano_ctx_t *ctx, uint8_t *pktpos, unsigned dispatch, unsigned src_or_dst)
{
    uint8_t *addr;
    unsigned stateful;
    unsigned addr_mode;
    unsigned addr_len;

    if (src_or_dst == SIXLP_DECOMP_SRC) {
        addr = ctx->src_addr.ipv6;
        stateful = (dispatch & SIXLP_SAC_STATEFUL);
        addr_mode = SIXLP_SAM(dispatch);
    }
    else {
        addr = ctx->dst_addr.ipv6;
        stateful = (dispatch & SIXLP_DAC_STATEFUL);
        addr_mode = SIXLP_DAM(dispatch);
    }

    if (!stateful) {
        if ((src_or_dst == SIXLP_DECOMP_SRC) || !(dispatch & SIXLP_MULTICAST)) {
            addr_len = _stateless(addr, pktpos, addr_mode);
        }
        else {
            addr_len = _stateless_mcast(addr, pktpos, addr_mode);
        }
        if (addr_len) {
            DEBUG("_decomp_addr(): stateless\n");
        }
        else {
            DEBUG("_decomp_addr(): stateless from_iid\n");
            addr[0] = 0xfe;
            addr[1] = 0x80;
            memset(addr + 2, '\0', 6);
            if (src_or_dst == SIXLP_DECOMP_SRC) {
                nano_ieee802154_get_iid(ctx->src_mac, ctx->src_mac_len, addr + 8, 1);
            }
            else {
                nano_ieee802154_get_iid(ctx->dst_mac, ctx->dst_mac_len, addr + 8, 1);
            }
        }
    }
    else {
        addr_len = _stateful(addr, pktpos, addr_mode, dispatch & SIXLP_MULTICAST);
        if (addr_len == SIXLP_BADADDR) {
            DEBUG("_decomp_addr(): stateful (unsupported)\n");
        }
    }

    return addr_len;
}

static size_t _un_iphc(nano_ctx_t *ctx, size_t offset)
{
    uint8_t *pktpos = ctx->buf + offset;

    /* get dispatch bytes */
    uint16_t dispatch;
    unsigned cid = 0;
    unsigned nh = 0;

    memcpy(&dispatch, pktpos, sizeof(dispatch));
    dispatch = ntohs(dispatch);
    pktpos += 2;

    DEBUG("_un_iphc(): base=0x%04x HL=0x%02x NH=%u CID=%u SAC=%u SAM=0x%02x M=%u DAC=%u DAM=%02x\n",
            (unsigned)dispatch,
            (SIXLP_HL(dispatch)),
            ((dispatch & SIXLP_NH_COMPRESSED) != 0),
            ((dispatch & SIXLP_CID_PRESENT) != 0),
            ((dispatch & SIXLP_SAC_STATEFUL) != 0),
            (SIXLP_SAM(dispatch)),
            ((dispatch & SIXLP_MULTICAST) != 0),
            ((dispatch & SIXLP_DAC_STATEFUL) != 0),
            (SIXLP_DAM(dispatch))
            );

    /* Next Header */
    if (! (dispatch & SIXLP_NH_COMPRESSED)) {
        nh = *pktpos++;
        DEBUG("_un_iphc(): NH=0x%02x\n", (unsigned)nh);
    }

    /* hop limit */
    if (!SIXLP_HL(dispatch)) {
        pktpos++;
    }

    /* Context Identifier */
    if (dispatch & SIXLP_CID_PRESENT) {
        cid = *pktpos++;
        DEBUG("_un_iphc(): CID=0x%02x\n", (unsigned)cid);
    }

    /* dispatch source & destination addresses */
    for (int i = 0; i < 2; i++) {
        unsigned tmp =_decomp_addr(ctx, pktpos, dispatch, i);
        if (tmp == SIXLP_BADADDR) {
            DEBUG("_parse_6lp(): error decoding %s address.\n",
                    (i == SIXLP_DECOMP_SRC) ? "source" : "destination");
            return 0;
        }
        else {
            pktpos += tmp;
        }
    }

#if ENABLE_DEBUG
    DEBUG("iphc src addr: ");
    ipv6_addr_print(ctx->src_addr.ipv6);
    DEBUG("\niphc dst addr: ");
    ipv6_addr_print(ctx->dst_addr.ipv6);
    DEBUG("\n");
#endif

    ctx->l4_start = pktpos;
    ipv6_dispatch(ctx, pktpos - ctx->buf, nh);

    return (pktpos - ctx->buf);
}

static inline int _is_iphc(nano_ctx_t *ctx, size_t offset)
{
    /* check if first byte starts with bit sequence 011 */
    return ((*(ctx->buf + offset)) & 0xe0) == 0x60;
}

int nano_6lp_send(const iolist_t *iolist, uint8_t *dst_ip, int protocol, nano_dev_t *dev)
{
    if (dev && !ipv6_addr_is_link_local(dst_ip)) {
        DEBUG("nano_6lp_send(): device given, but address is not link-local.\n");
        return -EINVAL;
    }

    if (!ipv6_addr_is_link_local(dst_ip)) {
        DEBUG("nano_6lp_send(): only linc-local supported.\n");
        return -EINVAL;
    }

    /* dispatch + NH + srcaddr + dstaddr */
    uint8_t buf[2 + 1 + (2*IPV6_ADDR_LEN)];
    uint8_t *hdr = buf;

    /* insert hdr into iolist */
    iolist_t _iolist = { (iolist_t *)iolist, buf, sizeof(buf) };

    *hdr++ = 0x7b;  /* IPHC, TF elided, HL255, inline NH, no CI */
    hdr++; /* no address compression */
    *hdr++ = protocol;

    ipv6_get_src_addr(hdr, dev, dst_ip);
    hdr += IPV6_ADDR_LEN;

    memcpy(hdr, dst_ip, IPV6_ADDR_LEN);
    hdr += IPV6_ADDR_LEN;

    uint8_t l2addr[8];
    memcpy(l2addr, dst_ip + 8, 8);
    l2addr[0] ^= 0x2;

    return dev->send(dev, &_iolist, l2addr, 0);
}

int nano_6lp_reply(nano_ctx_t *ctx)
{
    DEBUG("nano_6lp_reply() len=%u\n", (unsigned)ctx->len);
    /* uint8_t nh = _get_nh(ctx->l3_hdr_start); */
    uint8_t nh=0x3b;

    /* create iolist with l4 data */
    iolist_t iolist = { NULL, ctx->l4_start, (ctx->buf + ctx->len - ctx->l4_start) };

    /* send L4 data */
    nano_6lp_send(&iolist, ctx->src_addr.ipv6, nh, ctx->dev);

    return 0;
}

int nano_6lp_handle(nano_ctx_t *ctx, size_t offset)
{
    DEBUG("sixlp_handle(): handling packet with length %u\n", ctx->len - offset);

    /* provide space for possibly compressed IPv6 src/dst addresses */

    uint8_t src_addr[IPV6_ADDR_LEN];
    uint8_t dst_addr[IPV6_ADDR_LEN];

    ctx->src_addr.ipv6 = src_addr;
    ctx->dst_addr.ipv6 = dst_addr;

    if (_is_iphc(ctx, offset)) {
        _un_iphc(ctx, offset);
    }
    else {
        /* TODO: other dispatch types */
        DEBUG("sixlp_handle(): unhandled dispatch byte.\n");
        return 0;
    }

    return 0;
}

#endif /* NANONET_6LP */
