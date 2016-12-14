/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Manual test application for SIM8xx GSM modules
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "fmt.h"
#include "sim8xx.h"
#include "xtimer.h"

static sim8xx_t sim8xx;
static const sim8xx_params_t sim8xx_params = { .uart=SIM8XX_UART, .baudrate=SIM8XX_BAUDRATE };

static char buf[1024];

int main(void)
{
    puts("sim8xx test application started.");
    puts("sleeping one second");
    xtimer_usleep(1000000U);
    puts("done");

    sim8xx_init(&sim8xx, &sim8xx_params);

    int res = sim8xx_check_pin(&sim8xx);
    printf("SIM PIN check: %i\n", res);
    if (res == 1) {
#ifdef SIM_PIN
        res = sim8xx_set_pin(&sim8xx, SIM_PIN);
        printf("SIM SET: %i\n", res);
#endif
        if (res) {
            goto fail;
        }
    }

    while (sim8xx_reg_check(&sim8xx)) {
        puts("waiting for network registration...");
        xtimer_usleep(1000000U);
    }

    res = sim8xx_gprs_init(&sim8xx, SIM8XX_APN);
    printf("gprs init res: %i\n", res);
    if (res) {
        goto fail;
    }

    res = sim8xx_http_post(&sim8xx, "http://posttestserver.com/post.php?dump",
            (const uint8_t *) "TEST", 4, (uint8_t*)buf, sizeof(buf));

    printf("http post res: %i\n", res);
    if (res >= 0) {
        puts("---- response :");
        print(buf, res);
        puts("\n---- done.");
        return 0;
    }

fail:
    puts("error");

    return 1;
}
