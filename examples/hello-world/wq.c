#include <stdio.h>
#include "wq.h"
#include "thread_flags.h"
#include "irq.h"

void wq_wake_all(wq_t *wq) {
    int state = irq_disable();
    list_node_t *waiters = &wq->waiters;
    while ((waiters = waiters->next)) {
        wq_waiter_t *waiter = (wq_waiter_t*) waiters;
        thread_flags_set(waiter->thread, 0x1);
    }
    irq_restore(state);
}

void wq_attach(wq_t *wq, wq_waiter_t *waiter) {
    waiter->thread = thread_get_active();

    int state = irq_disable();

    list_add(&wq->waiters, &waiter->next);

    irq_restore(state);
}
