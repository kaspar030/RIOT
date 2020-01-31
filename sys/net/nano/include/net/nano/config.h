#ifndef NANO_CONFIG_H
#define NANO_CONFIG_H

#ifndef ENABLE_NANONET_DEBUG
#define ENABLE_NANONET_DEBUG 0
#endif

/* should fit the biggest expected packet */
#ifndef NANONET_RX_BUFSIZE
#define NANONET_RX_BUFSIZE 384
#endif

#ifdef MODULE_NETDEV_ETH
#define NANONET_IPV4
#define NANONET_IPV6
#define NANONET_ETH
#endif

#ifdef MODULE_NETDEV_IEEE802154
#define NANONET_IEEE802154
#define NANONET_6LP
#endif

#ifdef NANONET_6LP
#define NANONET_IPV6
#endif

#endif /* NANO_CONFIG_H */
