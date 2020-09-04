/*
 * Copyright (C) 2020 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#include "ztimer.h"

typedef ztimer_clock_t ztimer_systick_t;

void ztimer_systick_init(ztimer_systick_t *ztimer);
