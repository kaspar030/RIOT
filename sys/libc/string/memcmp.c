#include <stddef.h>

int memcmp(const void *s1, const void *s2, size_t n)
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
