/*
 * Copyright (C) 2016-17 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_nanocoap
 * @{
 *
 * @file
 * @brief       Nanocoap sock helpers
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <errno.h>
#include <string.h>
#include <stdio.h>

#include "net/nanocoap_sock.h"
#include "net/sock/udp.h"

#define ENABLE_DEBUG (1)
#include "debug.h"

static ssize_t _nanocoap_request(sock_udp_t *sock, coap_pkt_t *pkt, size_t len)
{
    ssize_t res;
    size_t pdu_len = (pkt->payload - (uint8_t *)pkt->hdr) + pkt->payload_len;
    uint8_t *buf = (uint8_t*)pkt->hdr;

    /* TODO: timeout random between between ACK_TIMEOUT and (ACK_TIMEOUT *
     * ACK_RANDOM_FACTOR) */
    uint32_t timeout = COAP_ACK_TIMEOUT * US_PER_SEC;
    unsigned tries_left = COAP_MAX_RETRANSMIT + 1;  /* add 1 for initial transmit */
    while (tries_left) {

        res = sock_udp_send(sock, buf, pdu_len, NULL);
        if (res <= 0) {
            DEBUG("nanocoap: error sending coap request, %d\n", (int)res);
            break;
        }

        res = sock_udp_recv(sock, buf, len, timeout, NULL);
        if (res <= 0) {
            if (res == -ETIMEDOUT) {
                DEBUG("nanocoap: timeout\n");

                timeout *= 2;
                tries_left--;
                if (!tries_left) {
                    DEBUG("nanocoap: maximum retries reached\n");
                }
                continue;
            }
            DEBUG("nanocoap: error receiving coap response, %d\n", (int)res);
            break;
        }
        else {
            if (coap_parse(pkt, (uint8_t *)buf, res) < 0) {
                DEBUG("nanocoap: error parsing packet\n");
                res = -EBADMSG;
            }
            break;
        }
    }

    return res;
}

ssize_t nanocoap_request(coap_pkt_t *pkt, sock_udp_ep_t *local, sock_udp_ep_t *remote, size_t len)
{
    sock_udp_t sock;

    if (!remote->port) {
        remote->port = COAP_PORT;
    }

    int res = sock_udp_create(&sock, local, remote, 0);
    if (res < 0) {
        return res;
    }

    res = _nanocoap_request(&sock, pkt, len);

    sock_udp_close(&sock);

    return res;
}

ssize_t nanocoap_get(sock_udp_ep_t *remote, const char *path, uint8_t *buf, size_t len)
{
    ssize_t res;
    coap_pkt_t pkt;
    uint8_t *pktpos = buf;

    pkt.hdr = (coap_hdr_t*)buf;
    pktpos += coap_build_hdr(pkt.hdr, COAP_TYPE_CON, NULL, 0, COAP_METHOD_GET, 1);
    pktpos += coap_opt_put_uri_path(pktpos, 0, path);
    pkt.payload = pktpos;
    pkt.payload_len = 0;

    res = nanocoap_request(&pkt, NULL, remote, len);
    if (res < 0) {
        return res;
    }
    else {
        res = coap_get_code(&pkt);
        if (res != 205) {
            res = -res;
        }
        else {
            if (pkt.payload_len) {
                memmove(buf, pkt.payload, pkt.payload_len);
            }
            res = pkt.payload_len;
        }
    }
    return res;
}

static int _fetch_block(coap_pkt_t *pkt, uint8_t *buf, sock_udp_t *sock, const char *path, coap_blksize_t blksize, size_t num)
{
    uint8_t *pktpos = buf;
    pkt->hdr = (coap_hdr_t *)buf;

    pktpos += coap_build_hdr(pkt->hdr, COAP_TYPE_CON, NULL, 0, COAP_METHOD_GET, num);
    pktpos += coap_opt_put_uri_path(pktpos, 0, path);
    pktpos += coap_put_option_block(pktpos, COAP_OPT_URI_PATH, num, blksize, 0, COAP_OPT_BLOCK2);

    pkt->payload = pktpos;
    pkt->payload_len = 0;

    int res = _nanocoap_request(sock, pkt, 64 + (0x1 << (blksize + 4)));
    if (res < 0) {
        return res;
    }

    res = coap_get_code(pkt);
    DEBUG("code=%i\n", res);
    if (res != 205) {
        return -res;
    }

    return 0;
}

int nanocoap_get_blockwise(sock_udp_ep_t *remote, const char *path,
                               coap_blksize_t blksize,
                               coap_blockwise_cb_t callback, void *arg)
{
    /* mmmmh dynamically sized array */
    uint8_t buf[64 + (0x1 << (blksize + 4))];
    sock_udp_ep_t local = SOCK_IPV6_EP_ANY;
    coap_pkt_t pkt;

    sock_udp_t sock;
    int res = sock_udp_create(&sock, &local, remote, 0);
    if (res < 0) {
        return res;
    }


    int more = 1;
    size_t num = 0;
    res = -1;
    while (more == 1) {
        DEBUG("fetching block %u\n", (unsigned)num);
        res = _fetch_block(&pkt, buf, &sock, path, blksize, num);
        DEBUG("res=%i\n", res);

        if (!res) {
            coap_block1_t block2;
            coap_get_block2(&pkt, &block2);
            more = block2.more;
            printf("more=%i\n", more);

            if (callback(arg, block2.offset, pkt.payload, pkt.payload_len, more)) {
                DEBUG("callback res != 0, aborting.\n");
                res = -1;
                goto out;
            }
        }
        else {
            DEBUG("error fetching block\n");
            res = -1;
            goto out;
        }

        num += 1;
    }

out:
    sock_udp_close(&sock);
    return res;
}

int nanocoap_get_blockwise_url(const char *url,
                               coap_blksize_t blksize,
                               coap_blockwise_cb_t callback, void *arg)
{
    char hostport[SOCK_HOSTPORT_MAXLEN];
    char urlpath[SOCK_URLPATH_MAXLEN];
    sock_udp_ep_t remote;

    if (strncmp(url, "coap://", 7)) {
        puts("no coap");
        return -EINVAL;
    }

    if (sock_urlsplit(url, hostport, urlpath) < 0) {
        puts("urlsplit");
        return -EINVAL;
    }

    if (sock_udp_str2ep(&remote, hostport) < 0) {
        puts("str2ep");
        return -EINVAL;
    }

    if (!remote.port) {
        remote.port = COAP_PORT;
    }

    return nanocoap_get_blockwise(&remote, urlpath, blksize, callback, arg);
}

typedef struct {
    size_t offset;
    uint8_t *ptr;
    size_t len;
} _buf_t;

static int _2buf(void *arg, size_t offset, uint8_t *buf, size_t len, int more)
{
    (void)more;

    _buf_t *_buf = arg;
    if (_buf->offset != offset) {
        return 0;
    }
    if (len > _buf->len) {
        return -1;
    }
    else {
        memcpy(_buf->ptr, buf, len);
        _buf->offset += len;
        _buf->ptr += len;
        _buf->len -= len;
        return 0;
    }
}

int nanocoap_get_blockwise_url_buf(const char *url,
                               coap_blksize_t blksize,
                               uint8_t *buf, size_t len)
{
    _buf_t _buf = { .ptr=buf, .len=len };
    return nanocoap_get_blockwise_url(url, blksize, _2buf, &_buf);
}

int nanocoap_server(sock_udp_ep_t *local, uint8_t *buf, size_t bufsize)
{
    sock_udp_t sock;
    sock_udp_ep_t remote;

    if (!local->port) {
        local->port = COAP_PORT;
    }

    ssize_t res = sock_udp_create(&sock, local, NULL, 0);
    if (res != 0) {
        return -1;
    }

    while (1) {
        res = sock_udp_recv(&sock, buf, bufsize, -1, &remote);
        if (res < 0) {
            DEBUG("error receiving UDP packet %d\n", (int)res);
        }
        else if (res > 0) {
            coap_pkt_t pkt;
            if (coap_parse(&pkt, (uint8_t *)buf, res) < 0) {
                DEBUG("error parsing packet\n");
                continue;
            }
            if ((res = coap_handle_req(&pkt, buf, bufsize)) > 0) {
                res = sock_udp_send(&sock, buf, res, &remote);
            }
            else {
                DEBUG("error handling request %d\n", (int)res);
            }
        }
    }

    return 0;
}
