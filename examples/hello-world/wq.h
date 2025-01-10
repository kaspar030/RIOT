#ifndef WQ_H
#define WQ_H

#include "list.h"
#include "thread.h"

typedef struct {
    list_node_t waiters;
} wq_t;

typedef struct {
    list_node_t next;
    thread_t *thread;
} wq_waiter_t;

void wq_wake_all(wq_t *wq);

void wq_attach(wq_t *wq, wq_waiter_t *waiter);

#endif /* WQ_H */
