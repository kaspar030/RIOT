/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *                    Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_riotboot_slot
 * @{
 *
 * @file
 * @brief       Slot management functions
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 *
 * @}
 */
#include <string.h>

#include "cpu.h"
#include "riotboot/slot.h"
#include "riotboot/hdr.h"

/*
 * Store the start addresses of each slot.
 * Take into account that CPU_FLASH_BASE represents the starting
 * address of the bootloader, thus the header is located after the
 * space reserved to the bootloader.
 */
const riotboot_hdr_t * const riotboot_slot_slots[] = {
    (riotboot_hdr_t*)(CPU_FLASH_BASE + SLOT0_OFFSET),   /* First slot address -> firmware image */
};

/* Calculate the number of slots */
const unsigned riotboot_slot_num_slots = sizeof(riotboot_slot_slots) / sizeof(riotboot_hdr_t*);

static void _riotboot_slot_jump_to_image(const riotboot_hdr_t *hdr)
{
    cpu_jump_to_image(hdr->start_addr);
}

int riotboot_slot_current_slot(void)
{
    uint32_t base_addr = cpu_get_image_baseaddr();

    for (unsigned i = 0; i < riotboot_slot_num_slots; i++) {
        const riotboot_hdr_t *hdr = riotboot_slot_get_hdr(i);
        if (base_addr == hdr->start_addr) {
            return i;
        }
    }

    return -1;
}

void riotboot_slot_jump(unsigned slot)
{
    _riotboot_slot_jump_to_image(riotboot_slot_get_hdr(slot));
}

uint32_t riotboot_slot_get_image_startaddr(unsigned slot)
{
    return riotboot_slot_get_hdr(slot)->start_addr;
}

void riotboot_slot_dump_addrs(void)
{
    for (unsigned slot = 0; slot < riotboot_slot_num_slots; slot++) {
        const riotboot_hdr_t *hdr = riotboot_slot_get_hdr(slot);

        if (hdr != NULL) {
            printf("slot %u: metadata: %p image: 0x%08" PRIx32 "\n", slot,
                   hdr,
                   hdr->start_addr);
        } else {
            printf("[riotboot_slot]: No riotboot_hdr found at %p\n", hdr);
        }
    }
}

const riotboot_hdr_t *riotboot_slot_get_hdr(unsigned slot)
{
    assert(slot < riotboot_slot_num_slots);

    return riotboot_slot_slots[slot];
}
