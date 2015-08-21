#ifndef GNRC_NETDEV2_H
#define GNRC_NETDEV2_H

#include "kernel_types.h"
#include "net/netdev2.h"
#include "net/gnrc.h"

#define NETDEV2_MSG_TYPE_EVENT 0x1234

typedef struct gnrc_netdev2 gnrc_netdev2_t;

struct gnrc_netdev2 {
    int (*send)(gnrc_netdev2_t *dev, gnrc_pktsnip_t *snip);
    gnrc_pktsnip_t * (*recv)(gnrc_netdev2_t *dev);
    netdev2_t *dev;
    kernel_pid_t pid;
};

kernel_pid_t gnrc_netdev2_init(char *stack, int stacksize, char priority,
                        const char *name, gnrc_netdev2_t *gnrc_netdev2);

#endif /* GNRC_NETDEV2_H */
