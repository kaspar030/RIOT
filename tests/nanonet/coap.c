#include <coap.h>
#include <string.h>

#include "nanonet.h"

#define ENABLE_DEBUG 1
#include "debug.h"

#define BUFSZ 1000
#define MAX_RESPONSE_LEN 500
static uint8_t response[MAX_RESPONSE_LEN] = {0};

uint8_t scratch_raw[BUFSZ];
coap_rw_buffer_t scratch_buf = {scratch_raw, sizeof(scratch_raw)};

static int handle_get_well_known_core(coap_rw_buffer_t *scratch, const coap_packet_t *inpkt, coap_packet_t *outpkt, uint8_t id_hi, uint8_t id_lo);

static int handle_get_riot_board(coap_rw_buffer_t *scratch, const coap_packet_t *inpkt,
        coap_packet_t *outpkt, uint8_t id_hi, uint8_t id_lo)
{
    const char *riot_name = RIOT_BOARD;
    int len = strlen(RIOT_BOARD);
    memcpy(response, riot_name, len);

    return coap_make_response(scratch, outpkt, (const uint8_t *)response, len, id_hi, id_lo, &inpkt->tok, COAP_RSPCODE_CONTENT, COAP_CONTENTTYPE_TEXT_PLAIN);
}

static const coap_endpoint_path_t path_well_known_core = {2, {".well-known", "core"}};
static const coap_endpoint_path_t path_riot_board = {2, {"riot", "board"}};
//static const coap_endpoint_path_t path_riot_mcu = {2, {"riot", "mcu"}};

const coap_endpoint_t endpoints[] =
{
    {COAP_METHOD_GET, handle_get_well_known_core, &path_well_known_core, "ct=40"},
    {COAP_METHOD_GET, handle_get_riot_board, &path_riot_board, "ct=0"},
//    {COAP_METHOD_GET, handle_get_riot_mcu, &path_riot_mcu, "ct=0"},
    {(coap_method_t)0, NULL, NULL, NULL} /* marks the end of the endpoints array */
};

static int handle_get_well_known_core(coap_rw_buffer_t *scratch, const coap_packet_t *inpkt, coap_packet_t *outpkt, uint8_t id_hi, uint8_t id_lo)
{
    char *rsp = (char*)response;
    int len = sizeof(response);
    const coap_endpoint_t *ep = endpoints;
    int i;

    len--; // Null-terminated string

    while(NULL != ep->handler)
    {
        if (NULL == ep->core_attr) {
            ep++;
            continue;
        }

        if (0 < strlen(rsp)) {
            strncat(rsp, ",", len);
            len--;
        }

        strncat(rsp, "<", len);
        len--;

        for (i = 0; i < ep->path->count; i++) {
            strncat(rsp, "/", len);
            len--;

            strncat(rsp, ep->path->elems[i], len);
            len -= strlen(ep->path->elems[i]);
        }

        strncat(rsp, ">;", len);
        len -= 2;

        strncat(rsp, ep->core_attr, len);
        len -= strlen(ep->core_attr);

        ep++;
    }

    return coap_make_response(scratch, outpkt, (const uint8_t *)rsp, strlen(rsp), id_hi, id_lo, &inpkt->tok, COAP_RSPCODE_CONTENT, COAP_CONTENTTYPE_APPLICATION_LINKFORMAT);
}

int nano_coap_handler(nano_ctx_t *ctx, size_t offset) {
    DEBUG("nano_coap_handler\n");
    int rc;

    int n = ctx->len-offset;
    uint8_t *buf = (uint8_t*)(ctx->buf+offset);
    coap_packet_t pkt;
    printf("Received packet: ");
    coap_dump(buf, n, true);
    printf("\n");

    if (0 != (rc = coap_parse(&pkt, buf, n))) {
        printf("Bad packet rc=%d\n", rc);
    }
    else {
        uint8_t *rspbuf = (uint8_t*) (ctx->buf+offset);
        size_t rsplen = NANONET_RX_BUFSIZE - offset;

        coap_packet_t rsppkt;
        printf("content:\n");
        coap_dumpPacket(&pkt);
        coap_handle_req(&scratch_buf, &pkt, &rsppkt);

        if (0 != (rc = coap_build(rspbuf, &rsplen, &rsppkt)))
            printf("coap_build failed rc=%d\n", rc);
        else
        {
            printf("Sending packet: ");
            coap_dump(rspbuf, rsplen, true);
            printf("\n");
            printf("content:\n");
            coap_dumpPacket(&rsppkt);

            /* update length in nanonet context */
            ctx->len += rsplen - n;
            udp_reply(ctx);
        }
    }

    return 0;
}
