#include <stdio.h>

int getc(FILE *f)
{
    unsigned char c;
    ssize_t res = f->in(f, (char *)&c, 1);
    if (res == 1) {
        return (int)c;
    }
    else {
        return res;
    }
}

