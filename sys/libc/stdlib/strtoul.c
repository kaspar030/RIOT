#include <string.h>

#include "fmt.h"

unsigned long int strtoul(const char *nptr, char **endptr, int base)
{
    (void)base;
    return scn_u32_dec(nptr, (*endptr-nptr));
}
