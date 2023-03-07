
#include "clist.h"
#include "irq.h"
#include "mq.h"
#include "msg_content.h"
#include "thread.h"
#include "thread_events.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

typedef struct {
    clist_node_t event;
    thread_t *sender;
    msg_content_t *msg;
} mq_event_t;

void mq_rx(mq_t *mq, mq_msg_t *msg)
{
    unsigned state = irq_disable();
    mq_event_t *rx_event;

    if ((rx_event = (mq_event_t *)clist_lpop(&mq->msgs_waiting))) {
        DEBUG("%s: msg waiting\n", __func__);
        msg->content = *rx_event->msg;
        thread_event_post_noirqs(rx_event->sender,
                                 (thread_event_t *)&rx_event->event);
        irq_restore(state);
    }
    else {
        DEBUG("%s: no message waiting, blocking\n", __func__);
        mq_event_t rx_event = {
            .msg = &msg->content,
            .sender = thread_get_active()
        };
        clist_rpush(&mq->send_slots, &rx_event.event);
        irq_restore(state);
        thread_event_get_this((thread_event_t *)&rx_event.event);
    }
    DEBUG("%s: done\n", __func__);
}

void mq_tx(mq_t *mq, mq_msg_t *msg)
{
    unsigned state = irq_disable();
    mq_event_t *event = (mq_event_t *)clist_lpop(&mq->send_slots);

    if (event) {
        DEBUG("%s: sync send\n", __func__);
        thread_t *owner = event->sender;
        event->sender = thread_get_active();
        *event->msg = msg->content;
        thread_event_post_noirqs(owner, (thread_event_t *)&event->event);
        irq_restore(state);
    }
    else {
        mq_event_t event = {
            .msg = &msg->content,
            .sender = thread_get_active()
        };
        DEBUG("%s: going blocked\n", __func__);
        clist_rpush(&mq->msgs_waiting, &event.event);
        irq_restore(state);
        thread_event_get_this((thread_event_t *)&event.event);
    }
    DEBUG("%s: done\n", __func__);
}
