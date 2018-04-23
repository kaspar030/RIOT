/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
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
 * @brief       Firmware update helper functions
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <string.h>

#include "firmware/update.h"
#include "log.h"
#include "od.h"

static inline size_t min(size_t a, size_t b)
{
    return a <= b ? a : b;
}

int firmware_update_init(firmware_update_t *firmware_update, int target_slot)
{
    LOG_INFO("ota: initializing update to target slot %i\n",
             target_slot);

    memset(firmware_update, 0, sizeof(firmware_update_t));

    firmware_update->target_slot = target_slot;
    firmware_update->flashpage = flashpage_page(firmware_get_metadata(target_slot));

    return 0;
}

int firmware_update_putbytes(firmware_update_t *state, const uint8_t *bytes, size_t len)
{
    LOG_DEBUG("ota: processing bytes %u-%u\n", state->offset, len);

    while (len) {
        size_t flashpage_pos = state->offset % FLASHPAGE_SIZE;
        size_t flashpage_avail = FLASHPAGE_SIZE - flashpage_pos;

        size_t left = 1;
        size_t to_copy = min(flashpage_avail, len);

        memcpy(state->flashpage_buf + flashpage_pos, bytes, to_copy);
        flashpage_avail -= to_copy;

        state->offset += to_copy;
        flashpage_pos += to_copy;
        bytes += to_copy;
        len -= to_copy;
        left -= to_copy;
        if ((!flashpage_avail) || (!left)) {
            LOG_DEBUG("ota: writing to flashpage %u\n", state->flashpage);
            if (flashpage_write_and_verify(state->flashpage, state->flashpage_buf) != FLASHPAGE_OK) {
                LOG_WARNING("ota: error writing flashpage %u!\n", state->flashpage);
                return -1;
            }
            state->flashpage++;
        }
    }

    return 0;
}

int firmware_update_finish(firmware_update_t *state, firmware_metadata_t *metadata, size_t len)
{
    int res = -1;

    void *slot_start = firmware_get_metadata(state->target_slot);
    size_t metadata_space = (void*)metadata->start_addr - slot_start;

    void *firstpage;

    if (len < FLASHPAGE_SIZE) {
        firstpage = state->flashpage_buf;
        memset(firstpage, 0, metadata_space);
        memcpy(firstpage, metadata, len);
        memcpy(firstpage + metadata_space, \
               slot_start + metadata_space, \
               FLASHPAGE_SIZE - metadata_space);
    }
    else {
        firstpage = metadata;
    }

    int flashpage = flashpage_page(slot_start);
    if (flashpage_write_and_verify(flashpage, firstpage) != FLASHPAGE_OK) {
        LOG_WARNING("ota: re-flashing first block failed!\n");
        goto out;
    }

    LOG_INFO("ota: firmware flashing completed successfully\n");
    res = 0;

out:
    return res;
}
