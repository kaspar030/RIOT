#ifndef NANONET_H
#define NANONET_H

#include "thread.h"

#include "nano_dev.h"
#include "nano_ctx.h"
#include "nano_eth.h"
#include "nano_arp.h"
#include "nano_ipv4.h"
#include "nano_ipv6.h"
#include "nano_route.h"
#include "nano_udp.h"
#include "nano_config.h"

void nanonet_init(void);
void nanonet_loop(void);
void nanonet_start_thread(void);

extern uint8_t nanonet_rxbuf[NANONET_RX_BUFSIZE];
extern thread_t *nanonet_thread;

#endif /* NANONET_H */
