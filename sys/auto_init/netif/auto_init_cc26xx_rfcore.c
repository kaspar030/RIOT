/*
 * Copyright (C) 2018 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/*
 * @ingroup sys_auto_init_gnrc_netif
 * @{
 *
 * @file
 * @brief   Auto initialization for cc26xx_rfcore network interfaces
 *
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifdef MODULE_CC26XX_RFCORE

#include "log.h"
#include "net/gnrc/netif/ieee802154.h"
#include "net/gnrc.h"

#include "cc26xx_rfcore_netdev.h"

/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define CC26XX_RFCORE_MAC_STACKSIZE     (THREAD_STACKSIZE_DEFAULT)
#ifndef CC26XX_RFCORE_MAC_PRIO
#define CC26XX_RFCORE_MAC_PRIO          (GNRC_NETIF_PRIO)
#endif

static cc26xx_rfcore_t _cc26xx_rfcore;
static char _cc26xx_rfcore_stack[CC26XX_RFCORE_MAC_STACKSIZE];

void auto_init_cc26xx_rfcore(void)
{
        LOG_INFO("[auto_init_netif] initializing cc26xx_rfcore\n");

        cc26xx_rfcore_setup(&_cc26xx_rfcore);

        gnrc_netif_ieee802154_create(_cc26xx_rfcore_stack,
                                     CC26XX_RFCORE_MAC_STACKSIZE,
                                     CC26XX_RFCORE_MAC_PRIO, "cc26xx_rfcore",
                                     (netdev_t *)&_cc26xx_rfcore);

}
#else
typedef int dont_be_pedantic;
#endif /* MODULE_CC26XX_RFCORE */

/** @} */
