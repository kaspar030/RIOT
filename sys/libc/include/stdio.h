#ifndef STDIO_H
#define STDIO_H

#include <stdarg.h>
#include <sys/types.h>

typedef struct _FILE FILE;

struct _FILE {
    ssize_t(*out)(FILE *restrict f, const char*, size_t n);
};

int vfprintf(FILE *restrict f, const char *restrict fmt, va_list ap);
int printf(const char *restrict fmt, ...);

int puts(const char *s);

#endif /* STDIO_H */
