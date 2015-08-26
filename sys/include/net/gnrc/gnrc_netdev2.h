/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   net_gnrc
 * @{
 *
 * @file
 * @brief     netdev2 gnrc glue code interface
 *
 * @author    Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef GNRC_NETDEV2_H
#define GNRC_NETDEV2_H

#include "kernel_types.h"
#include "net/netdev2.h"
#include "net/gnrc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NETDEV2_MSG_TYPE_EVENT 0x1234

typedef struct gnrc_netdev2 gnrc_netdev2_t;


/**
 * @brief Structure holding gnrc netdev2 adapter state
 */
struct gnrc_netdev2 {
    int (*send)(gnrc_netdev2_t *dev, gnrc_pktsnip_t *snip);
    gnrc_pktsnip_t * (*recv)(gnrc_netdev2_t *dev);
    netdev2_t *dev;
    kernel_pid_t pid;
};

/**
 * @brief Initialize gnrc netdev2 handler thread
 *
 * @param[in] stack         ptr to preallocated stack buffer
 * @param[in] stacksize     size of stack buffer
 * @param[in] priority      priority of thread
 * @param[in] name          name of thread
 * @param[in] gnrc_netdev2  ptr to netdev2 device to handle in created thread
 *
 * @return pid of created thread
 * @return KERNEL_PID_UNDEF on error
 */
kernel_pid_t gnrc_netdev2_init(char *stack, int stacksize, char priority,
                        const char *name, gnrc_netdev2_t *gnrc_netdev2);

#ifdef __cplusplus
}
#endif

#endif /* GNRC_NETDEV2_H */
/** @} */
