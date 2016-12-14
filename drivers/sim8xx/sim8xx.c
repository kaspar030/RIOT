/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <string.h>

#include "fmt.h"
#include "log.h"
#include "sim8xx.h"
#include "xtimer.h"

#define SIMDEV_INIT_MAXTRIES (3)

int sim8xx_init(sim8xx_t *simdev, const sim8xx_params_t *params)
{
    unsigned tries = SIMDEV_INIT_MAXTRIES;
    int res;

    LOG_INFO("sim800x: initializing...\n");

    at_dev_init(&simdev->at_dev, params->uart, params->baudrate, simdev->buf, sizeof(simdev->buf));
    mutex_init(&simdev->mutex);

    at_dev_t *at_dev = &simdev->at_dev;

    do {
        xtimer_usleep(100000U);
        res = at_send_cmd_wait_ok(at_dev, "AT", SIM8XX_SERIAL_TIMEOUT);
    } while (res != 0 && (tries--));

    if (res) {
        return -1;
    }

    char buf[16];
    res = at_send_cmd_get_resp(at_dev, "ATI", buf, sizeof(buf), SIM8XX_SERIAL_TIMEOUT);
    LOG_INFO("sim8xx: device type %s\n", buf);

    return 0;
}

int sim8xx_set_pin(sim8xx_t *simdev, unsigned pin)
{
    mutex_lock(&simdev->mutex);

    xtimer_usleep(100000U);

    char buf[16];
    char *pos = buf;
    pos += fmt_str(buf, "AT+CPIN=");
    pos += fmt_u32_dec(buf + 8, pin);
    *pos = '\0';

    int res = at_send_cmd_wait_ok(&simdev->at_dev, buf, SIM8XX_SERIAL_TIMEOUT);

    mutex_unlock(&simdev->mutex);
    return res;
}

int sim8xx_check_pin(sim8xx_t *simdev)
{
    char linebuf[32];

    mutex_lock(&simdev->mutex);
    xtimer_usleep(100000U);

    int res = at_send_cmd_get_resp(&simdev->at_dev, "AT+CPIN?", linebuf, sizeof(linebuf), SIM8XX_SERIAL_TIMEOUT);

    if (res > 0) {
        if (!strncmp(linebuf, "OK", res)) {
            res = 0;
        }
        else if (!strcmp(linebuf, "+CPIN: READY\r\n")) {
            /* sim is ready */
            res = 0;
        }
        else if (!strcmp(linebuf, "+CPIN: SIM PIN\r\n")) {
            /* sim needs pin */
            res = 1;
        }
    }

    mutex_unlock(&simdev->mutex);
    return res;
}

int sim8xx_gprs_init(sim8xx_t *simdev, const char *apn)
{
    char buf[64];
    int res;
    at_dev_t *at_dev = &simdev->at_dev;

    mutex_lock(&simdev->mutex);

    /* close possible open GPRS connection */
    at_send_cmd_wait_ok(at_dev, "AT+SAPBR=0,1", SIM8XX_SERIAL_TIMEOUT);

    /* set GPRS */
    res = at_send_cmd_wait_ok(at_dev, "AT+SAPBR=3,1,\"Contype\",\"GPRS\"", SIM8XX_SERIAL_TIMEOUT);
    if (res) {
        goto out;
    }

    /* set APN */
    char *pos = buf;
    pos += fmt_str(pos, "AT+SAPBR=3,1,\"APN\",\"");
    pos += fmt_str(pos, apn);
    pos += fmt_str(pos, "\"");
    *pos = '\0';
    res = at_send_cmd_wait_ok(at_dev, buf, SIM8XX_SERIAL_TIMEOUT);
    if (res) {
        goto out;
    }

    xtimer_usleep(100000U);

    /* AT+SAPBR=1,1 -> open GPRS */
    res = at_send_cmd_wait_ok(at_dev, "AT+SAPBR=1,1", SIM8XX_SERIAL_TIMEOUT * 5);

out:
    mutex_unlock(&simdev->mutex);

    return res;
}

int sim8xx_reg_check(sim8xx_t *simdev)
{
    char buf[32];

    mutex_lock(&simdev->mutex);

    int res = at_send_cmd_get_resp(&simdev->at_dev, "AT+COPS?", buf, sizeof(buf), SIM8XX_SERIAL_TIMEOUT);
    if ((res > 0) && (strncmp(buf, "+COPS: 0,", 9) == 0)) {
        res = 0;
    }
    else {
        res = -1;
    }

    mutex_unlock(&simdev->mutex);

    return res;
}

ssize_t sim8xx_http_get(sim8xx_t *simdev, const char *url, uint8_t *resultbuf, size_t len)
{
    int res;
    at_dev_t *at_dev = &simdev->at_dev;

    at_send_cmd_wait_ok(at_dev, "AT+HTTPTERM", SIM8XX_SERIAL_TIMEOUT);

    res = at_send_cmd_wait_ok(at_dev, "AT+HTTPINIT", SIM8XX_SERIAL_TIMEOUT);
    if (res) {
        goto out;
    }

    res = at_send_cmd_wait_ok(at_dev, "AT+HTTPPARA=\"CID\",1", SIM8XX_SERIAL_TIMEOUT);
    if (res) {
        goto out;
    }

    char buf[128];
    char *pos = buf;
    pos += fmt_str(pos, "AT+HTTPPARA=\"URL\",\"");
    pos += fmt_str(pos, url);
    pos += fmt_str(pos, "\"");
    *pos = '\0';
    res = at_send_cmd_wait_ok(at_dev, buf, SIM8XX_SERIAL_TIMEOUT);
    if (res) {
        goto out;
    }

    res = at_send_cmd_wait_ok(at_dev, "AT+HTTPACTION=0", SIM8XX_SERIAL_TIMEOUT);
    if (res) {
        goto out;
    }

    /* skip expected empty line */
    at_readline(at_dev, buf, sizeof(buf), SIM8XX_SERIAL_TIMEOUT);
    res = at_readline(at_dev, buf, sizeof(buf), SIM8XX_SERIAL_TIMEOUT * 30);
    if (res > 0) {
        if (strncmp(buf, "+HTTPACTION: 0,200,", 19) == 0) {
            /* read length from buf. substract beginning (19) and \r\n (2) */
            uint32_t response_len = scn_u32_dec(buf + 19, res - 19 - 2);
            response_len = response_len > len ? len : response_len;
            at_send_cmd(at_dev, "AT+HTTPREAD", SIM8XX_SERIAL_TIMEOUT);
            /* skip +HTTPREAD: <nbytes> */
            at_readline(at_dev, buf, sizeof(buf), SIM8XX_SERIAL_TIMEOUT);
            res = isrpipe_read_all_timeout(&at_dev->isrpipe, (char *)resultbuf, response_len, SIM8XX_SERIAL_TIMEOUT * 5);
            at_expect_bytes(at_dev, "\r\nOK\r\n", 6, SIM8XX_SERIAL_TIMEOUT);
        }
    }

out:
    mutex_unlock(&simdev->mutex);

    return res;
}

ssize_t sim8xx_http_post(sim8xx_t *simdev,
                         const char *url,
                         const uint8_t *data, size_t data_len,
                         uint8_t *resultbuf, size_t result_len)
{
    int res;
    at_dev_t *at_dev = &simdev->at_dev;

    mutex_lock(&simdev->mutex);

    at_send_cmd_wait_ok(at_dev, "AT+HTTPTERM", SIM8XX_SERIAL_TIMEOUT);

    res = at_send_cmd_wait_ok(at_dev, "AT+HTTPINIT", SIM8XX_SERIAL_TIMEOUT);
    if (res) {
        goto out;
    }

    res = at_send_cmd_wait_ok(at_dev, "AT+HTTPPARA=\"CID\",1", SIM8XX_SERIAL_TIMEOUT);
    if (res) {
        goto out;
    }

    char buf[128];
    char *pos = buf;
    pos += fmt_str(pos, "AT+HTTPPARA=\"URL\",\"");
    pos += fmt_str(pos, url);
    pos += fmt_str(pos, "\"");
    *pos = '\0';
    res = at_send_cmd_wait_ok(at_dev, buf, SIM8XX_SERIAL_TIMEOUT);
    if (res) {
        goto out;
    }

    pos = buf;
    pos += fmt_str(pos, "AT+HTTPDATA=\"");
    pos += fmt_u32_dec(pos, data_len);
    pos += fmt_str(pos, "\",");
    pos += fmt_u32_dec(pos, 10000);
    *pos = '\0';
    res = at_send_cmd_get_resp(at_dev, buf, buf, sizeof(buf), SIM8XX_SERIAL_TIMEOUT);
    if (res <= 0) {
        goto out;
    }
    else if (strcmp(buf, "DOWNLOAD\r\n")) {
        res = -1;
        goto out;
    }

    uart_write(at_dev->uart, data, data_len);

    res = at_send_cmd_wait_ok(at_dev, "AT+HTTPACTION=1", SIM8XX_SERIAL_TIMEOUT);
    if (res) {
        goto out;
    }

    /* skip expected empty line */
    at_readline(at_dev, buf, sizeof(buf), SIM8XX_SERIAL_TIMEOUT * 5);

    res = at_readline(at_dev, buf, sizeof(buf), SIM8XX_SERIAL_TIMEOUT * 30);
    if ((res > 0) && (strncmp(buf, "+HTTPACTION: 1,200,", 19) == 0)) {
        /* read length from buf. substract beginning (19) and \r\n (2) */
        uint32_t response_len = scn_u32_dec(buf + 19, res - 19 - 2);

        /* min(response_len, result_len) */
        response_len = response_len > result_len ? result_len : response_len;

        at_send_cmd(at_dev, "AT+HTTPREAD", SIM8XX_SERIAL_TIMEOUT);

        /* skip +HTTPREAD: <nbytes> */
        at_readline(at_dev, buf, sizeof(buf), SIM8XX_SERIAL_TIMEOUT);

        /* read response directly */
        res = isrpipe_read_all_timeout(&at_dev->isrpipe, (char *)resultbuf, response_len, SIM8XX_SERIAL_TIMEOUT * 5);

        /* drain expected extra output */
        at_expect_bytes(at_dev, "\r\nOK\r\n", 6, SIM8XX_SERIAL_TIMEOUT);
    }

out:
    mutex_unlock(&simdev->mutex);

    return res;
}
