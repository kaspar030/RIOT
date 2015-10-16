#include "fmt.h"

int puts(const char *s)
{
    print_str(s);
    print_str("\n");
    return 0;
}
