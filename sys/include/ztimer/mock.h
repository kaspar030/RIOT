/*
 * Copyright (C) 2018 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    sys_ztimer_mock
 * @ingroup     sys_ztimer
 * @brief       ztimer mock timer backend
 *
 * This ztimer module implements a virtual timer that can be used for unittests.
 *
 * @{
 * @file
 * @brief       ztimer mock timer backend API
 * @author  Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef ZTIMER_MOCK_H
#define ZTIMER_MOCK_H

#include <stdint.h>
#include "ztimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   ztimer mock clock class
 */
typedef struct {
    ztimer_dev_t super;     /**< superclass instance */
    uint32_t mask;          /**< counter mask */
    uint32_t now;           /**< current counter value */
    uint32_t target;        /**< ticks left until alarm is hit */
    unsigned armed;         /**< flag for checking if a target has been set */
    struct ztimer_mock_calls {
        unsigned now;       /**< Number of calls to @ref ztimer_ops_t::now */
        unsigned set;       /**< Number of calls to @ref ztimer_ops_t::set */
        unsigned cancel;    /**< Number of calls to @ref ztimer_ops_t::cancel */
    } calls;                /**< counting number of calls to each operation */
} ztimer_mock_t;

/**
 * @brief   Advance the mock clock counter and update target
 *
 * This will call @ref ztimer_handler if the target was passed.
 *
 * @param[in]   self        instance to operate on
 * @param[in]   val         counter increment value
 */
void ztimer_mock_advance(ztimer_mock_t *self, uint32_t val);

/**
 * @brief   Set the mock clock counter value without updating alarm target
 *
 * This will not touch the alarm target.
 *
 * @param[in]   self        instance to operate on
 * @param[in]   target      new absolute counter value
 */
void ztimer_mock_jump(ztimer_mock_t *self, uint32_t target);

/**
 * @brief   Trigger the alarm handlers
 *
 * This is equivalent to a hardware timer interrupt
 *
 * @param[in]   self        instance to operate on
 */
void ztimer_mock_fire(ztimer_mock_t *self);

/**
 * @brief   Constructor
 *
 * @param[in]   self        instance to operate on
 * @param[in]   width       counter width, 1 <= width <= 32
 */
void ztimer_mock_init(ztimer_mock_t *self, unsigned width);

#ifdef __cplusplus
}
#endif

#endif /* ZTIMER_MOCK_H */
/** @} */
