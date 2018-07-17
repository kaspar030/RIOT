/*
 * Copyright (C) 2018 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_grassberry-node
 * @{
 *
 * @file
 * @brief       Board specifics for the Grassberry High Sensor Node
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include "cpu.h"
#include "board.h"

/**
 * @brief           initialize the board
 */
void board_init(void)
{
    cpu_init();
    gpio_init(LED0_PIN, GPIO_OUT);
    gpio_clear(LED0_PIN);
}
