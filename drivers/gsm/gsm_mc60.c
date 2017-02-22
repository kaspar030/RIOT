#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "at.h"
#include "fmt.h"
#include "gsm.h"
#include "log.h"

static ssize_t _mc60_send_data(gsm_t *gsmdev, const char *cmd, const uint8_t *data, size_t data_len, uint32_t timeout)
{
    char buf[32];

    at_dev_t *at_dev = &gsmdev->at_dev;
    int res = at_send_cmd_get_resp(at_dev, cmd, buf, sizeof(buf), timeout);
    if (res < 0) {
        goto out;
    }

    if (strncmp(buf, "CONNECT", 7)) {
        res = -1;
        goto out;
    }

    /* send DATA */
    uart_write(at_dev->uart, data, data_len);

    /* read empty line */
    at_readline(at_dev, buf, sizeof(buf), timeout);

    res = at_readline(at_dev, buf, sizeof(buf), timeout);
    if (res <= 0 || strncmp(buf, "OK", 2)) {
        res = -1;
        goto out;
    }

out:
    return res;
}

static ssize_t _mc60_http_post(gsm_t *gsmdev,
                         const char *url,
                         const uint8_t *data, size_t data_len,
                         uint8_t *resultbuf, size_t result_len)
{
    int res;
    at_dev_t *at_dev = &gsmdev->at_dev;

    res = at_send_cmd_wait_ok(at_dev, "AT+QSSLCFG=\"https\",0", GSM_SERIAL_TIMEOUT);
    if (res) {
        goto out;
    }

    if (strncmp(url, "https://", 8) == 0) {
        const char *ssl_cmds[] = {
            "AT+QSSLCFG=\"sslversion\",1,4",
            "AT+QSSLCFG=\"seclevel\",1,0",
            "AT+QSSLCFG=\"ciphersuite\",1,\"0XFFFF\"",
            "AT+QSSLCFG=\"https\",1",
            "AT+QSSLCFG=\"httpsctxi\",1",
        };

        const unsigned ncmds = sizeof(ssl_cmds)/sizeof(char*);

        for (unsigned i = 0; i < ncmds; i++) {
            res = at_send_cmd_wait_ok(at_dev, ssl_cmds[i], GSM_SERIAL_TIMEOUT);
            if (res) {
                goto out;
            }
        }
    }

    unsigned url_len = fmt_strlen(url);
    char buf[128];
    char *pos = buf;
    pos += fmt_str(pos, "AT+QHTTPURL=");
    pos += fmt_u32_dec(pos, url_len);
    pos += fmt_str(pos, ",30");
    *pos = '\0';

    res = _mc60_send_data(gsmdev, buf, (uint8_t*)url, url_len, GSM_SERIAL_TIMEOUT);
    if (res < 0) {
        goto out;
    }

    if (data && data_len) {
        /* POST */
        char *pos = buf;
        pos += fmt_str(pos, "AT+QHTTPPOST=");
        pos += fmt_u32_dec(pos, data_len);
        pos += fmt_str(pos, ",50,50");
        *pos = '\0';
        res = _mc60_send_data(gsmdev, buf, data, data_len, GSM_SERIAL_TIMEOUT * 30);
        if (res < 0) {
            goto out;
        }
    }
    else {
        /* GET */
        res = at_send_cmd_wait_ok(at_dev, "AT+QHTTPGET=60", GSM_SERIAL_TIMEOUT * 30);
        if (res < 0) {
            goto out;
        }
    }

    res = at_send_cmd_get_lines(at_dev, "AT+QHTTPREAD=30", (char *)resultbuf, result_len, GSM_SERIAL_TIMEOUT * 30);
    if (res < 11) {
        res = -1;
        goto out;
    }

    res -= 11;

    /* resultbuf contains "CONNECT\n<data>OK\n". So copy data to beginning of
     * buf, and cut off "OK\n". */
    memcpy(resultbuf, resultbuf + 8, res);
    resultbuf[res] = '\0';

out:
    mutex_unlock(&gsmdev->mutex);

    return res;
}

static ssize_t _mc60_http_get(gsm_t *gsmdev, const char *url, uint8_t *resultbuf, size_t len)
{
    return _mc60_http_post(gsmdev, url, NULL, 0, resultbuf, len);
}

static int mc60_get_loc(gsm_t *gsmdev, char *lon, char *lat)
{
    char buf[64];
    char *pos = buf;

    mutex_lock(&gsmdev->mutex);
    int res = at_send_cmd_get_resp(&gsmdev->at_dev, "AT+QCELLLOC=1", buf, sizeof(buf), GSM_SERIAL_TIMEOUT * 30);
    mutex_unlock(&gsmdev->mutex);

    if ((res > 2) && strncmp(buf, "+QCELLLOC: ", 11) == 0) {
        pos += 11;
        while (*pos != ',') {
            if (!*pos) return -1;
            *lon++ = *pos++;
        }
        pos++;
        *lon = 0;

        while (*pos != ',') {
            if (!*pos) break;
            *lat++ = *pos++;
        }
        pos++;
        *lat = 0;

        res = 0;
    }
    else {
        res = -1;
    }

    return res;
}

static int mc60_cnet_scan(gsm_t *gsmdev, char *buf, size_t len)
{
    LOG_INFO("gsm: mc60: scanning GSM towers...\n");
    mutex_lock(&gsmdev->mutex);

    int res = at_send_cmd_get_lines(&gsmdev->at_dev, "AT+QOPS?", buf, len, GSM_SERIAL_TIMEOUT * 120);
    if (res > 0) {
        buf[res-1] = '\0';
    }
    else {
        res = -1;
    }

    mutex_unlock(&gsmdev->mutex);
    return res;
}

const gsm_driver_t gsm_driver_mc60 = {
    .get_loc=mc60_get_loc,
    .cnet_scan=mc60_cnet_scan,
    .http_get=_mc60_http_get,
    .http_post=_mc60_http_post,
};
