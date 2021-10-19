#ifndef INTTYPES_H
#define INTTYPES_H

#include <stdint.h>

#define PRIu8 "u"
#define PRIi8 "i"
#define PRIx8 "x"

#define PRIu16 "u"
#define PRIi16 "i"
#define PRId16 "i"
#define PRIx16 "x"

#define PRIu32 "lu"
#define PRIi32 "li"
#define PRId32 PRIi32
#define PRIx32 "lx"

#define PRIu64 "llu"

typedef uint32_t uint_farptr_t;

#endif /* INTTYPES_H */
