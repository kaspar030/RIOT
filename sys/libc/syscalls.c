#include <errno.h>
#include <unistd.h>

#include "stdio_uart.h"

ssize_t read(int fd, void *buf, size_t n)
{
    switch (fd) {
#if defined(MODULE_STDIO_UART) || defined(MODULE_STDIO_RTT)
        case STDIN_FILENO:
            return stdio_read(buf, n);
#endif
        default:
            (void)fd;
            (void)buf;
            (void)n;
            return -EBADF;
    }
}

ssize_t write(int fd, const void *buf, size_t n)
{
    switch (fd) {
#if defined(MODULE_STDIO_UART) || defined(MODULE_STDIO_RTT)
        case STDOUT_FILENO:
            return stdio_write(buf, n);
#endif
        default:
            (void)fd;
            (void)buf;
            (void)n;
            return -EBADF;
    }
}
