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
 * @brief       Firmware Metadata Specification
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 * @author      Koen Zandberg <koen@bergzand.net>
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
#define FIRMWARE_CHECKSUM_LEN       (16)

/**
 *  @brief Length of metadata prefixed to firmware binaries
 */
#ifndef FIRMWARE_METADATA_SIZE
#define FIRMWARE_METADATA_SIZE      (256)
#endif

/**
 *  @brief Real length of metadata without padding
 */
#define FIRMWARE_METADATA_REALLEN   (sizeof(firmware_metadata_t) - FIRMWARE_PADDING)

/**
 * @brief Structure to store firmware metadata
 * @{
 */
typedef struct {
    uint32_t magic_number;              /**< metadata magic_number (always "RIOT")  */
    uint32_t version;                   /**< Integer representing firmware version  */
    uint32_t start_addr;                /**< Start address in flash                 */
    uint32_t metadata_type;             /**< Type of metadata, 16 bits type,
                                          16 bit version */
    uint32_t chksum;                    /**< checksum of metadata                   */
} firmware_metadata_t;
/** @} */

/**
 * @brief  Validate FW image metadata
 *
 * @param[in] metadata  ptr to firmware metadata
 *
 */
int firmware_validate_metadata_checksum(const firmware_metadata_t *metadata);

/**
 * @brief  Calculate metadata checksum
 *
 * @param[in] metadata  ptr to firmware metadata
 *
 */
uint32_t firmware_metadata_checksum(const firmware_metadata_t *metadata);

/**
 * @brief  Jump to image
 *
 * @param[in] metadata  ptr to firmware metadata
 *
 */
void firmware_jump_to_image(const firmware_metadata_t *metadata);

/**
 * @brief  Get currently running image slot
 *
 * returns nr of currently active slot
 */
int firmware_current_slot(void);

/**
 * @brief  Get next (to be empty) image slot
 *
 * returns free target slot
 */
int firmware_target_slot(void);

/**
 * @brief  Get metadata of firmware slot
 *
 * @param[in]   slot    slot nr to work on
 *
 * returns metadata of image slot nr @p slot
 */
firmware_metadata_t *firmware_get_metadata(unsigned slot);

/**
 * @brief  Get jump-to address of firmware slot
 *
 * @param[in]   slot    slot nr to work on
 *
 * @returns address of first byte of @p slot
 */
unsigned firmware_get_image_startaddr(unsigned slot);

/**
 * @brief  Boot into image in slot @p slot
 *
 * @param[in]   slot    slot nr to jump to
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
