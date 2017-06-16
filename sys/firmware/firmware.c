#include <string.h>

#include "cpu.h"
#include "firmware.h"

#include "log.h"

const static unsigned _firmware_slot_start[] = {
    0,
    FIRMWARE_SLOT0_SIZE,
    FIRMWARE_SLOT0_SIZE + FIRMWARE_SLOT1_SIZE + sizeof(firmware_metadata_t)
};

const char _canary[4] = "RIOT";

void firmware_metadata_print(firmware_metadata_t *metadata);

int firmware_validate_metadata(firmware_metadata_t *metadata)
{
    LOG_INFO("%s: STUB\n", __func__);
    return (memcmp(_canary, &metadata->canary, 4) == 0) && (metadata->chksum == 0x12345678);
}

int firmware_validate_image(firmware_metadata_t *metadata) {
    LOG_INFO("%s: STUB\n", __func__);
    return 1;
}

void firmware_jump_to_image(firmware_metadata_t *metadata)
{
    unsigned addr = (unsigned)metadata + (unsigned)sizeof(firmware_metadata_t);
    cpu_jump_to_image(addr);
}

int firmware_current_slot(void)
{
    unsigned base_addr = cpu_get_image_baseaddr() - sizeof(firmware_metadata_t);

    for (unsigned i = 0; i < FIRMWARE_NUM_SLOTS; i++) {
        if (base_addr == _firmware_slot_start[i]) {
            return i;
        }
    }

    return -1;
}

firmware_metadata_t *firmware_get_metadata(unsigned slot) {
    assert (slot < FIRMWARE_NUM_SLOTS);
    return (firmware_metadata_t *)_firmware_slot_start[slot];
}

unsigned firmware_get_image_startaddr(unsigned slot)
{
    assert (slot < FIRMWARE_NUM_SLOTS);
    return _firmware_slot_start[slot] + sizeof(firmware_metadata_t);
}

void firmware_jump_to_slot(unsigned slot)
{
    firmware_jump_to_image(firmware_get_metadata(slot));
}

void firmware_dump_slot_addrs(void)
{
    for (unsigned i = 0; i < FIRMWARE_NUM_SLOTS; i++) {
        printf("slot %u: metadata: 0x%08x image: 0x%08x\n", i, \
                (unsigned)_firmware_slot_start[i], firmware_get_image_startaddr(i));
    }
}
