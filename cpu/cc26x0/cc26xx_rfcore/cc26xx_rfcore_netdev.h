#ifndef CC26XX_RFCORE_NETDEV_H
#define CC26XX_RFCORE_NETDEV_H

#include "net/netdev.h"
#include "net/netdev/ieee802154.h"

typedef struct {
    netdev_ieee802154_t netdev;             /**< netdev parent struct */
} cc26xx_rfcore_t;

void cc26xx_rfcore_setup(cc26xx_rfcore_t *dev);

#endif /* CC26XX_RFCORE_NETDEV_H */
