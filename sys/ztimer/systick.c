/*
 * Copyright (C) 2018 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#include <stdio.h>

#include "cpu.h"
#include "ztimer/systick.h"

static volatile unsigned _ztimer_systick_base;
static volatile unsigned _ztimer_systick_ofl;

static int _check_overflow(void)
{
    if (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Pos) {
        _ztimer_systick_base += 1<<24;
        _ztimer_systick_ofl = 1;
        return 1;
    }

    return 0;
}

static void _ztimer_systick_set(ztimer_dev_t *ztimer, uint32_t val)
{
    ztimer_systick_t *ztimer_systick = (ztimer_systick_t*) ztimer;
}

static uint32_t _ztimer_systick_now(ztimer_dev_t *ztimer)
{
    ztimer_systick_t *ztimer_systick = (ztimer_systick_t*) ztimer;

    unsigned now;

    unsigned state = irq_disable();

    do {
        now = SysTick->VAL;
    } while (_check_overflow());

    now -= _ztimer_systick_base;

    irq_restore(state);

    return 0 - now;
}

static void _ztimer_systick_cancel(ztimer_dev_t *ztimer)
{
    ztimer_systick_t *ztimer_systick = (ztimer_systick_t*) ztimer;
}

void isr_systick(void)
{
    if (!_ztimer_systick_ofl) {
        _ztimer_systick_base += 1<<24;
    }
    else {
        _ztimer_systick_ofl = 0;
    }

    //ztimer_handler((ztimer_dev_t*) arg);
}

static const ztimer_ops_t _ztimer_systick_ops = {
    .set=_ztimer_systick_set,
    .now=_ztimer_systick_now,
    .cancel=_ztimer_systick_cancel,
};

void ztimer_systick_init(ztimer_systick_t *ztimer)
{
    ztimer->ops = &_ztimer_systick_ops;

    SysTick->CTRL = 0;
    SysTick->VAL = 0;
    SysTick->LOAD = 0x00ffffff;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
                    SysTick_CTRL_TICKINT_Msk   |
                    SysTick_CTRL_ENABLE_Msk;
}
