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

int printf(const char *restrict fmt, ...);
int sprintf(char *str, const char *format, ...);
int snprintf(char *str, size_t size, const char *format, ...);
int vfprintf(FILE *restrict f, const char *restrict fmt, va_list ap);
int vsprintf(char *str, const char *format, va_list ap);
int vsnprintf(char *str, size_t size, const char *format, va_list ap);


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

static inline void fflush(FILE *stream) {}

#endif /* STDIO_H */
