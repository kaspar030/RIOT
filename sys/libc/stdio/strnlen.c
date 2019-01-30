#include "sys/types.h"

size_t strnlen(const char *s, size_t maxlen)
{
    size_t n = 0;
    while (maxlen-- && *s++) n++;
    return n;
}
