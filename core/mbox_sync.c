#include "thread.h"
#include "mbox_sync.h"
#include "irq.h"

#if MODULE_CORE_THREAD_FLAGS
#include "thread_flags.h"
#endif

static int _mbox_sync_try_recv(mbox_sync_t *mbox, msg_t *msg)
{
    int res = 0;
    thread_t *sender;

    if ((sender = (thread_t *)list_remove_head(&mbox->senders))) {
        *msg = *(msg_t *)sender->wait_data;
        sched_set_status(sender, STATUS_PENDING);
        res = 1;
    }
    return res;
}

int mbox_sync_try_recv(mbox_sync_t *mbox, msg_t *msg)
{
    unsigned state = irq_disable();
    int res = _mbox_sync_try_recv(mbox, msg);

    irq_restore(state);
    return res;
}

static void _wait_msg(mbox_sync_t *mbox, msg_t *msg, unsigned irq_state)
{
    thread_t *me = thread_get_active();
    mbox->waiter = me;
    me->wait_data = msg;
    sched_set_status(me, STATUS_MBOX_BLOCKED);
    thread_yield_higher();
    irq_restore(irq_state);
    // sender copied message
}

void mbox_sync_recv(mbox_sync_t *mbox, msg_t *msg)
{
    unsigned state = irq_disable();

    if (_mbox_sync_try_recv(mbox, msg) == 1) {
        irq_restore(state);
    }
    else {
        _wait_msg(mbox, msg, state);
    }
}

int _mbox_sync_try_send(mbox_sync_t *mbox, msg_t *msg) {
    thread_t *waiter;
    if ((waiter = mbox->waiter)) {
        *((msg_t *)waiter->wait_data) = *msg;
        mbox->waiter = NULL;
        sched_set_status(waiter, STATUS_PENDING);
        thread_yield_higher();
        return 1;
    } else {
        return 0;
    }
}

int mbox_sync_try_send(mbox_sync_t *mbox, msg_t *msg)
{
    msg->sender_pid = thread_getpid();
    int res = 0;
    unsigned state = irq_disable();

    if (_mbox_sync_try_send(mbox, msg) == 1) {
        res = 1;
    }
    irq_restore(state);
    return res;
}

void mbox_sync_send(mbox_sync_t *mbox, msg_t *msg)
{
    msg->sender_pid = thread_getpid();
    unsigned state = irq_disable();
    if (_mbox_sync_try_send(mbox, msg)) {
        irq_restore(state);
    } else {
        thread_t *me = thread_get_active();
        thread_add_to_list(&mbox->senders, me);
        me->wait_data = (msg_t *)msg;
        sched_set_status(me, STATUS_MBOX_BLOCKED);

#if MODULE_CORE_THREAD_FLAGS
        target->flags |= THREAD_FLAG_MSG_WAITING;
        thread_flags_wake(target);
#endif

        thread_yield_higher();
        irq_restore(state);
        // receiver copied message
    }
}
