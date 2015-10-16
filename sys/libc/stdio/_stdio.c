#include <stdio.h>
#include <unistd.h>

typedef struct {
    FILE super;
    int fd;
} _FD_FILE;

static ssize_t _fdwrite(FILE *restrict f, const char *ptr, size_t n)
{
    return write(((_FD_FILE*)f)->fd, ptr, n);
}

static ssize_t _fdread(FILE *restrict f, char *ptr, size_t n)
{
    return read(((_FD_FILE*)f)->fd, ptr, n);
}


static const _FD_FILE _stdin = { .super.out=_fdwrite, .super.in=_fdread, .fd=STDIN_FILENO };
static const _FD_FILE _stdout = { .super.out=_fdwrite, .super.in=_fdread, .fd=STDOUT_FILENO };

FILE *const stdin = (FILE*)&_stdin;
FILE *const stdout = (FILE*)&_stdout;
