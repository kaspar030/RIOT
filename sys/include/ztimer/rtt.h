/*
 * Copyright (C) 2018 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    sys_ztimer_rtt  ztimer periph/rtt backend
 * @ingroup     sys_ztimer
 * @brief       ztimer periph/rtt backend
 *
 * This ztimer module implements a ztimer virtual timer on top of periph/rtt.
 *
 * @{
 *
 * @file
 * @brief       ztimer periph/rtt backend API
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef ZTIMER_RTT_H
#define ZTIMER_RTT_H

#include "ztimer.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef ztimer_dev_t ztimer_rtt_t;

void ztimer_rtt_init(ztimer_rtt_t *ztimer);

#endif /* ZTIMER_RTT_H */
/** @} */
