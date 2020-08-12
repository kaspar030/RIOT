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
 * @brief       Board initialization for the Adafruit Circuit Playground Bluefruit
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include "cpu.h"
#include "board.h"

#include "periph/gpio.h"

void board_init(void)
{
    /* initialize the board's RGB LED */
    gpio_init(LED0_PIN, GPIO_OUT);
    gpio_set(LED0_PIN);

    /* gpio_init(LED1_PIN, GPIO_OUT); */
    /* gpio_set(LED1_PIN); */
    /* gpio_init(LED2_PIN, GPIO_OUT); */
    /* gpio_set(LED2_PIN); */

    /* initialize the CPU */
    cpu_init();
}
