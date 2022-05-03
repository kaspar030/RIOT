#include <stddef.h>

void __attribute__((used)) *memset(void *ptr, int c, size_t n)
{
    char *_ptr = ptr;
    while (n--) {
        *_ptr++ = (char)c;
    }
    return ptr;
}
