#ifndef STDIO_H
#define STDIO_H

#include <stdarg.h>
#include <sys/types.h>

#undef EOF
#define EOF (-1)

typedef struct _FILE FILE;

struct _FILE {
    ssize_t(*out)(FILE *restrict f, const char*, size_t n);
    ssize_t(*in)(FILE *restrict f, char*, size_t n);
};

int vfprintf(FILE *restrict f, const char *restrict fmt, va_list ap);
int printf(const char *restrict fmt, ...);

int puts(const char *s);
int putc(int c, FILE *stream);
int putchar(int c);

int getc(FILE *stream);
int getchar(void);

size_t fwrite(const void *ptr, size_t size, size_t nmemb,
        FILE *stream);

extern FILE *const stdin;
extern FILE *const stdout;
extern FILE *const stderr;

#endif /* STDIO_H */
