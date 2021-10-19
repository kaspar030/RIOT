#ifndef STDLIB_H
#define STDLIB_H

#include <stddef.h>

long int strtol(const char *nptr, char **endptr, int base);
unsigned long int strtoul(const char *nptr, char **endptr, int base);
static inline int atoi(const char *nptr) { return strtol(nptr, NULL, 10);}
static inline long atol(const char *nptr) { return strtol(nptr, NULL, 10);}

typedef struct { int quot, rem; } div_t;
typedef struct { long quot, rem; } ldiv_t;
typedef struct { long long quot, rem; } lldiv_t;

div_t div (int, int);
ldiv_t ldiv (long, long);
lldiv_t lldiv (long long, long long);

#endif /* STDLIB_H */
