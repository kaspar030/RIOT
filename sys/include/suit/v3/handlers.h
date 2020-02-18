/*
 * Copyright (C) 2019 Koen Zandberg
 *               2019 Kaspar Schleiser <kaspar@schleiser.de>
 *               2019 Inria
 *               2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @ingroup     sys_suit_v3
 * @brief       SUIT v3 manifest handlers
 *
 * @experimental
 *
 * @{
 *
 * @brief       Handler functions for SUIT manifests
 * @author      Koen Zandberg <koen@bergzand.net>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef SUIT_V3_HANDLERS_H
#define SUIT_V3_HANDLERS_H

#include <stddef.h>
#include <stdint.h>

#include "suit/v3/suit.h"
#include "uuid.h"
#include "nanocbor/nanocbor.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name SUIT condition codes
 * @{
 */
#define SUIT_COND_VENDOR_ID     (1)
#define SUIT_COND_CLASS_ID      (2)
#define SUIT_COND_IMAGE_MATCH   (3)
#define SUIT_COND_USE_BEFORE    (4)
#define SUIT_COND_COMPONENT_OFFSET      (5)
#define SUIT_COND_DEVICE_ID            (24)
#define SUIT_COND_IMAGE_NOT_MATCH      (25)
#define SUIT_COND_MIN_BATTERY          (26)
#define SUIT_COND_UPDATE_AUTHZ         (27)
#define SUIT_COND_VERSION              (28)
/* @} */

/**
 * @name SUIT directive codes
 * @{
 */
#define SUIT_DIR_SET_COMPONENT_IDX  (12)
#define SUIT_DIR_SET_DEPENDENCY_IDX (13)
#define SUIT_DIR_ABORT              (14)
#define SUIT_DIR_TRY_EACH           (15)
#define SUIT_DIR_PROCESS_DEPS       (18)
#define SUIT_DIR_SET_PARAM          (19)
#define SUIT_DIR_OVERRIDE_PARAM     (20)
#define SUIT_DIR_FETCH              (21)
#define SUIT_DIR_COPY               (22)
#define SUIT_DIR_RUN                (23)
#define SUIT_DIR_WAIT               (29)
#define SUIT_DIR_RUN_SEQUENCE       (30)
#define SUIT_DIR_RUN_WITH_ARGS      (31)
#define SUIT_DIR_SWAP               (32)
/** @} */

/**
 * @brief suit handler prototype
 *
 * @param manifest  SUIT v3 manifest context
 * @param it        nanocbor_value_t iterator to the content the handler must handle
 *
 * @return          1 on success
 * @return          negative on error
 */
typedef int (*suit_manifest_handler_t)(suit_v3_manifest_t *manifest, int key, nanocbor_value_t *it);

/**
 * @brief global handler reference
 */
extern const suit_manifest_handler_t suit_global_handlers[];
extern const size_t suit_global_handlers_len;

/**
 * @brief sequence handler reference
 */
extern const suit_manifest_handler_t suit_sequence_handlers[];
extern const size_t suit_sequence_handlers_len;
extern const suit_manifest_handler_t suit_container_handlers[];
extern const size_t suit_container_handlers_len;

int suit_handle_command_sequence(suit_v3_manifest_t *manifest, nanocbor_value_t *bseq,
                                 const suit_manifest_handler_t *handler, size_t handler_len);

#ifdef __cplusplus
}
#endif

#endif /* SUIT_V3_HANDLERS_H */
/** @} */
