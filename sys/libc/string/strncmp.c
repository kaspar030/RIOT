#include <stddef.h>

int strncmp(const char *s1, const char *s2, size_t n)
{
    char c1, c2;
    int diff = 0;
    while ( n-- && (c1 = *s1++) && (c2 = *s2++)) {
        if ((diff = (int)c2 - (int)c1)) {
            break;
        }
    }
    return diff;
}
