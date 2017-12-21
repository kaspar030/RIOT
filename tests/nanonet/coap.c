/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdio.h>
#include <string.h>

#include "net/nanocoap.h"
#include "nanonet.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static ssize_t _riot_board_handler(coap_pkt_t *pkt, uint8_t *buf, size_t len)
{
    return coap_reply_simple(pkt, COAP_CODE_205, buf, len,
                             COAP_FORMAT_TEXT, (uint8_t *)RIOT_BOARD, strlen(RIOT_BOARD));
}

const coap_resource_t coap_resources[] = {
    COAP_WELL_KNOWN_CORE_DEFAULT_HANDLER,
    { "/riot/board", COAP_GET, _riot_board_handler },
};

const unsigned coap_resources_numof = sizeof(coap_resources) / sizeof(coap_resources[0]);

int nano_coap_handler(nano_ctx_t *ctx, size_t offset)
{
    ssize_t res;
    int n = ctx->len - offset;
    uint8_t *buf = (uint8_t *)(ctx->buf + offset);

    coap_pkt_t pkt;

    if (coap_parse(&pkt, (uint8_t *)buf, n) < 0) {
        DEBUG("error parsing packet\n");
    }
    else if ((res = coap_handle_req(&pkt, buf, NANONET_RX_BUFSIZE - offset)) > 0) {
        /* update length in nanonet context */
        ctx->len += res - n;
        udp_reply(ctx);
    }
    return 0;
}
