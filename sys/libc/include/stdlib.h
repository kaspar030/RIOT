#ifndef STDLIB_H
#define STDLIB_H

#include <stddef.h>

long int strtol(const char *nptr, char **endptr, int base);
unsigned long int strtoul(const char *nptr, char **endptr, int base);
static inline atoi(const char *nptr) { return strtol(nptr, NULL, 10);}


#endif /* STDLIB_H */
