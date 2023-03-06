#ifndef THREAD_EVENTS_H
#define THREAD_EVENTS_H

#include "clist.h"
#include "thread.h"

typedef struct thread_event {
    clist_node_t next;
} thread_event_t;

void thread_event_post(thread_t *thread, thread_event_t *thread_event);
void thread_event_post_noirqs(thread_t *thread, thread_event_t *thread_event);

thread_event_t *thread_event_get(void);
thread_event_t *thread_event_get_oneof(thread_event_t *events, size_t num);
static inline thread_event_t *thread_event_get_this(thread_event_t *event) {
    return thread_event_get_oneof(event, 1);
}

thread_event_t *thread_event_tryget(void);
thread_event_t *thread_event_tryget_this(thread_event_t *events);

#endif /* THREAD_EVENTS_H */
