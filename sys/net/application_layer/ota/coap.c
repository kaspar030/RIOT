/*
 * Copyright (C) 2018 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_firmware
 * @{
 *
 * @file
 * @brief       Firmware update via CoAP implementation
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include "firmware.h"
#include "firmware/update.h"
#include "firmware/riotboot.h"
#include "hashes/sha256.h"
#include "log.h"
#include "net/nanocoap.h"
#include <string.h>

#define ENABLE_DEBUG (0)
#include "debug.h"

static firmware_riotboot_update_t _state;

ssize_t ota_coap_handler(coap_pkt_t *pkt, uint8_t *buf, size_t len, void *context)
{
    (void)context;

    uint32_t result = COAP_CODE_204;
    int res = 0;

    coap_block1_t block1;
    int blockwise = coap_get_block1(pkt, &block1);

    LOG_INFO("ota: received bytes %u-%u",
             (unsigned)block1.offset, (unsigned)block1.offset + pkt->payload_len);

    if (_state.state == FIRMWARE_UPDATE_VERIFIED) {
        unsigned total = _state.m.metadata.size + FIRMWARE_METADATA_SIZE;
        LOG_INFO(" of %u (left=%u)\n", total,
                 total - block1.offset - pkt->payload_len);
    }
    else {
        LOG_INFO("\n");
    }

    if (block1.offset == 0) {
        /* initialize firmware upgrade state struct */
        firmware_update_init(&_state.update, firmware_target_slot());
        _state.state = FIRMWARE_UPDATE_INITIALIZED;
    }

    if (_state.state == FIRMWARE_UPDATE_IDLE) {
        res = -1;
    }
    else {
        if (block1.offset == _state.update.offset) {
            size_t len = pkt->payload_len;
            size_t remaining = len;
            /* copy the first 256 bytes to the firmware_riotboot_t struct and
             * verify */
            if (_state.state != FIRMWARE_UPDATE_VERIFIED) {
                ssize_t processed = firmware_riotboot_recv_metadata(&_state, pkt->payload, len);
                if (processed < 0) {
                    res = processed;
                }
                else {
                    remaining -= processed;
                }
            }
            if (remaining && _state.state == FIRMWARE_UPDATE_VERIFIED) {
                /* TODO: offset */
                res = firmware_update_putbytes(&_state.update,
                        pkt->payload + len - remaining,
                        remaining);
            }
            _state.update.offset += pkt->payload_len;
        }
        else {
            LOG_INFO("coap_ota_handler(): ignoring already received block\n");
            res = 0;
        }
    }

    if (res) {
        result = COAP_CODE_NOT_ACCEPTABLE;
        _state.state = FIRMWARE_UPDATE_IDLE;
        LOG_INFO("coap_ota_handler(): unexpected packet\n");
    }
    else if (block1.more == 1) {
        result = COAP_CODE_231;
    }

    if (!res && (!blockwise || !block1.more)) {
        firmware_riotboot_t *metadata = &_state.m.metadata;
        sha256_init(&_state.sha);
        sha256_update(&_state.sha, (uint8_t*)metadata->metadata.start_addr, metadata->size);
        sha256_final(&_state.sha, _state.hash);
        if (memcmp(_state.hash, metadata->hash, SHA256_DIGEST_LENGTH) != 0) {
            LOG_WARNING("ota: Digest verification failed!\n");
            result = COAP_CODE_BAD_OPTION;
        }
        else if (firmware_update_finish(&_state.update, (firmware_metadata_t*)metadata, sizeof(firmware_riotboot_t)) != 0) {
            result = COAP_CODE_BAD_OPTION;
        }
    }

    ssize_t reply_len = coap_build_reply(pkt, result, buf, len, 0);
    uint8_t *pkt_pos = (uint8_t *)pkt->hdr + reply_len;
    if (!res) {
        pkt_pos += coap_put_block1_ok(pkt_pos, &block1, 0);
    }

    return pkt_pos - (uint8_t *)pkt->hdr;
}
