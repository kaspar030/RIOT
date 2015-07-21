#ifndef NANO_SNDBUF_H
#define NANO_SNDBUF_H

#include <stdint.h>
#include <stddef.h>

typedef struct nano_sndbuf {
    uint8_t *buf;
    size_t size;
    size_t used;
} nano_sndbuf_t;

static inline uint8_t *nano_sndbuf_alloc(nano_sndbuf_t *sndbuf, size_t size)
{
    uint8_t *res = sndbuf->buf + sndbuf->size - sndbuf->used - size;
    if (res < sndbuf->buf) {
        return NULL;
    }
    else {
        sndbuf->used += size;
        return res;
    }
}

#endif /* NANO_SNDBUF_H */
