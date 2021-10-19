/* From musl libc.  See LICENSE.musl for copyright information. */
#include <stdlib.h>

ldiv_t ldiv(long num, long den)
{
	return (ldiv_t){ num/den, num%den };
}
