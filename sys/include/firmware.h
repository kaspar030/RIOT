/*
 * Copyright (C) 2018 Kaspar Schleiser <kaspar@schleiser.de>
 *                    Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_firmware Firmware Updates
 * @ingroup     sys
 * @{
 *
 * This module contains functions for reading and manipulating firmware metadata
 * as used with the riotboot bootloader.
 *
 * ## Concept
 *
 * The riotboot bootloader expects the flash to be split in three parts: one for
 * the bootloader itself, and two slots for images.
 * Of the latter, one slot holds the running (or latest) image, while the other
 * is used to write a newer image.
 *
 * The bootloader will, on reset, verify the checksum of the image slots, then
 * choose the one with the highest version number that has a valid checksum.  If
 * all slots have the same version number, the first one will be booted. If none
 * of the slots has a valid checksum, no image will be booted and the bootloader
 * will enter "while(1);".
 *
 * The bootloader metadata contains
 *
 * - "RIOT" as magic number
 * - an application version
 * - Start address
 * - a checksum
 *
 * The bootloader only cares about checksum and application version. It expects
 * a running image to cryptographically verify the metadata of a new image.
 *
 * riotboot needs some board specific support, mainly slot size configuration,
 * in order to work for a specific board.
 *
 * @}
 */
#ifndef FIRMWARE_H
#define FIRMWARE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 *  @brief Number of metadata bytes that get checksummed
 *
 *  Includes magic number, version, and start address.
 */
#define FIRMWARE_CHECKSUM_LEN       (12)

/**
 *  @brief Fixed size space for metadata prefixed to firmware binaries
 */
#ifndef FIRMWARE_METADATA_SIZE
#define FIRMWARE_METADATA_SIZE      (256)
#endif

/**
 * @brief Structure to store firmware metadata
 * @{
 */
typedef struct {
    uint32_t magic_number;      /**< metadata magic_number (always "RIOT")  */
    uint32_t version;           /**< Integer representing firmware version  */
    uint32_t start_addr;        /**< Start address in flash                 */
    uint32_t chksum;            /**< checksum of metadata                   */
} firmware_metadata_t;
/** @} */

/**
 * @brief  Validate firmware image metadata
 *
 * Checks both the magic number and the checksum
 *
 * @param[in] metadata  ptr to firmware metadata
 *
 * @returns zero if the @p metadata is valid
 */
int firmware_validate_metadata_checksum(firmware_metadata_t *metadata);

/**
 * @brief  Calculate metadata checksum
 *
 * @param[in] metadata  ptr to firmware metadata
 *
 * @return  The calculated checksum of the @p metadata
 */
uint32_t firmware_metadata_checksum(firmware_metadata_t *metadata);

/**
 * @brief  Jump to image of the metadata struct.
 *
 * Jumps to the start_address of the @p metadata.
 *
 * @param[in] metadata  ptr to firmware metadata
 */
void firmware_jump_to_image(firmware_metadata_t *metadata);

/**
 * @brief  Get currently running image slot
 *
 * @returns number of currently active slot
 */
int firmware_current_slot(void);

/**
 * @brief  Get next (to be empty) image slot
 *
 * @returns free target slot
 */
int firmware_target_slot(void);

/**
 * @brief  Get metadata of firmware slot
 *
 * @param[in]   slot    slot number to work on
 *
 * returns metadata of image slot number @p slot
 */
firmware_metadata_t *firmware_get_metadata(unsigned slot);

/**
 * @brief  Get jump-to address of firmware slot.
 *
 * This function returns the first actual firmware address of the slot, start
 * address of the slot metadata + FIRMWARE_METADATA_SIZE
 *
 * @param[in]   slot    slot number to work on
 *
 * @returns address of first byte of @p slot
 */
unsigned firmware_get_image_startaddr(unsigned slot);

/**
 * @brief  Boot into image in slot @p slot
 *
 * @param[in]   slot    slot number to jump to
 */
void firmware_jump_to_slot(unsigned slot);

/**
 * @brief  Dump firmware slot addresses
 */
void firmware_dump_slot_addrs(void);

/**
 * @brief   Number of configured firmware slots (incl. bootloader slot)
 */
extern const unsigned firmware_num_slots;

#ifdef __cplusplus
}
#endif

#endif /* FIRMWARE_H */
