#ifndef SANDBOX_H
#define SANDBOX_H

#include <stdint.h>
#include <stdlib.h>

typedef struct {
    void *mem_start;
    size_t mem_len;
    //uint32_t pledges;
} sandbox_t;

#endif /* SANDBOX_H */
