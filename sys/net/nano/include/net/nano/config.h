#ifndef NANO_CONFIG_H
#define NANO_CONFIG_H

#ifndef ENABLE_NANONET_DEBUG
#define ENABLE_NANONET_DEBUG 0
#endif

/* should fit the biggest expected packet */
#ifndef NANONET_RX_BUFSIZE
#define NANONET_RX_BUFSIZE 384
#endif

#endif /* NANO_CONFIG_H */
