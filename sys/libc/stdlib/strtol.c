#include <string.h>

#include "fmt.h"

long int strtol(const char *nptr, char **endptr, int base)
{
    (void)base;

    unsigned neg = 0;

    switch (*nptr) {
        case '-':neg = 1;
                 /* fall through */
        case '+':nptr++;
    }

    size_t n = *endptr - nptr;
    uint32_t res = scn_u32_dec(nptr, n);

    return neg ? res : -res;
}
