/*
 * Copyright (C) 2020 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_circuitplayground_bluefruit
 * @{
 *
 * @file
 * @brief       Peripheral configuration for the Curcuit Playground Bluefruit
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "periph_cpu.h"
#include "cfg_rtt_default.h"
#include "cfg_timer_default.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Clock configuration
 *
 * @note    The radio will not work with the internal RC oscillator!
 *
 * @{
 */
#define CLOCK_HFCLK         (32U)           /* set to  0: internal RC oscillator
                                             *        32: 32MHz crystal */
#define CLOCK_LFCLK         (0)             /* set to  0: internal RC oscillator
                                             *         1: 32.768 kHz crystal
                                             *         2: derived from HFCLK */
/**
 * @name    UART configuration
 *
 * This configures NRF_UARTE0 to the RX/TX pins as marked on the board, left of
 * the battery connector.
 * Enable by setting UART_NUMOF.
 *
 * @{
 */

static const uart_conf_t uart_config[] = {
    {
        .dev        = NRF_UARTE0,
        .rx_pin     = GPIO_PIN(0, 30),
        .tx_pin     = GPIO_PIN(0, 14),
#ifdef MODULE_PERIPH_UART_HW_FC
        .rts_pin    = GPIO_UNDEF,
        .cts_pin    = GPIO_UNDEF,
#endif
        .irqn       = UARTE0_UART0_IRQn,
    },
};

#define UART_0_ISR          (isr_uart0)

//#define UART_NUMOF          ARRAY_SIZE(uart_config)
#define UART_NUMOF          0
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
