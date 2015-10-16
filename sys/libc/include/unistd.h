#ifndef UNISTD_H
#define UNISTD_H

#include <stddef.h>

#define STDIN_FILENO (0)
#define STDOUT_FILENO (1)

typedef int ssize_t;

ssize_t read(int fildes, void *buf, size_t nbyte);
ssize_t write(int fd, const void *buf, size_t n);

#endif /* UNISTD_H */
