/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_rdcli_config CoAP Resource Directory Client Configuration
 * @ingroup     net_rdcli
 * @{
 *
 * @file
 * @brief
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef NET_RDCLI_CONFIG_H
#define NET_RDCLI_CONFIG_H

#include "net/ipv6/addr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Default lifetime in seconds (the default is 1 day)
 */
#ifndef RDCLI_LT
#define RDCLI_LT                (86400UL)
#endif

/**
 * @brief   Delay until the RD client starts to try registering (in seconds)
 */
#ifndef RDCLI_STARTUP_DELAY
#define RDCLI_STARTUP_DELAY     (3U)
#endif

/**
 * @brief   Default client update interval (default is half the lifetime)
 */
#ifndef RDCLI_UPDATE_INTERVAL
#define RDCLI_UPDATE_INTERVAL   (RDCLI_LT / 2)
#endif

/**
 * @name    Endpoint ID definition
 *
 * Per default, the endpoint ID (ep) is generated by concatenation of a user
 * defined prefix (RDCLI_EP_PREFIX) and a locally unique ID (luid) encoded in
 * hexadecimal formatting with the given length of characters
 * (RDCLI_EP_SUFFIX_LEN).
 *
 * Alternatively, the endpoint ID value can be defined at compile time by
 * assigning a string value to the RDCLI_ED macro.
 *
 * @{
 */
#ifndef RDCLI_EP
/**
 * @brief   Number of generated hexadecimal characters added to the ep
 */
#define RDCLI_EP_SUFFIX_LEN     (16)

/**
 * @brief   Default static prefix used for the generated ep
 */
#define RDCLI_EP_PREFIX         "RIOT-"
#endif
/** @} */

/**
 * @brief   Default IPv6 address to use when looking for RDs
 */
#ifndef RDCLI_SERVER_ADDR
#define RDCLI_SERVER_ADDR       IPV6_ADDR_ALL_NODES_LINK_LOCAL
#endif

/**
 * @brief   Default Port to use when looking for RDs
 */
#ifndef RDCLI_SERVER_PORT
#define RDCLI_SERVER_PORT       COAP_PORT
#endif

#ifdef __cplusplus
}
#endif

#endif /* NET_RDCLI_CONFIG_H */
/** @} */
