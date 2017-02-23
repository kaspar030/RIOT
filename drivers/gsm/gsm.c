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

static int gsm_set_driver(gsm_t *gsmdev);

int gsm_init(gsm_t *gsmdev, const gsm_params_t *params)
{
    unsigned tries = MODEM_INIT_MAXTRIES;
    int res;

    LOG_INFO("gsm: initializing...\n");

    at_dev_init(&gsmdev->at_dev, params->uart, params->baudrate, gsmdev->buf, sizeof(gsmdev->buf));
    mutex_init(&gsmdev->mutex);

    at_dev_t *at_dev = &gsmdev->at_dev;

    do {
        xtimer_usleep(100000U);
        res = at_send_cmd_wait_ok(at_dev, "AT", GSM_SERIAL_TIMEOUT);
    } while (res != 0 && (tries--));

    if (res) {
        goto out;
    }

    res = gsm_set_driver(gsmdev);

out:
    return res;
}

int gsm_set_pin(gsm_t *gsmdev, const char *pin)
{
    if (strlen(pin) != 4) {
        return -EINVAL;
    }

    mutex_lock(&gsmdev->mutex);

    xtimer_usleep(100000U);

    char buf[32];
    char *pos = buf;
    pos += fmt_str(pos, "AT+CPIN=");
    pos += fmt_str(pos, pin);
    *pos = '\0';

    int res = at_send_cmd_get_resp(&gsmdev->at_dev, buf, buf, sizeof(buf), GSM_SERIAL_TIMEOUT);
    mutex_unlock(&gsmdev->mutex);

    if (res > 0) {
        /* if one matches (equals 0), return 0 */
        return strncmp(buf, "OK", 2) && strncmp(buf, "+CPIN: READY", 12);
    }
    else {
        return -1;
    }
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
    at_send_cmd_get_lines(at_dev, "AT+CGATT=0", buf, sizeof(buf), GSM_SERIAL_TIMEOUT);

    /* set IP connection and APN name */
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

    res = gsm_iccid_get(gsmdev, buf, sizeof(buf));
    if (res >= 0) {
        printf("gsm: ICCID: \"%s\"\n", buf);
    }
    else {
        printf("gsm: error getting ICCID\n");
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

int gsm_iccid_get(gsm_t *gsmdev, char *buf, size_t len)
{
    mutex_lock(&gsmdev->mutex);

    int res = at_send_cmd_get_resp(&gsmdev->at_dev, "AT+CCID", buf, len, GSM_SERIAL_TIMEOUT);
    if (res > 0) {
        if (!strncmp(buf, "+CCID: ", 7)) {
            memcpy(buf, buf+8, res-9);
            buf[res-10] = '\0';
        }
    }
    else {
        res = -1;
    }

    mutex_unlock(&gsmdev->mutex);

    return res;
}

int gsm_gps_get_loc(gsm_t *gsmdev, uint8_t *buf, size_t len)
{
    mutex_lock(&gsmdev->mutex);

    int res = at_send_cmd_get_resp(&gsmdev->at_dev, "AT+CGPSINF=0", (char *)buf, len, GSM_SERIAL_TIMEOUT);

    mutex_unlock(&gsmdev->mutex);

    return res;
}

size_t gsm_cmd(gsm_t *gsmdev, const char *cmd, uint8_t *buf, size_t len, unsigned timeout)
{
    mutex_lock(&gsmdev->mutex);

    size_t res = at_send_cmd_get_lines(&gsmdev->at_dev, cmd, (char *)buf, len, GSM_SERIAL_TIMEOUT * timeout);

    mutex_unlock(&gsmdev->mutex);

    return res;
}

int gsm_get_loc(gsm_t *gsmdev, char *lon, char *lat)
{
    return gsmdev->driver->get_loc(gsmdev, lon, lat);
}

int gsm_cnet_scan(gsm_t *gsmdev, char *buf, size_t len)
{
    return gsmdev->driver->cnet_scan(gsmdev, buf, len);
}

ssize_t gsm_http_get(gsm_t *gsmdev, const char *url, uint8_t *resultbuf, size_t len)
{
    return gsmdev->driver->http_get(gsmdev, url, resultbuf, len);
}

ssize_t gsm_http_post(gsm_t *gsmdev,
                         const char *url,
                         const uint8_t *data, size_t data_len,
                         uint8_t *resultbuf, size_t result_len)
{
    return gsmdev->driver->http_post(gsmdev, url, data, data_len, resultbuf, result_len);
}

extern const gsm_driver_t gsm_driver_mc60;
extern const gsm_driver_t gsm_driver_sim8xx;

typedef struct {
    const char *id;
    const char *name;
    const gsm_driver_t *driver;
} gsm_device_id_t;

static const gsm_device_id_t _device_ids[] = {
    { "Quectel_MC60", "Quectel MC60", &gsm_driver_mc60 },
    { "SIMTEL", "Simtel SIM8xx", &gsm_driver_sim8xx },
};

static const unsigned _device_ids_numof = sizeof(_device_ids)/sizeof(gsm_device_id_t);

static int gsm_set_driver(gsm_t *gsmdev)
{
    char buf[64];
    at_dev_t *at_dev = &gsmdev->at_dev;

    int res = at_send_cmd_get_lines(at_dev, "ATI", buf, sizeof(buf), GSM_SERIAL_TIMEOUT);
    if (res > 0) {
        for (unsigned i = 0; i < _device_ids_numof; i++) {
            const gsm_device_id_t *entry = &_device_ids[i];
            if (strstr(buf, entry->id)) {
                gsmdev->driver = entry->driver;
                LOG_INFO("gsm: using driver %s\n", entry->name);
                return 0;
            }
        }
    }

    if (!gsmdev->driver) {
        LOG_WARNING("gsm: no driver found\n");
        res = -1;
    }

    return res;
}
