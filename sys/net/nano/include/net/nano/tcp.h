#ifndef NANO_TCP_H
#define NANO_TCP_H

#include <stdint.h>
#include <stddef.h>

#include "mutex.h"
#include "clist.h"
#include "event.h"
#include "net/nano/ctx.h"
#include "net/nano/ipv4.h"
#include "net/nano/ipv6.h"
#include "tsrb.h"

typedef enum {
    CLOSED,
    LISTEN,
    SYN_SENT,
    SYN_RECEIVED,
    ESTABLISHED,
    FIN_WAIT_1,
    FIN_WAIT_2,
    CLOSE_WAIT,
    CLOSING,
    LAST_ACK,
    TIME_WAIT,
    UNKNOWN,
} tcp_state_t;

enum {
    TCP_SND = 1,
    TCP_SND_REQ = 2,
};

typedef struct __attribute__((packed)) {
    uint16_t src_port;
    uint16_t dst_port;
    uint32_t seq_nr;
    uint32_t ack_nr;
    uint8_t data_offset; /* (NS not used) */
    uint8_t flags;
    uint16_t window_size;
    uint16_t checksum;
    uint16_t urgent_ptr;
    uint8_t options[];
} tcp_hdr_t;

typedef struct {
    clist_node_t next;
    tcp_state_t state;
    uint16_t flags;
    uint16_t src_port;
    uint16_t dst_port;
    union {
        uint32_t ipv4;
        uint8_t ipv6[8];
    } peer;

    uint32_t irs;
    uint32_t rcv_nxt;

    /* urgent pointer not supported (yet) */
    /*uint32_t rcv_up;*/

    uint32_t iss;
    uint32_t snd_nxt;
    uint32_t snd_una;
    uint32_t snd_wnd;

    tsrb_t rcv_buf;
    event_t event;

    const iolist_t *snd_iolist;
    uint32_t snd_pos;
    mutex_t snd_mutex;
} tcp_tcb_t;

/* typedef struct nano_tcp_bind { */
/*     uint16_t port; */
/*     nano_tcp_handler_t handler; */
/* } nano_tcp_bind_t; */
/* typedef int (*nano_tcp_handler_t) (nano_ctx_t *ctx, size_t offset); */

/* extern nano_tcp_bind_t nano_tcp_binds[]; */

int tcp_init(tcp_tcb_t *tcb, uint8_t *buf, size_t buf_len);
int tcp_connect(tcp_tcb_t *tcb, uint32_t dst_ip, uint16_t dst_port, uint16_t src_port);
int tcp_write(tcp_tcb_t *tcb, const iolist_t *iolist);
int tcp_close(tcp_tcb_t *tcb);
int tcp_handle(nano_ctx_t *ctx, size_t offset);

/* int tcp6_send(const iolist_t *iolist, uint8_t* dst_ip, uint16_t dst_port, uint16_t src_port, nano_dev_t *dev); */
/* int tcp_reply(nano_ctx_t *ctx); */
#endif /* NANO_TCP_H */
