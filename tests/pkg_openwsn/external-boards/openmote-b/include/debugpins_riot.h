/*
 * Copyright (C) 2017 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_openwsn
 * @{
 *
 * @file
 * @brief       Provides an adaption of OpenWSN debug pin handling
 *              to RIOTs handling of GPIOs.
 *
 * @author      Michael Frey <michael.frey@msasafety.com>
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 *
 * @}
 */

#ifndef __DEBUGPINS_RIOT_H
#define __DEBUGPINS_RIOT_H

#include "periph/gpio.h"

/**
 * @brief   struct holding configuration of OpenWSN debug pins
 */
typedef struct debugpins_config {
    gpio_t frame;       /**< debug pin for frames */
    gpio_t slot;        /**< debug pin for slots  */
    gpio_t fsm;         /**< debug pin for fsm */
    gpio_t task;        /**< debug pin for tasks */
    gpio_t isr;         /**< debug pin for interrupt service routines */
    gpio_t radio;       /**< debug pin for the radio */
} debugpins_config_t;


/*
 * Set default configuration parameters for debugpins. Default settings match
 * OpenWSN openmote-b configuration, otherwise they are undefined.
 */
#ifndef OPENWSN_DEBUGPIN_FRAME
#define OPENWSN_DEBUGPIN_FRAME          (GPIO_PIN(PORT_A, 7))
#endif
#ifndef OPENWSN_DEBUGPIN_SLOT
#define OPENWSN_DEBUGPIN_SLOT           (GPIO_PIN(PORT_B, 3))
#endif
#ifndef OPENWSN_DEBUGPIN_FSM
#define OPENWSN_DEBUGPIN_FSM            (GPIO_PIN(PORT_B, 2))
#endif
#ifndef OPENWSN_DEBUGPIN_TASK
#define OPENWSN_DEBUGPIN_TASK           (GPIO_PIN(PORT_B, 1))
#endif
#ifndef OPENWSN_DEBUGPIN_ISR
#define OPENWSN_DEBUGPIN_ISR            (GPIO_PIN(PORT_C, 3))
#endif
#ifndef OPENWSN_DEBUGPIN_RADIO
#define OPENWSN_DEBUGPIN_RADIO          (GPIO_PIN(PORT_B, 0))
#endif

#define OPENWSN_DEBUGPINS_DEFAULT    { .frame = OPENWSN_DEBUGPIN_FRAME, \
                                       .slot = OPENWSN_DEBUGPIN_SLOT, \
                                       .fsm = OPENWSN_DEBUGPIN_FSM,  \
                                       .task = OPENWSN_DEBUGPIN_TASK, \
                                       .isr = OPENWSN_DEBUGPIN_ISR,  \
                                       .radio = OPENWSN_DEBUGPIN_RADIO }


static const debugpins_config_t openwsn_debugpins_params[] =
{
#ifdef OPENWSN_DEBUGPINS_BOARD
    OPENWSN_DEBUGPINS_BOARD,
#else
    OPENWSN_DEBUGPINS_DEFAULT,
#endif
};

void openwsn_debugpins_init(const debugpins_config_t *user_config);

#endif /* __DEBUGPINS_RIOT_H */
