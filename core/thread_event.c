#include "irq.h"
#include "thread.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

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
        DEBUG("%s: looping\n", __func__);
        irq_disable();
        /* if there's any event ... */
        if ((thread_event =
                    (thread_event_t *)clist_lpop(&me->thread_events))) {
            /* check if it is in our array of wanted events */
            if (thread_event >= &wanted_events[0] &&
                (thread_event < &wanted_events[wanted_events_numof])) {
                /* it is. push the deferred events to the front of our
                 * thread_events */
                clist_concat(&deferred, &me->thread_events);
                me->thread_events.next = deferred.next;
                irq_enable();
                break;
            }
            else {
                /* unwanted event, add to deferred list */
                clist_rpush(&deferred, &thread_event->next);
            }
        }
        sched_set_status(me, STATUS_EVENT_BLOCKED);
        thread_yield_higher();
        irq_enable();
    } while (1);

    return thread_event;
}

thread_event_t *thread_event_post_and_get_this(thread_t *thread,
                                               thread_event_t *to_post,
                                               thread_event_t *this)
{
    thread_t *me = thread_get_active();
    thread_event_t *thread_event;

    clist_node_t deferred = { .next = NULL };

    irq_disable();
    DEBUG("%s: posting\n", __func__);
    thread_event_post_noirqs(thread, to_post);

    do {
        DEBUG("%s: looping\n", __func__);
        irq_disable();
        /* if there's any event ... */
        while ((thread_event =
                    (thread_event_t *)clist_lpop(&me->thread_events))) {
            /* check if it is in our array of wanted events */
            if (thread_event == this) {
                DEBUG("%s: got my event\n", __func__);
                /* it is. push the deferred events to the front of our
                 * thread_events */
                clist_concat(&deferred, &me->thread_events);
                me->thread_events.next = deferred.next;
                irq_enable();
                goto out;
            }
            else {
                DEBUG("%s: deferring unwanted event\n", __func__);
                /* unwanted event, add to deferred list */
                clist_rpush(&deferred, &thread_event->next);
            }
        }
        sched_set_status(me, STATUS_EVENT_BLOCKED);
        thread_yield_higher();
        irq_enable();
    } while (1);
out:
    DEBUG("%s: done\n", __func__);

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
