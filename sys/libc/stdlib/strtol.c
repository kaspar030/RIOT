#include <string.h>

#include "fmt.h"

long int strtol(const char *nptr, char **endptr, int base)
{
    uint8_t buf[16] = { 0 };
    memcpy(buf, nptr, endptr);
    return scn_u32_dec(buf);
}
