#ifndef NANONET_H
#define NANONET_H

#include "event.h"
#include "thread.h"

#include "net/nano/config.h"

void nanonet_init(void);
void nanonet_loop(void);
void nanonet_start_thread(void);

extern uint8_t nanonet_rxbuf[NANONET_RX_BUFSIZE];
extern thread_t *nanonet_thread;
extern event_queue_t nanonet_events;

#endif /* NANONET_H */
