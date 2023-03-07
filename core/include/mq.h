#ifndef MQ_H
#define MQ_H

#include "clist.h"
#include "msg_content.h"

typedef struct {
    clist_node_t msgs_waiting;
    clist_node_t send_slots;
} mq_t;

typedef struct {
    msg_content_t content;
} mq_msg_t;

void mq_rx(mq_t *mq, mq_msg_t *msg);
void mq_tx(mq_t *mq, mq_msg_t *msg);

#endif /* MQ_H */
