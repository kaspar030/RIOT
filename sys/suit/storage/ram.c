/*
 * Copyright (C) 2020 Koen Zandberg
 *               2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_suit_storage
 * @{
 *
 * @file
 * @brief       SUIT ram storage module implementation
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */
#include <string.h>
#include <inttypes.h>

#include "fmt.h"
#include "kernel_defines.h"
#ifndef LOG_LEVEL
#define LOG_LEVEL   LOG_ERROR
#endif
#include "log.h"

#include "suit.h"
#include "suit/storage.h"
#include "suit/storage/ram.h"

static inline suit_storage_ram_t *_get_ram(suit_storage_t *storage)
{
    return container_of(storage, suit_storage_ram_t, storage);
}

static inline const suit_storage_ram_t *_get_ram_const(
    const suit_storage_t *storage)
{
    return container_of(storage, suit_storage_ram_t, storage);
}

static inline suit_storage_region_t *_get_active_region(
    suit_storage_ram_t *ram)
{
    return &ram->regions[ram->active_region];
}

static bool _get_region_by_string(const char *location, uint32_t *val)
{
    /* Matching on .ram.### */
    static const char prefix[] = CONFIG_SUIT_STORAGE_RAM_LOCATION_PREFIX;
    static const size_t prefix_len = sizeof(prefix) - 1;

    /* Check for prefix */
    if (strncmp(prefix, location, prefix_len) == 0 &&
        location[prefix_len] != '\n') {
        /* Advance to the number */
        location += prefix_len;
        /* Check if the rest of the string is a number */
        if (fmt_is_number(location)) {
            /* grab the number */
            *val = scn_u32_dec(location, 5);
            /* Number must be smaller than the number of regions */
            if (*val < CONFIG_SUIT_STORAGE_RAM_REGIONS) {
                return true;
            }
        }
    }

    return false;
}

static int _ram_init(suit_storage_t *storage)
{

    suit_storage_ram_t *ram = _get_ram(storage);

    /* set suit_storage regions information */
    ram->storage.regions = ram->regions;
    ram->storage.regions_num = CONFIG_SUIT_STORAGE_RAM_REGIONS;

    /* init the ram regions */
    for (uint8_t i = 0; i < ram->storage.regions_num; i++) {
        ram->regions[i].location = ram->ram_regions[i].mem;
        ram->regions[i].used = 0;
        ram->regions[i].size = CONFIG_SUIT_STORAGE_RAM_SIZE;
        ram->regions[i].pre = NULL;
        ram->regions[i].post = NULL;
    }
    /* Clear the ram regions */
    memset(ram->ram_regions, 0,
           sizeof(suit_storage_ram_region_t) * CONFIG_SUIT_STORAGE_RAM_REGIONS);
    return SUIT_OK;
}

static int _ram_start(suit_storage_t *storage, const suit_manifest_t *manifest,
                      size_t len)
{
    (void)manifest;
    suit_storage_ram_t *ram = _get_ram(storage);
    suit_storage_region_t *region = _get_active_region(ram);

    if (len > CONFIG_SUIT_STORAGE_RAM_SIZE) {
        return SUIT_ERR_STORAGE_EXCEEDED;
    }

    for (suit_storage_hooks_t *i = region->pre; i != NULL; i = i->next) {
        if (i->cb) {
            i->cb(i->arg);
        }
    }

    region->used = 0;
    return SUIT_OK;
}

static int _ram_write(suit_storage_t *storage, const suit_manifest_t *manifest,
                      const uint8_t *buf, size_t offset, size_t len)
{
    (void)manifest;
    suit_storage_ram_t *ram = _get_ram(storage);
    suit_storage_region_t *region = _get_active_region(ram);

    if (offset + len > CONFIG_SUIT_STORAGE_RAM_SIZE) {
        return SUIT_ERR_STORAGE_EXCEEDED;
    }

    memcpy(&region->location[offset], buf, len);
    region->used += len;
    return SUIT_OK;
}

static int _ram_finish(suit_storage_t *storage, const suit_manifest_t *manifest)
{
    (void)storage;
    (void)manifest;
    return SUIT_OK;
}

static int _ram_install(suit_storage_t *storage,
                        const suit_manifest_t *manifest)
{
    (void)manifest;

    suit_storage_ram_t *ram = _get_ram(storage);
    suit_storage_region_t *region = _get_active_region(ram);

    for (suit_storage_hooks_t *i = region->post; i != NULL; i = i->next) {
        if (i->cb) {
            i->cb(i->arg);
        }
    }
    return SUIT_OK;
}

static int _ram_erase(suit_storage_t *storage)
{
    suit_storage_ram_t *ram = _get_ram(storage);
    suit_storage_region_t *region = _get_active_region(ram);

    memset(region->location, 0, CONFIG_SUIT_STORAGE_RAM_SIZE);
    region->used = 0;
    for (suit_storage_hooks_t *i = region->post; i != NULL; i = i->next) {
        if (i->cb) {
            i->cb(i->arg);
        }
    }

    return SUIT_OK;
}

static int _ram_read(suit_storage_t *storage, uint8_t *buf, size_t offset,
                     size_t len)
{
    suit_storage_ram_t *ram = _get_ram(storage);
    suit_storage_region_t *region = _get_active_region(ram);

    if (offset + len > CONFIG_SUIT_STORAGE_RAM_SIZE) {
        return SUIT_ERR_STORAGE_EXCEEDED;
    }

    memcpy(buf, &region->location[offset], len);

    return SUIT_OK;
}

static int _ram_read_ptr(suit_storage_t *storage,
                         const uint8_t **buf, size_t *len)
{
    suit_storage_ram_t *ram = _get_ram(storage);
    suit_storage_region_t *region = _get_active_region(ram);

    *buf = region->location;
    *len = region->used;
    return SUIT_OK;
}

static bool _ram_has_location(const suit_storage_t *storage,
                              const char *location)
{
    (void)storage;
    uint32_t val;

    return _get_region_by_string(location, &val);
}


static suit_storage_region_t* _ram_get_region(const suit_storage_t *storage,
                            const char *location)
{
    uint32_t val;
    if (_get_region_by_string(location, &val) == true) {
        return &storage->regions[val];
    }
    return NULL;
}

static int _ram_set_active_location(suit_storage_t *storage,
                                    const char *location)
{
    suit_storage_ram_t *ram = _get_ram(storage);
    uint32_t region = 0;

    if (!_get_region_by_string(location, &region)) {
        return -1;
    }

    ram->active_region = region;
    return SUIT_OK;
}

static int _ram_get_seq_no(const suit_storage_t *storage, uint32_t *seq_no)
{
    const suit_storage_ram_t *ram = _get_ram_const(storage);

    *seq_no = ram->sequence_no;
    LOG_INFO("Retrieved sequence number: %" PRIu32 "\n", *seq_no);
    return SUIT_OK;
}

static int _ram_set_seq_no(suit_storage_t *storage, uint32_t seq_no)
{
    suit_storage_ram_t *ram = _get_ram(storage);

    if (ram->sequence_no < seq_no) {
        LOG_INFO("Stored sequence number: %" PRIu32 "\n", seq_no);
        ram->sequence_no = seq_no;
        return SUIT_OK;
    }

    return SUIT_ERR_SEQUENCE_NUMBER;
}

static const suit_storage_driver_t suit_storage_ram_driver = {
    .init = _ram_init,
    .start = _ram_start,
    .write = _ram_write,
    .finish = _ram_finish,
    .read = _ram_read,
    .read_ptr = _ram_read_ptr,
    .install = _ram_install,
    .erase = _ram_erase,
    .has_location = _ram_has_location,
    .get_region = _ram_get_region,
    .set_active_location = _ram_set_active_location,
    .get_seq_no = _ram_get_seq_no,
    .set_seq_no = _ram_set_seq_no,
    .separator = CONFIG_SUIT_STORAGE_RAM_LOCATION_SEPARATOR,
};

suit_storage_ram_t suit_storage_ram = {
    .storage = {
        .driver = &suit_storage_ram_driver,
    },
};
