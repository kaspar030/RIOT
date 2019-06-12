#ifndef NANONET_H
#define NANONET_H

#include "event.h"
#include "thread.h"

#include "nano_config.h"
#include "nano_dev.h"
#include "nano_ctx.h"
#include "nano_eth.h"
#include "nano_ieee802154.h"
#include "nano_arp.h"
#include "nano_ipv4.h"
#include "nano_ipv6.h"
#include "nano_6lp.h"
#include "nano_route.h"
#include "nano_udp.h"

void nanonet_init(void);
void nanonet_loop(void);
void nanonet_start_thread(void);

extern uint8_t nanonet_rxbuf[NANONET_RX_BUFSIZE];
extern thread_t *nanonet_thread;
extern event_queue_t nanonet_events;

#endif /* NANONET_H */
