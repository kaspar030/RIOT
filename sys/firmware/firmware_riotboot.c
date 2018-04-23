/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *                    Inria
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
 * @brief       RIOTBoot specific firmware module
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 *
 * @}
 */

#include <string.h>

#ifdef RIOT_VERSION
#include "log.h"
#include "ota_pubkey.h"
#else
#include <stdio.h>
#define LOG_INFO(...) printf(__VA_ARGS__)
#endif

#include "firmware.h"
#include "firmware/riotboot.h"

static inline size_t min(size_t a, size_t b)
{
    return a <= b ? a : b;
}

void firmware_riotboot_print(firmware_riotboot_t *riotboot)
{
    printf("Firmware magic_number: 0x%08x\n", (unsigned)riotboot->metadata.magic_number);
    printf("Firmware Version: %#x\n", (unsigned)riotboot->metadata.version);
    printf("Firmware start address: 0x%08x\n", (unsigned)riotboot->metadata.start_addr);
    if (riotboot->metadata.metadata_type == FIRMWARE_METADATA_RIOTBOOT)
    {
        printf("Firmware APPID: %#x\n", (unsigned)riotboot->appid);
        printf("Firmware Size: %" PRIu32 "\n", riotboot->size);
        printf("Firmware HASH: ");
        for (unsigned long i = 0; i < sizeof(riotboot->hash); i++) {
            printf("%02x ", riotboot->hash[i]);
        }
        printf("\n");
        printf("Firmware chksum: 0x%08x\n", (unsigned)riotboot->metadata.chksum);
        printf("Firmware signature: ");
        for (unsigned long i = 0; i < sizeof(riotboot->sig); i++) {
            printf("%02x ", riotboot->sig[i]);
        }
        printf("\n");
    }
    else {
        printf("Firmware of unknown type\n");
    }
}

int firmware_riotboot_validate_signature(firmware_riotboot_t *riotboot, const unsigned char *pk)
{
    if (firmware_validate_metadata_checksum(&riotboot->metadata)) {
        return -1;
    }

    unsigned char sm[FIRMWARE_SIGN_BYTES + crypto_sign_BYTES];
    memcpy(sm, ((unsigned char *)riotboot) + FIRMWARE_SIGN_BYTES, crypto_sign_BYTES);
    memcpy(sm + crypto_sign_BYTES, riotboot, FIRMWARE_SIGN_BYTES);

    unsigned char m[FIRMWARE_SIGN_BYTES + crypto_sign_BYTES];
    unsigned long long mlen;
    int res = crypto_sign_open(m, &mlen, sm, FIRMWARE_SIGN_BYTES + crypto_sign_BYTES, pk);
    if (res) {
        LOG_INFO("%s: RIOTboot metadata signature invalid\n", __func__);
    }
    return res;
}

int firmware_riotboot_sign(firmware_riotboot_t *riotboot, unsigned char *sk)
{
    unsigned char sm[FIRMWARE_SIGN_BYTES + crypto_sign_BYTES];
    unsigned long long smlen;

    crypto_sign(sm, &smlen, (unsigned char *)riotboot, FIRMWARE_SIGN_BYTES, sk);
    memcpy(riotboot->sig, sm, crypto_sign_BYTES);
    return 0;
}

#ifdef RIOT_VERSION
ssize_t firmware_riotboot_recv_metadata(firmware_riotboot_update_t *state, uint8_t *buf, size_t len)
{
    size_t to_copy = min(len, FIRMWARE_METADATA_SIZE - state->update.offset);
    /* copy metadata from packet to the firmware update struct */
    memcpy(state->m.metadata_buf + state->update.offset,
           buf,
           to_copy);
    if ((state->update.offset + to_copy) >= 256) {
        /* Full metadata received */
        firmware_riotboot_t *metadata = &state->m.metadata;
        LOG_INFO("ota: verifying metadata ...\n");
        if (metadata->metadata.start_addr != firmware_get_image_startaddr(state->update.target_slot)) {
            LOG_WARNING("ota: start address doesn't match selected slot. Aborting.\n");
            LOG_WARNING("ota: (image start=%p slot start=%p)\n", (void *)metadata->metadata.start_addr, \
                        (void *)firmware_get_image_startaddr(state->update.target_slot));
            state->state = FIRMWARE_UPDATE_IDLE;
            return -1;
        }

        /* check metadata magic nr, checksum and signature */
        if (firmware_riotboot_validate_signature(metadata, ota_public_key)) {
            LOG_WARNING("ota: verification failed!\n");
            state->state = FIRMWARE_UPDATE_IDLE;
            return -1;
        }
        else {
            state->state = FIRMWARE_UPDATE_VERIFIED;
            LOG_INFO("ota: verification successful\n");
        }
    }
    return to_copy;
}
#endif
