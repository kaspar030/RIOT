#ifndef STRING_H
#define STRING_H

#include "sys/types.h"

static inline void *memcpy(void *dest, const void *src, size_t n)
{
    char *d = dest;
    const char *s = src;
    while(n--) *d++ = *s++;
    return dest;
}

static inline int memcmp(const void *s1, const void *s2, size_t n)
{
    const unsigned char *_s1 = s1;
    const unsigned char *_s2 = s2;
    int diff = 0;
    while(n--) {
        diff = (int)*_s1++ - (int)*_s2++;
        if (diff)
            break;
    }
    return  diff;
}

static inline void *memset(void *ptr, int c, size_t n)
{
    char *_ptr = ptr;
    while (n--) {
        *_ptr++ = (char)c;
    }
    return ptr;
}

static inline void *memmove(void *dest, const void *src, size_t n)
{
    return __builtin_memmove(dest, src, n);
}

static inline size_t strlen(const char *s)
{
    size_t n = 0;
    while (*s++) n++;
    return n;
}

int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t n);

#endif /* STRING_H */
