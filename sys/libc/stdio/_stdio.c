#include <stdio.h>
#include <unistd.h>

typedef struct {
    FILE super;
    int fd;
} _IO_FILE;

ssize_t fdwrite(FILE *restrict f, const char *ptr, size_t n)
{
    return write(((_IO_FILE*)f)->fd, ptr, n);
}

_IO_FILE _stdout = { .super.out=fdwrite, .fd=0 };
FILE *stdout = (FILE*)&_stdout;
