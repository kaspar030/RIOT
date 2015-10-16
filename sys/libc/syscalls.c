#include <errno.h>
#include <unistd.h>

#include "uart_stdio.h"

ssize_t read(int fd, void *buf, size_t n)
{
    switch (fd) {
#ifdef MODULE_UART_STDIO
        case STDIN_FILENO:
            return uart_stdio_read(buf, n);
#endif
        default:
            return -EBADF;
    }
}

ssize_t write(int fd, const void *buf, size_t n)
{
    switch (fd) {
#ifdef MODULE_UART_STDIO
        case STDOUT_FILENO:
            return uart_stdio_write(buf, n);
#endif
        default:
            return -EBADF;
    }
}
