#ifndef STRING_H
#define STRING_H

#include "sys/types.h"

int memcmp(const void *s1, const void *s2, size_t n);
void *memcpy(void *dest, const void *src, size_t n);
void *memmove(void *dest, const void *src, size_t n);
void *memset(void *ptr, int c, size_t n);
size_t strlen(const char *s);
size_t strnlen(const char *s, size_t maxlen);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t n);
char *strchr(const char *s, int c);
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, size_t n);
char *strtok_r(char *restrict s, const char *restrict sep, char **restrict p);
size_t strspn(const char *s, const char *c);
size_t strcspn(const char *s, const char *c);
char *strchrnul(const char *s, int c);

#endif /* STRING_H */
