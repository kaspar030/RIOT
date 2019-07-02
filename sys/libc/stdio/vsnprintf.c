#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>

typedef struct {
    FILE super;
    void *ptr;
    size_t left;
} _MEM_FILE;

static ssize_t _sn_write(FILE *restrict f, const char *ptr, size_t n)
{
    _MEM_FILE *mem = (_MEM_FILE *)f;
    if (n > mem->left) {
        n = mem->left;
    }
    memcpy(mem->ptr, ptr, n);
    mem->ptr += n;
    mem->left -= n;
    return 1;
}

int vsnprintf(char *restrict s, size_t n, const char *restrict fmt, va_list ap)
{
    _MEM_FILE f = {
        .super.out = _sn_write,
        .ptr = s,
        .left = n,
    };

    if (n > INT_MAX) {
        errno = EOVERFLOW;
        return -1;
    }

    return vfprintf((FILE *)&f, fmt, ap);
}
