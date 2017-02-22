#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "at.h"
#include "fmt.h"
#include "gsm.h"
#include "log.h"

static ssize_t _sim8xx_http_get(gsm_t *gsmdev, const char *url, uint8_t *resultbuf, size_t len)
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

static ssize_t _sim8xx_http_post(gsm_t *gsmdev,
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

static int sim8xx_cnet_scan(gsm_t *gsmdev, char *buf, size_t len)
{
    LOG_INFO("sim800x: scanning GSM towers...\n");
    mutex_lock(&gsmdev->mutex);

    at_send_cmd_wait_ok(&gsmdev->at_dev, "AT+CNETSCAN=1", GSM_SERIAL_TIMEOUT);
    int res = at_send_cmd_get_resp(&gsmdev->at_dev, "AT+CNETSCAN", buf, len, GSM_SERIAL_TIMEOUT * 120);
    if (res > 0) {
        buf[res-1] = '\0';
    }
    else {
        res = -1;
    }

    mutex_unlock(&gsmdev->mutex);
    return res;
}

static int sim8xx_get_loc(gsm_t *gsmdev, char *lng, char *lat)
{
    char buf[64];
    char *pos = buf;

    mutex_lock(&gsmdev->mutex);
    int res = at_send_cmd_get_resp(&gsmdev->at_dev, "AT+CIPGSMLOC=1,1", buf, sizeof(buf), GSM_SERIAL_TIMEOUT * 30);
    mutex_unlock(&gsmdev->mutex);

    if ((res > 2) && strncmp(buf, "+CIPGSMLOC",10) == 0) {
        pos += 12;
        while (*pos != ',') {
            if (!*pos) return -1;
            pos++;
        }
        pos++;

        while (*pos != ',') {
            if (!*pos) return -1;
            *lng++ = *pos++;
        }
        pos++;
        *lng = 0;

        while (*pos != ',') {
            if (!*pos) return -1;
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

const gsm_driver_t gsm_driver_sim8xx = {
    .get_loc=sim8xx_get_loc,
    .cnet_scan=sim8xx_cnet_scan,
    .http_get=_sim8xx_http_get,
    .http_post=_sim8xx_http_post,
};
