/*
 * Copyright (C) 2018 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    sys_ztimer_rtc  ztimer periph/rtc backend
 * @ingroup     sys_ztimer
 * @brief       ztimer periph/rtc backend
 *
 * This ztimer module implements a ztimer virtual timer on top of periph/rtc.
 *
 * @{
 *
 * @file
 * @brief       ztimer rtc/timer backend API
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef ZTIMER_RTC_H
#define ZTIMER_RTC_H

#include "ztimer.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef ztimer_dev_t ztimer_rtc_t;

void ztimer_rtc_init(ztimer_rtc_t *ztimer);

#endif /* ZTIMER_RTC_H */
/** @} */
