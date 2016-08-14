#ifndef NANO_SNDBUF_H
#define NANO_SNDBUF_H

#include <stdint.h>
#include <stddef.h>

#include <sys/uio.h>

#define NANONET_SNDBUF_VECSIZE (8U)

typedef struct nano_sndbuf {
    struct iovec vec[NANONET_SNDBUF_VECSIZE];
    unsigned count;
    size_t used;
} nano_sndbuf_t;

uint8_t *nano_sndbuf_alloc(nano_sndbuf_t *sndbuf, size_t size);

static inline unsigned nano_sndbuf_used(const nano_sndbuf_t *sndbuf)
{
    return sndbuf->used;
    /* return ((unsigned)sndbuf->vec[NANONET_SNDBUF_VECSIZE-1].iov_base */
    /*         + sndbuf->vec[NANONET_SNDBUF_VECSIZE-1].iov_len */
    /*         - (unsigned)sndbuf->vec[0].iov_base */
    /*         - (unsigned)sndbuf->vec[0].iov_len); */
}

static inline struct iovec *nano_sndbuf_getvec(const nano_sndbuf_t *sndbuf)
{
    return (struct iovec *)(&sndbuf->vec[sndbuf->count]);
}

static inline unsigned nano_sndbuf_getcount(const nano_sndbuf_t *sndbuf)
{
    return (NANONET_SNDBUF_VECSIZE - sndbuf->count);
}

#define NANO_SNDBUF_INIT(buf, size) { .vec={{.iov_len=size, .iov_base=buf} }, .count=(NANONET_SNDBUF_VECSIZE-1) }

#endif /* NANO_SNDBUF_H */
