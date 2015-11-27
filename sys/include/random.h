/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */


/**
 * @defgroup    sys_random Random
 * @ingroup     sys
 * @brief       Random number generator
 * @{
 *
 * @file
 * @brief       Mersenne Twister - a very fast random number generator
 */

#ifndef RANDOM_H
#define RANDOM_H

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief initializes mt[N] with a seed
 *
 * @param s seed for the PRNG
 */
void random_init(uint32_t seed);

/**
 * @brief generates a random number on [0,0xffffffff]-interval
 * @return a random number on [0,0xffffffff]-interval
 */
uint32_t random_u32(void);

/**
 * @brief   generates a random number r with a <= r < b.
 *
 * @param[in] a minimum for random number
 * @param[in] b upper bound for random number
 *
 * @pre     a < b
 *
 * @return  a random number on [a,b)-interval
 */
static inline uint32_t random_u32_range(uint32_t a, uint32_t b)
{
    return (random_u32() % (b - a)) + a;
}

/**
 * @brief generates a random double number r (0.0 < r <= 1.0)
 *
 * This function outputs double precision floating point number.
 * The returned value has 32-bit precision.
 * In other words, this function makes one double precision floating point
 * number from one 32-bit unsigned integer.
 *
 * @return floating point number r (0.0 < r <= 1.0)
 */
inline static double random_double(void)
{
    return random_u32() * (1.0 / 4294967296.0);
}

#ifdef __cplusplus
}
#endif

#endif /* RANDOM_H */
/** @} */
