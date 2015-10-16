#include <stdio.h>

int putc(int c, FILE *stream)
{
    unsigned char _c = (unsigned char) c;
    ssize_t res = stream->out(stream, (char *)&_c, 1);
    if (res <= 0) {
        return EOF;
    }
    else {
        return c;
    }
}
