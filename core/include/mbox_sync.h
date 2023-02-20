#ifndef MBOX_SYNC_H
#define MBOX_SYNC_H

#include "list.h"
#include "msg.h"

typedef struct {
    list_node_t senders;
    thread_t *waiter;
} mbox_sync_t;

void mbox_sync_send(mbox_sync_t *mbox, const msg_t *msg);
int mbox_sync_try_send(mbox_sync_t *mbox, const msg_t *msg);
void mbox_sync_recv(mbox_sync_t *mbox, msg_t *msg);
int mbox_sync_try_recv(mbox_sync_t *mbox, msg_t *msg);

#endif /* MBOX_SYNC_H */
