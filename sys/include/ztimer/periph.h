/*
 * Copyright (C) 2018 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    sys_ztimer_periph  ztimer periph/timer backend
 * @ingroup     sys_ztimer
 * @brief       ztimer periph/timer backend
 *
 * This ztimer module implements a ztimer virtual timer on top of periph/timer.
 *
 * @{
 *
 * @file
 * @brief       ztimer periph/timer backend API
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef ZTIMER_PERIPH_H
#define ZTIMER_PERIPH_H

#include "ztimer.h"
#include "periph/timer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ztimer periph context structure
 */
typedef struct {
    ztimer_dev_t super;     /*<< super class            */
    tim_t dev;              /*<< periph timer device    */
    uint32_t adjust;        /*<< optional trim value    */
} ztimer_periph_t;

/**
 * @brief   ztimer periph initialization
 *
 * Initializes the given periph timer and sets up the ztimer device.
 *
 * @param[in]   ztimer  ztimer periph device to initialize
 * @param[in]   dev     periph timer to use
 * @param[in]   freq    frequency to configure
 */
void ztimer_periph_init(ztimer_periph_t *ztimer, tim_t dev, unsigned long freq);

#ifdef __cplusplus
}
#endif

#endif /* ZTIMER_PERIPH_H */
/** @} */
