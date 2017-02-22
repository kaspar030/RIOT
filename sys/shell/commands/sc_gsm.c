/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_shell_commands
 * @{
 *
 * @file
 * @brief       Provides shell commands to test GSM GSM/GPRS modem modules
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "fmt.h"
#include "gsm.h"
#include "xtimer.h"

#ifdef MODULE_GSM

static bool initialized = false;
static gsm_t dev;

static char _http_buf[1024];

int _gsm(int argc, char **argv)
{
    if(argc <= 1) {
        printf("Usage: %s init|status|http|loc|cmd\n", argv[0]);
        return -1;
    }

    if(strcmp(argv[1], "init") == 0) {
        if(argc < 5) {
            printf("Usage: %s init <uart> <baudrate> <apn> [<pin>]\n"
                   "  e.g. %s init 1 115200 internet.t-mobile 1234\n", argv[0], argv[0]);
            return -1;
        }

        uint32_t uartnr = scn_u32_dec(argv[2], 8);
        if (uartnr == 0 && (argv[2][0] != '0')) {
            printf("gsm: cannot parse uart\n");
            return -1;
        }

        uint32_t baudrate = scn_u32_dec(argv[3], 8);
        if (baudrate == 0) {
            printf("gsm: cannot parse baudrate\n");
            return -1;
        }


        if (argc == 6) {
            if (strlen(argv[5]) != 4) {
                printf("gsm: invalid pin format\n");
                return -1;
            }
        }

        gsm_params_t params = { .uart=uartnr, .baudrate=baudrate };
        int res = gsm_init(&dev, &params);
        if (res) {
            printf("gsm: error initializing. res=%i\n", res);
            return -1;
        }

        initialized = true;

        if (argc == 6) {
            if (gsm_check_pin(&dev) == 1) {
                res = gsm_set_pin(&dev, argv[5]);
                if (res) {
                    printf("gsm: error setting pin\n");
                    return -1;
                }
            }
            else {
                printf("gsm: device SIM already unlocked\n");
            }
        }

        puts("gsm: waiting for network registration...");
        while (gsm_reg_check(&dev)) {
            xtimer_usleep(1000000U);
        }

        char buf[32];
        res = gsm_reg_get(&dev, buf, sizeof(buf));
        if (res > 0) {
            printf("gsm: registered to \"%s\"\n", buf);
        }

        res = gsm_gprs_init(&dev, argv[4]);
        if (res) {
            printf("gsm: error initializing GPRS. res=%i\n", res);
            return -1;
        }

        uint32_t ip = gsm_gprs_getip(&dev);
        if (ip) {
            printf("gsm: GPRS connect successful. IP=");
            for (unsigned i = 0; i < 4; i++) {
                uint8_t *_tmp = (uint8_t*) &ip;
                printf("%u%s", (unsigned)_tmp[i], (i < 3) ? "." : "\n");
            }
        }
        else {
            printf("gsm: error getting GPRS state\n");
            return -1;
        }
    }

    else if(strcmp(argv[1], "status") == 0) {
        if (!initialized) {
            printf("gsm: not initialize. do so using \"gsm init ...\"\n");
            return -1;
        }

        gsm_print_status(&dev);
    }
    else if (strcmp(argv[1], "http") == 0) {
        if (argc != 3 && argc != 4) {
            printf("Usage: %s httpreq <url> [<POST-data>]\n", argv[0]);
            return -1;
        }


        int res;
        if (argc == 3) {
            res = gsm_http_get(&dev, argv[2], (uint8_t *)_http_buf, sizeof(_http_buf));
        }
        else {
            res = gsm_http_post(&dev, argv[2], (uint8_t *)argv[3], strlen(argv[3]),
                    (uint8_t *)_http_buf, sizeof(_http_buf));
        }

        if (res < 0) {
            printf("gsm: error\n");
        }
        else {
            printf("gsm: request ok. data:\n");
            puts(_http_buf);
        }
    }
    else if (strcmp(argv[1], "gps") == 0) {
        int res = gsm_gps_get_loc(&dev, (uint8_t *)_http_buf, sizeof(_http_buf));

        if (res < 0) {
            printf("gsm: error\n");
        }
        else {
            printf("gsm: request ok. data:\n");
            puts(_http_buf);
        }
    }
    else if (strcmp(argv[1], "cmd") == 0) {
        if (argc < 3) {
            printf("Usage: %s cmd <command> [<timeout>]\n", argv[0]);
            return -1;
        }

        unsigned timeout = 10;
        if (argc == 4) {
            timeout = atoi(argv[3]);
        }

        gsm_cmd(&dev, argv[2], (uint8_t *)_http_buf, sizeof(_http_buf), timeout);

        /* gsm_cmd will internally memset(0) the buffer, so printing here is
         * safe even if the command errors. */
        printf("gsm: response:\n");
        puts(_http_buf);
    }
    else if (strcmp(argv[1], "loc") == 0) {
        char lon[16];
        char lat[16];

        int res = gsm_get_loc(&dev, lon, lat);

        if (res < 0) {
            printf("gsm: error\n");
        }
        else {
            printf("gms: location: lontitude=%s latitude=%s\n", lon, lat);
        }
    }
    else if (strcmp(argv[1], "towers") == 0) {
        int res = gsm_cnet_scan(&dev, _http_buf, sizeof(_http_buf));

        if (res < 0) {
            printf("gsm: error\n");
        }
        else {
            printf("gsm: response:\n");
            puts(_http_buf);
        }
    }

    return 0;
}

#endif /* MODULE_GSM */
