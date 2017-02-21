/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <errno.h>
#include <string.h>

#include "fmt.h"
#include "log.h"
#include "gsm.h"
#include "xtimer.h"
#include "net/ipv4/addr.h"

#define MODEM_INIT_MAXTRIES (3)

int gsm_init(gsm_t *gsmdev, const gsm_params_t *params)
{
    unsigned tries = MODEM_INIT_MAXTRIES;
    int res;

    LOG_INFO("sim800x: initializing...\n");

    at_dev_init(&gsmdev->at_dev, params->uart, params->baudrate, gsmdev->buf, sizeof(gsmdev->buf));
    mutex_init(&gsmdev->mutex);

    at_dev_t *at_dev = &gsmdev->at_dev;

    do {
        xtimer_usleep(100000U);
        res = at_send_cmd_wait_ok(at_dev, "AT", GSM_SERIAL_TIMEOUT);
    } while (res != 0 && (tries--));

    if (res) {
        return -1;
    }

    char buf[16];
    res = at_send_cmd_get_resp(at_dev, "ATI", buf, sizeof(buf), GSM_SERIAL_TIMEOUT);
    LOG_INFO("gsm: device type %s\n", buf);

    return 0;
}

int gsm_set_pin(gsm_t *gsmdev, const char *pin)
{
    if (strlen(pin) != 4) {
        return -EINVAL;
    }

    mutex_lock(&gsmdev->mutex);

    xtimer_usleep(100000U);

    char buf[16];
    char *pos = buf;
    pos += fmt_str(pos, "AT+CPIN=");
    pos += fmt_str(pos, pin);
    *pos = '\0';

    int res = at_send_cmd_wait_ok(&gsmdev->at_dev, buf, GSM_SERIAL_TIMEOUT);

    mutex_unlock(&gsmdev->mutex);
    return res;
}

int gsm_check_pin(gsm_t *gsmdev)
{
    char linebuf[32];

    mutex_lock(&gsmdev->mutex);
    xtimer_usleep(100000U);

    int res = at_send_cmd_get_resp(&gsmdev->at_dev, "AT+CPIN?", linebuf, sizeof(linebuf), GSM_SERIAL_TIMEOUT);

    if (res > 0) {
        if (!strncmp(linebuf, "OK", res)) {
            res = 0;
        }
        else if (!strcmp(linebuf, "+CPIN: READY")) {
            /* sim is ready */
            res = 0;
        }
        else if (!strcmp(linebuf, "+CPIN: SIM PIN")) {
            /* sim needs pin */
            res = 1;
        }
    }

    mutex_unlock(&gsmdev->mutex);
    return res;
}

//int gsm_gprs_init(gsm_t *gsmdev, const char *apn)
//{
//    char buf[64];
//    int res;
//    at_dev_t *at_dev = &gsmdev->at_dev;
//
//    mutex_lock(&gsmdev->mutex);
//
//    /* close possible open GPRS connection */
//    at_send_cmd_wait_ok(at_dev, "AT+SAPBR=0,1", GSM_SERIAL_TIMEOUT);
//
//    /* set GPRS */
//    res = at_send_cmd_wait_ok(at_dev, "AT+SAPBR=3,1,\"Contype\",\"GPRS\"", GSM_SERIAL_TIMEOUT);
//    if (res) {
//        goto out;
//    }
//
//    /* set APN */
//    char *pos = buf;
//    pos += fmt_str(pos, "AT+SAPBR=3,1,\"APN\",\"");
//    pos += fmt_str(pos, apn);
//    pos += fmt_str(pos, "\"");
//    *pos = '\0';
//    res = at_send_cmd_wait_ok(at_dev, buf, GSM_SERIAL_TIMEOUT);
//    if (res) {
//        goto out;
//    }
//
//    xtimer_usleep(100000U);
//
//    /* AT+SAPBR=1,1 -> open GPRS */
//    res = at_send_cmd_wait_ok(at_dev, "AT+SAPBR=1,1", GSM_SERIAL_TIMEOUT * 5);
//
//out:
//    mutex_unlock(&gsmdev->mutex);
//
//    return res;
//}

int gsm_gprs_init(gsm_t *gsmdev, const char *apn)
{
    char buf[64];
    int res;
    at_dev_t *at_dev = &gsmdev->at_dev;

    mutex_lock(&gsmdev->mutex);

    /* detach possibly attached data session, ignore result */
    at_send_cmd_get_resp(at_dev, "AT+CGATT=0", buf, sizeof(buf), GSM_SERIAL_TIMEOUT);

    /* set APN */
    char *pos = buf;
    pos += fmt_str(pos, "AT+CGDCONT=1,\"IP\",\"");
    pos += fmt_str(pos, apn);
    pos += fmt_str(pos, "\"");
    *pos = '\0';
    res = at_send_cmd_wait_ok(at_dev, buf, GSM_SERIAL_TIMEOUT);
    if (res) {
        goto out;
    }

    /* setup IP session via APN */
    res = at_send_cmd_wait_ok(at_dev, "AT+CGACT=1,1", 150LLU*(1000000));
    if (res) {
        goto out;
    }

    xtimer_usleep(100000U);

out:
    mutex_unlock(&gsmdev->mutex);

    return res;
}

int gsm_reg_check(gsm_t *gsmdev)
{
    char buf[64];

    mutex_lock(&gsmdev->mutex);

    int res = at_send_cmd_get_resp(&gsmdev->at_dev, "AT+COPS?", buf, sizeof(buf), GSM_SERIAL_TIMEOUT);
    if ((res > 0) && (strncmp(buf, "+COPS: 0,", 9) == 0)) {
        res = 0;
    }
    else {
        res = -1;
    }

    mutex_unlock(&gsmdev->mutex);

    return res;
}

size_t gsm_reg_get(gsm_t *gsmdev, char *outbuf, size_t len)
{
    char buf[64];
    char *pos = buf;

    mutex_lock(&gsmdev->mutex);

    size_t outlen = 0;
    int res = at_send_cmd_get_resp(&gsmdev->at_dev, "AT+COPS?", buf, sizeof(buf), GSM_SERIAL_TIMEOUT);
    if ((res > 12) && (strncmp(pos, "+COPS: 0,", 9) == 0)) {
        /* skip '+COPS: 0,[01],"' */
        pos += 12;

        while (*pos != '"' && len--) {
            *outbuf++ = *pos++;
            outlen++;
        }
        if (len) {
            *outbuf = '\0';
            res = outlen;
        }
        else {
            return -ENOSPC;
        }
    }
    else {
        res = -1;
    }

    mutex_unlock(&gsmdev->mutex);

    return res;
}

ssize_t gsm_imei_get(gsm_t *gsmdev, char *buf, size_t len)
{
    mutex_lock(&gsmdev->mutex);

    int res = at_send_cmd_get_resp(&gsmdev->at_dev, "AT+GSN", buf, len, GSM_SERIAL_TIMEOUT);
    if (res > 0) {
        buf[res] = '\0';
    }
    else {
        res = -1;
    }

    mutex_unlock(&gsmdev->mutex);

    return res;
}

int gsm_signal_get(gsm_t *gsmdev, unsigned *rssi, unsigned *ber)
{
    char buf[32];
    char *pos = buf;

    mutex_lock(&gsmdev->mutex);

    int res = at_send_cmd_get_resp(&gsmdev->at_dev, "AT+CSQ", buf, sizeof(buf), GSM_SERIAL_TIMEOUT);
    if ((res > 2) && strncmp(buf, "+CSQ: ", 6) == 0) {
        pos += 6; /* skip "+CSQ: " */
        *rssi = scn_u32_dec(pos, 2);
        pos += 2 + (*rssi > 9); /* skip rssi value ( n, or nn,) */
        *ber = scn_u32_dec(pos, 2);
        res = 0;
    }
    else {
        res = -1;
    }

    mutex_unlock(&gsmdev->mutex);

    return res;
}

//uint32_t gsm_gprs_getip(gsm_t *gsmdev)
//{
//    char buf[40];
//    char *pos = buf;
//    uint32_t ip = 0;
//
//    mutex_lock(&gsmdev->mutex);
//
//    int res = at_send_cmd_get_resp(&gsmdev->at_dev, "AT+SAPBR=2,1", buf, sizeof(buf), GSM_SERIAL_TIMEOUT);
//    if ((res > 13) && strncmp(buf, "+SAPBR: 1,1,\"", 13) == 0) {
//        res -= 1;   /* cut off " */
//        buf[res] = '\0';
//        pos += 13; /* skip +SAPBR: 1,1," */
//
//        ipv4_addr_from_str((ipv4_addr_t *)&ip, pos);
//    }
//
//    mutex_unlock(&gsmdev->mutex);
//
//    return ip;
//}

uint32_t gsm_gprs_getip(gsm_t *gsmdev)
{
    char buf[40];
    char *pos = buf;
    uint32_t ip = 0;

    mutex_lock(&gsmdev->mutex);

    int res = at_send_cmd_get_resp(&gsmdev->at_dev, "AT+CGPADDR=1", buf, sizeof(buf), GSM_SERIAL_TIMEOUT);
    if ((res > 13) && strncmp(buf, "+CGPADDR: 1, \"", 14) == 0) {
        res -= 1;   /* cut off " */
        buf[res] = '\0';
        pos += 14; /* skip '+CGPADDR: 1, "' */

        ipv4_addr_from_str((ipv4_addr_t *)&ip, pos);
    }

    mutex_unlock(&gsmdev->mutex);

    return ip;
}

ssize_t gsm_http_get(gsm_t *gsmdev, const char *url, uint8_t *resultbuf, size_t len)
{
    int res;
    at_dev_t *at_dev = &gsmdev->at_dev;

    at_send_cmd_wait_ok(at_dev, "AT+HTTPTERM", GSM_SERIAL_TIMEOUT);

    res = at_send_cmd_wait_ok(at_dev, "AT+HTTPINIT", GSM_SERIAL_TIMEOUT);
    if (res) {
        goto out;
    }

    res = at_send_cmd_wait_ok(at_dev, "AT+HTTPPARA=\"CID\",1", GSM_SERIAL_TIMEOUT);
    if (res) {
        goto out;
    }

    char buf[128];
    char *pos = buf;
    pos += fmt_str(pos, "AT+HTTPPARA=\"URL\",\"");
    pos += fmt_str(pos, url);
    pos += fmt_str(pos, "\"");
    *pos = '\0';
    res = at_send_cmd_wait_ok(at_dev, buf, GSM_SERIAL_TIMEOUT);
    if (res) {
        goto out;
    }

    if (strncmp(url, "https://", 8) == 0) {
        res = at_send_cmd_wait_ok(at_dev, "AT+HTTPSSL=1", GSM_SERIAL_TIMEOUT);
        if (res) {
            goto out;
        }
    }

    res = at_send_cmd_wait_ok(at_dev, "AT+HTTPACTION=0", GSM_SERIAL_TIMEOUT);
    if (res) {
        goto out;
    }

    /* skip expected empty line */
    at_readline(at_dev, buf, sizeof(buf), GSM_SERIAL_TIMEOUT * 30);
    res = at_readline(at_dev, buf, sizeof(buf), GSM_SERIAL_TIMEOUT);
    if (res > 0) {
        if (strncmp(buf, "+HTTPACTION: 0,200,", 19) == 0) {
            /* read length from buf. substract beginning (19) */
            uint32_t response_len = scn_u32_dec(buf + 19, res - 19);
            response_len = response_len > len ? len : response_len;
            at_send_cmd(at_dev, "AT+HTTPREAD", GSM_SERIAL_TIMEOUT);
            /* skip +HTTPREAD: <nbytes> */
            at_readline(at_dev, buf, sizeof(buf), GSM_SERIAL_TIMEOUT);
            res = isrpipe_read_all_timeout(&at_dev->isrpipe, (char *)resultbuf, response_len, GSM_SERIAL_TIMEOUT * 5);
            at_expect_bytes(at_dev, "\r\nOK\r\n", 6, GSM_SERIAL_TIMEOUT);
        }
        else {
            return -1;
        }
    }

out:
    mutex_unlock(&gsmdev->mutex);

    return res;
}

ssize_t gsm_http_post(gsm_t *gsmdev,
                         const char *url,
                         const uint8_t *data, size_t data_len,
                         uint8_t *resultbuf, size_t result_len)
{
    int res;
    at_dev_t *at_dev = &gsmdev->at_dev;

    mutex_lock(&gsmdev->mutex);

    at_send_cmd_wait_ok(at_dev, "AT+HTTPTERM", GSM_SERIAL_TIMEOUT);

    res = at_send_cmd_wait_ok(at_dev, "AT+HTTPINIT", GSM_SERIAL_TIMEOUT);
    if (res) {
        goto out;
    }

    res = at_send_cmd_wait_ok(at_dev, "AT+HTTPPARA=\"CID\",1", GSM_SERIAL_TIMEOUT);
    if (res) {
        goto out;
    }

    char buf[128];
    char *pos = buf;
    pos += fmt_str(pos, "AT+HTTPPARA=\"URL\",\"");
    pos += fmt_str(pos, url);
    pos += fmt_str(pos, "\"");
    *pos = '\0';
    res = at_send_cmd_wait_ok(at_dev, buf, GSM_SERIAL_TIMEOUT);
    if (res) {
        goto out;
    }

    if (strncmp(url, "https://", 8) == 0) {
        res = at_send_cmd_wait_ok(at_dev, "AT+HTTPSSL=1", GSM_SERIAL_TIMEOUT);
        if (res) {
            goto out;
        }
    }

    pos = buf;
    pos += fmt_str(pos, "AT+HTTPDATA=\"");
    pos += fmt_u32_dec(pos, data_len);
    pos += fmt_str(pos, "\",");
    pos += fmt_u32_dec(pos, 10000);
    *pos = '\0';
    res = at_send_cmd_get_resp(at_dev, buf, buf, sizeof(buf), GSM_SERIAL_TIMEOUT);
    if (res <= 0) {
        goto out;
    }
    else if (strcmp(buf, "DOWNLOAD")) {
        res = -1;
        goto out;
    }

    uart_write(at_dev->uart, data, data_len);

    res = at_send_cmd_wait_ok(at_dev, "AT+HTTPACTION=1", GSM_SERIAL_TIMEOUT);
    if (res) {
        goto out;
    }

    /* skip expected empty line */
    at_readline(at_dev, buf, sizeof(buf), GSM_SERIAL_TIMEOUT * 30);

    res = at_readline(at_dev, buf, sizeof(buf), GSM_SERIAL_TIMEOUT);
    if ((res > 0) && (strncmp(buf, "+HTTPACTION: 1,200,", 19) == 0)) {
        /* read length from buf. substract beginning (19) */
        uint32_t response_len = scn_u32_dec(buf + 19, res - 19);

        /* min(response_len, result_len) */
        response_len = response_len > result_len ? result_len : response_len;

        at_send_cmd(at_dev, "AT+HTTPREAD", GSM_SERIAL_TIMEOUT);

        /* skip +HTTPREAD: <nbytes> */
        at_readline(at_dev, buf, sizeof(buf), GSM_SERIAL_TIMEOUT);

        /* read response directly */
        res = isrpipe_read_all_timeout(&at_dev->isrpipe, (char *)resultbuf, response_len, GSM_SERIAL_TIMEOUT * 5);

        /* drain expected extra output */
        at_expect_bytes(at_dev, "\r\nOK\r\n", 6, GSM_SERIAL_TIMEOUT);
    }

out:
    mutex_unlock(&gsmdev->mutex);

    return res;
}

void gsm_print_status(gsm_t *gsmdev)
{
    char buf[64];

    int res = at_send_cmd_get_resp(&gsmdev->at_dev, "ATI", buf, sizeof(buf), GSM_SERIAL_TIMEOUT);

    if (res >= 2) {
        res -= 2;
        buf[res] = '\0';
        printf("gsm: device type %s\n", buf);
    }
    else {
        printf("gsm: error reading device type!\n");
    }

    res = gsm_imei_get(gsmdev, buf, sizeof(buf));
    if (res >= 0) {
        printf("gsm: IMEI: \"%s\"\n", buf);
    }
    else {
        printf("gsm: error getting IMEI\n");
    }

    res = gsm_reg_get(gsmdev, buf, sizeof(buf));
    if (res >= 0) {
        printf("gsm: registered to \"%s\"\n", buf);
    }
    else {
        printf("gsm: not registered\n");
    }

    unsigned rssi;
    unsigned ber;
    res = gsm_signal_get(gsmdev, &rssi, &ber);
    if (res == 0) {
        printf("gsm: RSSI=%u ber=%u%%\n", rssi, ber);
    }
    else {
        printf("gsm: error getting signal strength\n");
    }

    uint32_t ip = gsm_gprs_getip(gsmdev);
    if (ip) {
        printf("gsm: GPRS connected. IP=");
        for (unsigned i = 0; i < 4; i++) {
            uint8_t *_tmp = (uint8_t*) &ip;
            printf("%u%s", (unsigned)_tmp[i], (i < 3) ? "." : "\n");
        }
    }
    else {
        printf("gsm: error getting GPRS state\n");
    }
}

int gsm_gps_get_loc(gsm_t *gsmdev, uint8_t *buf, size_t len)
{
    mutex_lock(&gsmdev->mutex);

    int res = at_send_cmd_get_resp(&gsmdev->at_dev, "AT+CGPSINF=0", (char *)buf, len, GSM_SERIAL_TIMEOUT);

    mutex_unlock(&gsmdev->mutex);

    return res;
}

size_t gsm_cmd(gsm_t *gsmdev, const char *cmd, uint8_t *buf, size_t len)
{
    mutex_lock(&gsmdev->mutex);

    size_t res = at_send_cmd_get_resp(&gsmdev->at_dev, cmd, (char *)buf, len, GSM_SERIAL_TIMEOUT * 10);

    mutex_unlock(&gsmdev->mutex);

    return res;
}
