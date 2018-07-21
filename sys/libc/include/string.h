#ifndef STRING_H
#define STRING_H

#include "sys/types.h"

int memcmp(const void *s1, const void *s2, size_t n);
void *memcpy(void *dest, const void *src, size_t n);
void *memmove(void *dest, const void *src, size_t n);
void *memset(void *ptr, int c, size_t n);

static inline size_t strlen(const char *s)
{
    size_t n = 0;
    while (*s++) n++;
    return n;
}

static inline size_t strnlen(const char *s, size_t maxlen)
{
    size_t n = 0;
    while (maxlen-- && *s++) n++;
    return n;
}

int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t n);

#endif /* STRING_H */
