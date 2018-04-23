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
 * @brief       Over the air (OTA) update module
 *
 * This module provides an API to write an firmware to flash.
 *
 * The API is similar to stream hashing functions:
 *
 * 1. initialize state structure using firmware_update_init()
 * 2. put some data using firmware_update_putbytes()
 * 3. repeat 2. until all data has been received
 * 4. finish update using firmware_update_finish()
 *
 * The module will *not* automatically reboot after an image has been
 * successfully written.
 *
 * Under the hood, the module tries to abstract image writing to flash.
 * Verification of the image is left to the caller.
 * If the data is not correctly written, firmware_put_bytes() will
 * return -1.
 *
 * The module makes sure that at no point in time an invalid image is bootable.
 * The algorithm for that makes use of the bootloader verifying checksum and
 * works as follows:
 *
 * 1. erase first block (making its checksum invalid)
 * 2. write image
 * 3. write first block
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#ifndef FIRMWARE_UPDATE_H
#define FIRMWARE_UPDATE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "firmware.h"
#include "periph/flashpage.h"

/**
 * @brief   firmware update state structure
 */
typedef struct {
    int target_slot;                        /**< update targets this slot     */
    size_t offset;                          /**< update is at this position   */
    unsigned flashpage;                     /**< update is at this flashpage  */
    uint8_t flashpage_buf[FLASHPAGE_SIZE];  /**< flash writing buffer         */
} firmware_update_t;

/**
 * @brief   Initialize firmware update
 *
 * @param[in/out]   state       ptr to preallocated state structure
 * @param[in]       target_slot slot to write update into
 *
 * @returns         0 on success, <0 otherwise
 */
int firmware_update_init(firmware_update_t *state, int target_slot);

/**
 * @brief   Feed bytes into the firmware writer
 *
 * @param[in/out]   state   ptr to previously used update state
 * @param[in]       offset  offset of @p bytes (from image start)
 * @param[in]       bytes   ptr to data
 * @param[in]       len     len of data
 *
 * @returns         0 on success, <0 otherwise
 */
int firmware_update_putbytes(firmware_update_t *state,
                             const uint8_t *bytes, size_t len);

/**
 * @brief   Finish a firmware update
 *
 * @param[in]   state       ptr to previously used state structure
 * @param[in]   metadata    Metadata to write on the first page
 * @param[in]   len         Size of the metadata in bytes
 *
 * @returns     0 on success, <0 otherwise
 */
int firmware_update_finish(firmware_update_t *state,
        firmware_metadata_t *metadata, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* FIRMWARE_UPDATE_H */
