#ifndef THREAD_EVENTS_H
#define THREAD_EVENTS_H

#include "clist.h"
#include "thread.h"

typedef struct thread_event {
    clist_node_t next;
} thread_event_t;

void thread_event_post(thread_t *thread, thread_event_t *thread_event);

thread_event_t *thread_event_get(void);
thread_event_t *thread_event_tryget(void);

#endif /* THREAD_EVENTS_H */
