#ifndef NANO_CONFIG_H
#define NANO_CONFIG_H

#ifndef ENABLE_NANONET_DEBUG
#define ENABLE_NANONET_DEBUG 0
#endif

/* should fit the biggest expected packet */
#define NANONET_RX_BUFSIZE 256

#ifdef MODULE_NETDEV2_ETH
#define NANONET_IPV4
#define NANONET_ETH
#endif

#endif /* NANO_CONFIG_H */
