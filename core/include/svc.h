#ifndef SVC_H
#define SVC_H

#include <string.h>

#include "clist.h"
#include "irq.h"
#include "thread.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

typedef struct {
    clist_node_t providers;
    clist_node_t request_queue_list;
} svc_t;

typedef struct {
    clist_node_t list_entry;
    thread_t *owner;
} svc_provider_t;

typedef struct {
    clist_node_t event;
    thread_t *sender;
    char *request;
    size_t request_size;
    char *reply;
    size_t reply_size;

} svc_request_t;

void svc_req(svc_t *svc, char *request, size_t request_size, char *reply,
             size_t reply_size)
{
    svc_request_t req = {
        .sender = thread_get_active(),
        .request = request,
        .request_size = request_size,
        .reply = reply,
        .reply_size = reply_size
    };

    unsigned state = irq_disable();

    DEBUG("%s\n", __func__);
    clist_rpush(&svc->request_queue_list, &req.event);
    svc_provider_t *provider;

    if ((provider = (svc_provider_t *)clist_lpop(&svc->providers))) {
        DEBUG("%s: posting to owner\n", __func__);
        thread_event_post_and_get_this(provider->owner,
                                 (thread_event_t *)&provider->list_entry, (thread_event_t *)&req.event);
    } else {
        DEBUG("%s: enable irq\n", __func__);
        irq_restore(state);
        DEBUG("%s: waiting for reply\n", __func__);
        thread_event_get_this((thread_event_t *)&req.event);
    }
    DEBUG("%s: done\n", __func__);
}

void svc_provide(svc_t *svc, svc_provider_t *provider)
{
    unsigned state = irq_disable();
    DEBUG("%s\n", __func__);
    //provider->owner = thread_get_active();
    clist_rpush(&svc->providers, &provider->list_entry);
    irq_restore(state);
}

svc_request_t *svc_request_get(svc_t *svc)
{
    DEBUG("%s\n", __func__);
    unsigned state = irq_disable();
    svc_request_t *request = (svc_request_t *)clist_lpop(
        &svc->request_queue_list);

    irq_restore(state);
    return request;
}

void svc_reply(svc_request_t *request, void *reply, size_t reply_size)
{
    if (request->reply) {
        DEBUG("%s: copying reply\n", __func__);
        if (request->reply_size < reply_size) {
            reply_size = request->reply_size;
        }
        memcpy(request->reply, reply, reply_size);
    }
    DEBUG("%s: posting reply\n", __func__);
    thread_event_post(request->sender, (thread_event_t *)&request->event);
}

#endif /* SVC_H */
