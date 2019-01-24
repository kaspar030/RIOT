/*
 * Copyright (C) 2019 Kaspar Schleiser <kaspar@schleiser.de>
 *               2019 Philipp-Alexander Blum <philipp-blum@jakiku.de>
 *               2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Test application for HX711 scale ADCs
 *
 * @author      Philipp-Alexander Blum <philipp-blum@jakiku.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "thread.h"
#include "periph/gpio.h"
#include "arduino_pinmap.h"
#include "hx711.h"
#include "hx711_params.h"

static hx711_t dev;

void calibrate(void)
{
    hx711_power_up(&dev);
    hx711_tare(&dev, 50);
    hx711_power_down(&dev);
}

int main(void)
{
    puts("RIOT HX711 Scale");
    puts("=====================================");

    puts("Init HX711...");

    hx711_init(&dev, hx711_params);

    puts("Initialized HX711.");

    puts("Calibration...");
    calibrate();
    puts("Calibrated.");

    while (1) {
        hx711_power_up(&dev);
        printf("Read value: %" PRIu32 "\n", hx711_get_value(&dev, 10));
        hx711_power_down(&dev);
    }


    return 0;
}
