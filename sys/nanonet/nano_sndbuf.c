#include <stdint.h>

#include "nano_config.h"
#include "nano_sndbuf.h"

#define ENABLE_DEBUG NANONET_ENABLE_DEBUG
#include "debug.h"

uint8_t *nano_sndbuf_alloc(nano_sndbuf_t *sndbuf, size_t size)
{
    if (!sndbuf->count) {
//        DEBUG("nano_sndbuf_alloc(): no free iovec entry.\n");
        return NULL;
    }

    unsigned bytes_left = sndbuf->vec[0].iov_len;
    if (bytes_left < size) {
        return NULL;
    }

    sndbuf->used += size;
    sndbuf->vec[0].iov_len -= size;
    sndbuf->vec[sndbuf->count].iov_len = size;
    sndbuf->vec[sndbuf->count].iov_base = (void*)((unsigned)sndbuf->vec[0].iov_base + sndbuf->vec[0].iov_len);

    return sndbuf->vec[sndbuf->count--].iov_base;
}
