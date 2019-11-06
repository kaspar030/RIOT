/*
 * Copyright (C) 2019 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    sys_ztimer_convert
 * @ingroup     sys_ztimer
 * @brief       ztimer frequency conversion module
 *
 * This ztimer module allows converting a lower-level ztimer with a given
 * frequency to another frequency.
 *
 * It is configured by passing two parameters (dev, mul).
 * Given a lower ztimer frequency f_low and a desired upper frequency f_upper,
 * div and mul must be chosen such that
 *
 *     (f_upper * mul / div) == f_lower
 *
 * A div or mul value of 0 is treated as 1 (no multiplication or division by 0 is
 * done).
 *
 * On every ztimer_set(), the target offset is first multiplied by mul and
 * then divided by div, before passing it to the lower ztimer's ztimer_set().
 *
 * On every ztimer_now(), the value from the lower ztimer is first multiplied
 * by div and then divided by mul.
 *
 * Multiplication and division is done as uint32_t, thus every use of
 * ztimer_convert requires the usage of ztimer_extend to ensure that no
 * multiplication done by ztimer_convert overflows.
 *
 * Example:
 *
 * 1. if a ztimer_periph with 250khz is to be "sped up" to 1MHz, use div=4,
 *    mul=0, ztimer_extend() two missing bits
 *
 * 2. if a ztimer with 1024Hz is to be converted to 1000Hz, use div=125,
 *    mul=128, ztimer_extend() 7 missing bits
 *
 * @{
 * @file
 * @brief       ztimer frequency conversion module API
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef ZTIMER_CONVERT_H
#define ZTIMER_CONVERT_H

#include "ztimer.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    ztimer_dev_t super;
    ztimer_dev_t *parent;
    ztimer_t parent_entry;
    uint16_t mul;
    uint16_t div;
} ztimer_convert_t;

void ztimer_convert_init(ztimer_convert_t *ztimer_convert, ztimer_dev_t *parent,
                         unsigned div, unsigned mul);

#ifdef __cplusplus
}
#endif

#endif /* ZTIMER_CONVERT_H */
/** @} */
