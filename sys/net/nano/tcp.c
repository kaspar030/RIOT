#include "byteorder.h"
#include "clist.h"
#include "iolist.h"
#include "net/nano.h"
#include "net/nano/icmp.h"
#include "net/nano/tcp.h"
#include "net/nano/util.h"
#include "random.h"
#include "tsrb.h"
#include "kernel_defines.h"

#define ENABLE_DEBUG ENABLE_NANONET_DEBUG
#include "debug.h"

#define TCP_FLAG_FIN    (1<<0)
#define TCP_FLAG_SYN    (1<<1)
#define TCP_FLAG_RST    (1<<2)
#define TCP_FLAG_PSH    (1<<3)
#define TCP_FLAG_ACK    (1<<4)
#define TCP_FLAG_URG    (1<<5)
#define TCP_FLAG_ECE    (1<<6)
#define TCP_FLAG_CWR    (1<<7)

typedef int (*tcp_state_handler_t)(nano_ctx_t *ctx, tcp_tcb_t *tcb, tcp_hdr_t *hdr);

static int tcp_reset(tcp_tcb_t *tcb);
static int tcp_send_ack(tcp_tcb_t *tcb);
static int tcp_send_fin(tcp_tcb_t *tcb);
static int tcp_send_finack(tcp_tcb_t *tcb);
static int tcp_send_syn(tcp_tcb_t *tcb);
static void tcp_event_handler(event_t *event);
static size_t tcp_send_max(tcp_tcb_t *tcb, size_t val);

static uint16_t tcp_window_available(tcp_tcb_t *tcb);

static clist_node_t _tcp_list;

int tcp_init(tcp_tcb_t *tcb, uint8_t *buf, size_t buf_len)
{
    memset(tcb, 0, sizeof(*tcb));
    tsrb_init(&tcb->rcv_buf, buf, buf_len);
    tcb->event.handler = tcp_event_handler;
    return 0;
}

int tcp_connect(tcp_tcb_t *tcb, uint32_t dst_ip, uint16_t dst_port, uint16_t src_port)
{
    tcb->src_port = src_port;
    tcb->dst_port = dst_port;
    tcb->peer.ipv4 = dst_ip;

    /* TODO: implement RFC initial sequence number */
    tcb->iss = random_uint32();

    clist_rpush(&_tcp_list, &tcb->next);

    return tcp_send_syn(tcb);
}

int tcp_write(tcp_tcb_t *tcb, const iolist_t *iolist)
{
    if (tcb->snd_iolist) {
        return -EBUSY;
    }

    tcb->snd_iolist = iolist;
    tcb->snd_pos = 0;
    tcb->flags |= TCP_SND_REQ;

    event_post(&nanonet_events, &tcb->event);

    /* double-lock snd_mutex */
    mutex_lock(&tcb->snd_mutex);
    mutex_lock(&tcb->snd_mutex);

    return 0;
}

int tcp_close(tcp_tcb_t *tcb)
{
    DEBUG("tcp_close(): closing connection\n");
    tcb->state = FIN_WAIT_1;
    tcb->snd_nxt += 1;

    return tcp_send_fin(tcb);
}

static size_t tcp_send_max(tcp_tcb_t *tcb, size_t val)
{
    size_t wnd_avail = tcb->snd_una + tcb->snd_wnd - tcb->snd_nxt;

    return val < wnd_avail ? val : wnd_avail;
}

static void tcp_event_handler(event_t *event)
{
    tcp_tcb_t *tcb = container_of(event, tcp_tcb_t, event);
    if (tcb->flags & TCP_SND_REQ) {
        DEBUG("tcp_event_handler() got send request\n");
        tcb->flags &= ~TCP_SND_REQ;
        size_t snd_size = iolist_size(tcb->snd_iolist);
        size_t to_send = tcp_send_max(tcb, snd_size);
        if (to_send) {
            DEBUG("tcp: sending %zu bytes\n", to_send);
        }
    }
}

/* returns the available rx buffer space in network byte order */
static uint16_t tcp_window_available(tcp_tcb_t *tcb)
{
    size_t avail = tcb->rcv_buf.size - tsrb_avail(&tcb->rcv_buf);
    if (avail > 0xffff) {
        return 0xffff;
    }
    else {
        return htons(avail);
    }
}

static void tcp_build_hdr(tcp_tcb_t *tcb, tcp_hdr_t *hdr, uint8_t flags)
{
    memset(hdr, 0, sizeof(*hdr));

    hdr->src_port = htons(tcb->src_port);
    hdr->dst_port = htons(tcb->dst_port);
    hdr->data_offset = (5<<4);
    hdr->flags = flags;
    hdr->window_size = tcp_window_available(tcb);
}

static int tcp_send_hdr(tcp_tcb_t *tcb, tcp_hdr_t *hdr)
{
    DEBUG("tcp_send_hdr()\n");
    /* TODO: handle possible options */
    /* TODO: set up timeout */
    iolist_t iolist = { .iol_base=hdr, .iol_len=sizeof(*hdr) };
    return ipv4_send(&iolist, tcb->peer.ipv4, 0x6);
}

static void tcp_cleanup(tcp_tcb_t *tcb)
{
    tcb->state = CLOSED;
    clist_remove(&_tcp_list, &tcb->next);
}

static int tcp_send_syn(tcp_tcb_t *tcb)
{
    DEBUG("tcp_send_syn() sending SYN\n");
    tcb->state = SYN_SENT;

    tcp_hdr_t hdr;
    tcp_build_hdr(tcb, &hdr, TCP_FLAG_SYN);

    hdr.seq_nr = htonl(tcb->iss);

    return tcp_send_hdr(tcb, &hdr);
}

static int tcp_reset(tcp_tcb_t *tcb)
{
    tcp_cleanup(tcb);

    DEBUG("tcp_reset() sending RST\n");

    tcp_hdr_t hdr;
    tcp_build_hdr(tcb, &hdr, TCP_FLAG_RST);

    return tcp_send_hdr(tcb, &hdr);
}

static int tcp_reply_connection_refused(nano_ctx_t *ctx, size_t offset)
{
    tcp_hdr_t *hdr = (tcp_hdr_t *) (ctx->buf+offset);

    uint16_t src_port = hdr->dst_port;
    uint16_t dst_port = hdr->src_port;
    uint32_t irs = hdr->seq_nr;

    /* re-use incoming header */
    memset(hdr, 0, sizeof(*hdr));

    /* these are already in network byte order */
    hdr->src_port = src_port;
    hdr->dst_port = dst_port;

    hdr->data_offset = (5<<4);
    hdr->flags = TCP_FLAG_RST | (hdr->flags & TCP_FLAG_SYN ? TCP_FLAG_ACK : 0);
    hdr->ack_nr = htonl(ntohl(irs) + 1);

    /* chop reply after our header */
    ctx->len = offset + sizeof(*hdr);

    return ipv4_reply(ctx);
}

static int tcp_send_ack(tcp_tcb_t *tcb)
{
    DEBUG("tcp_send_ack() sending ACK\n");
    tcp_hdr_t hdr;
    tcp_build_hdr(tcb, &hdr, TCP_FLAG_ACK);

    hdr.seq_nr = htonl(tcb->iss + tcb->snd_nxt);
    hdr.ack_nr = htonl(tcb->irs + tcb->rcv_nxt);

    return tcp_send_hdr(tcb, &hdr);
}

static int tcp_send_fin(tcp_tcb_t *tcb)
{
    DEBUG("tcp_send_fin() sending FIN\n");
    tcp_hdr_t hdr;
    tcp_build_hdr(tcb, &hdr, TCP_FLAG_FIN);

    hdr.seq_nr = htonl(tcb->iss + tcb->snd_nxt);
    hdr.ack_nr = htonl(tcb->irs + tcb->rcv_nxt);

    return tcp_send_hdr(tcb, &hdr);
}

static int tcp_send_finack(tcp_tcb_t *tcb)
{
    DEBUG("tcp_send_finack() sending FIN, ACK\n");
    tcp_hdr_t hdr;
    tcp_build_hdr(tcb, &hdr, TCP_FLAG_FIN | TCP_FLAG_ACK);

    hdr.seq_nr = htonl(tcb->iss + tcb->snd_nxt);
    hdr.ack_nr = htonl(tcb->irs + tcb->rcv_nxt);

    return tcp_send_hdr(tcb, &hdr);
}

static int tcp_handle_synsent(nano_ctx_t *ctx, tcp_tcb_t *tcb, tcp_hdr_t *hdr)
{
    (void)ctx;
    if (hdr->flags & TCP_FLAG_ACK) {
        if (hdr->flags & TCP_FLAG_SYN) {
            DEBUG("tcp_handle_synsent() established\n");

            tcb->state = ESTABLISHED;

            /* TODO: check received seq nr */

            tcb->irs = ntohl(hdr->seq_nr);
            tcb->rcv_nxt = 1;
            tcb->snd_nxt = 1;

            return tcp_send_ack(tcb);
        }
        else if (hdr->flags & TCP_FLAG_RST) {
            DEBUG("tcp_handle_synsent(): connection refused\n");
            tcp_cleanup(tcb);
            return 0;
        }
    }

    DEBUG("tcp_handle_synsent() unexpected flags\n");

    return 0;
}

static int tcp_handle_established(nano_ctx_t *ctx, tcp_tcb_t *tcb, tcp_hdr_t *hdr)
{
    uint32_t seq_nr = htonl(hdr->seq_nr) - tcb->irs;
    if (seq_nr != tcb->rcv_nxt) {
        DEBUG("tcp_handle_established(): unexpected seqnr: %"PRIu32"(%"PRIu32"- %"PRIu32") (exp: %"PRIu32")\n", seq_nr,
                htonl(hdr->seq_nr), tcb->irs, tcb->rcv_nxt);
        return tcp_reset(tcb);
    }
    else {
        if (hdr->flags & TCP_FLAG_FIN) {
            DEBUG("tcp_handle_established(): peer closing connection.\n");
            tcb->rcv_nxt++;
            tcb->state = CLOSE_WAIT;
            return tcp_send_finack(tcb);
        }
        else {
            size_t hdr_len = (hdr->data_offset >> 4) * 4;
            size_t frame_len = nano_ctx_bufleft(ctx, hdr);

            if (frame_len < hdr_len) {
                DEBUG("tcp: invalid data_offset\n");
                return tcp_reset(tcb);
            }

            uint8_t *payload = (uint8_t *)hdr + hdr_len;
            size_t payload_len = frame_len - hdr_len;
            if (payload_len) {
                DEBUG("tcp_handle_established(): got bytes %"PRIu32"-%"PRIu32" (%u)\n", seq_nr, seq_nr + payload_len, (unsigned)payload_len);
                tsrb_add(&tcb->rcv_buf, payload, payload_len);
                tcb->rcv_nxt += payload_len;
                return tcp_send_ack(tcb);
            }
        }
    }

    return 0;
}

static int tcp_handle_close_wait(nano_ctx_t *ctx, tcp_tcb_t *tcb, tcp_hdr_t *hdr)
{
    (void)ctx;
    (void)tcb;
    (void)hdr;
    uint32_t seq_nr = htonl(hdr->seq_nr) - tcb->irs;
    if (seq_nr != tcb->rcv_nxt) {
        DEBUG("unexpected seqnr: %"PRIu32"(%"PRIu32"- %"PRIu32") (exp: %"PRIu32")\n", seq_nr, 
                htonl(hdr->seq_nr), tcb->irs, tcb->rcv_nxt);
        return tcp_reset(tcb);
    } else if (! (hdr->flags & TCP_FLAG_ACK)) {
        DEBUG("tcp_handle_close_wait() final ACK expected, but not received\n");
        return tcp_reset(tcb);
    } else {
        DEBUG("tcp_handle_close_wait() closing connection\n");
        tcp_cleanup(tcb);
        return 0;
    }
}

static int tcp_handle_fin_wait(nano_ctx_t *ctx, tcp_tcb_t *tcb, tcp_hdr_t *hdr)
{
    /* this handler combines both handling of FIN_WAIT_1 and FIN_WAIT_2 */
    /* TODO: validate logic */
    (void)ctx;
    (void)tcb;
    uint32_t seq_nr = htonl(hdr->seq_nr) - tcb->irs;
    if (seq_nr != tcb->rcv_nxt) {
        DEBUG("unexpected seqnr: %"PRIu32"(%"PRIu32"- %"PRIu32") (exp: %"PRIu32")\n", seq_nr,
                htonl(hdr->seq_nr), tcb->irs, tcb->rcv_nxt);
        return tcp_reset(tcb);
    }
    else if ((tcb->state == FIN_WAIT_1) && (! (hdr->flags & TCP_FLAG_ACK))) {
        DEBUG("tcp_handle_fin_wait() ACK for FIN expected, but not received\n");
        return tcp_reset(tcb);
    }
    else {
        if (hdr->flags & TCP_FLAG_FIN) {
            DEBUG("tcp_handle_fin_wait(): sending final ACK\n");
            tcb->rcv_nxt++;
            tcp_send_ack(tcb);
            tcp_cleanup(tcb);
        }
        else {
            if (tcb->state == FIN_WAIT_1) {
                tcb->state = FIN_WAIT_2;
            }
            else {
                DEBUG("tcp_handle_fin_wait(): expected FIN\n");
                return tcp_reset(tcb);
            }
        }
        return 0;
    }
}

static const tcp_state_handler_t tcp_state_handlers[UNKNOWN] = {
    [SYN_SENT] = tcp_handle_synsent,
    [ESTABLISHED] = tcp_handle_established,
    [CLOSE_WAIT] = tcp_handle_close_wait,
    [FIN_WAIT_1] = tcp_handle_fin_wait,
    [FIN_WAIT_2] = tcp_handle_fin_wait,
};

static int tcp_handler_dispatch(nano_ctx_t *ctx, tcp_tcb_t *tcb, tcp_hdr_t *hdr)
{
    /* TODO: validate checksum */

    /* update remote window size */
    tcb->snd_wnd = ntohs(hdr->window_size);

    tcp_state_handler_t handler = tcp_state_handlers[tcb->state];
    if (handler) {
        return handler(ctx, tcb, hdr);
    }
    else {
        DEBUG("unhandled tcp state\n");
        return tcp_reset(tcb);
    }
}

static int _find_tcp_helper(clist_node_t *tcp_tcb, void *arg)
{
    nano_ctx_t *ctx = arg;
    tcp_tcb_t *tcb = (void *)tcp_tcb;
    return (ctx->src_addr.ipv4 == tcb->peer.ipv4) &&
           (ctx->src_port == tcb->dst_port) &&
           (ctx->dst_port == tcb->src_port);
}

static tcp_tcb_t *_find_tcp(nano_ctx_t *ctx)
{
    return (void *)clist_foreach(&_tcp_list, _find_tcp_helper, ctx);
}

int tcp_handle(nano_ctx_t *ctx, size_t offset)
{
    /* TODO: unify with udp_handle() */

    tcp_hdr_t *hdr = (tcp_hdr_t*) (ctx->buf+offset);

    if ((ctx->len-offset) < sizeof(tcp_hdr_t)) {
        DEBUG("tcp: truncated packet received.\n");
        return -1;
    }

    ctx->src_port = ntohs(hdr->src_port);

    uint16_t dst_port = ntohs(hdr->dst_port);
    ctx->dst_port = dst_port;

#if ENABLE_DEBUG
    if (is_ipv4_hdr(ctx->l3_hdr_start)) {
        DEBUG("tcp: received packet from 0x%08x, src_port %u, dst_port %u\n",
                (unsigned int) ctx->src_addr.ipv4, ctx->src_port, ctx->dst_port);
    } else {
        DEBUG("tcp: received tcpv6 packet src_port %u, dst_port %u\n",
                ctx->src_port, ctx->dst_port);
    }
#endif

    tcp_tcb_t *tcb = _find_tcp(ctx);
    if (tcb) {
        return tcp_handler_dispatch(ctx, tcb, hdr);
    }
    else {
        DEBUG("tcp: unreachable port %u\n", dst_port);
        return tcp_reply_connection_refused(ctx, offset);
    }

    return 0;
}
