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

thread_event_t *thread_event_get_oneof(thread_event_t *wanted_events,
                                       size_t wanted_events_numof)
{
    thread_t *me = thread_get_active();
    thread_event_t *thread_event;

    clist_node_t deferred = { .next = NULL };

    do {
        unsigned state = irq_disable();
        /* if there's any event ... */
        if ((thread_event = (thread_event_t *)clist_lpop(&me->thread_events))) {
            /* check if it is in our array of wanted events */
            if (thread_event >= &wanted_events[0] &&
                (thread_event < &wanted_events[wanted_events_numof])) {
                /* it is. push the deferred events to the front of our
                 * thread_events */
                clist_concat(&deferred, &me->thread_events);
                me->thread_events.next = deferred.next;
                irq_restore(state);
                break;
            }
            else {
                /* unwanted event, add to deferred list */
                clist_rpush(&deferred, &thread_event->next);
            }
        }
        sched_set_status(me, STATUS_EVENT_BLOCKED);
        thread_yield_higher();
        irq_restore(state);
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

thread_event_t *thread_event_tryget_this(thread_event_t *wanted_event)
{
    thread_event_t *thread_event;
    thread_t *me = thread_get_active();

    unsigned state = irq_disable();

    if ((thread_event_t *)clist_lpeek(&me->thread_events) == wanted_event) {
        thread_event = (thread_event_t *)clist_lpop(&me->thread_events);
    }
    else {
        thread_event = NULL;
    }

    irq_restore(state);

    return thread_event;
}

void thread_event_post_noirqs(thread_t *thread, thread_event_t *thread_event)
{
    clist_rpush(&thread->thread_events, &thread_event->next);

    if (thread->status == STATUS_EVENT_BLOCKED) {
        sched_set_status(thread, STATUS_PENDING);
        thread_yield_higher();
    }
}

void thread_event_post(thread_t *thread, thread_event_t *thread_event)
{
    unsigned state = irq_disable();

    thread_event_post_noirqs(thread, thread_event);
    irq_restore(state);
}
