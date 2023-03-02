#include "irq.h"
#include "thread.h"

thread_event_t *thread_event_get(void)
{
    thread_t *me = thread_get_active();
    thread_event_t *thread_event;

    do {
        unsigned state = irq_disable();
        if ((thread_event = (thread_event_t *)clist_lpop(&me->thread_events))) {
            irq_restore(state);
            break;
        }
        else {
            sched_set_status(me, STATUS_EVENT_BLOCKED);
            thread_yield_higher();
            irq_restore(state);
        }
    } while (1);

    return thread_event;
}

thread_event_t *thread_event_tryget(void)
{
    thread_event_t *thread_event;
    thread_t *me = thread_get_active();

    unsigned state = irq_disable();
    thread_event = (thread_event_t *)clist_lpop(&me->thread_events);
    irq_restore(state);

    return thread_event;
}

void thread_event_post(thread_t *thread, thread_event_t *thread_event)
{
    unsigned state = irq_disable();

    clist_rpush(&thread->thread_events, &thread_event->next);

    if (thread->status == STATUS_EVENT_BLOCKED) {
        sched_set_status(thread, STATUS_PENDING);
        thread_yield_higher();
    }

    irq_restore(state);
}
