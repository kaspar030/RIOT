#ifndef CC26XX_RFCORE_H
#define CC26XX_RFCORE_H

#include <stdint.h>

#include "cc2650_radio.h"

void cc26xx_init(void);

void cc2650RadioInit(void);
unsigned cc26xx_rfcore_enable(void);

unsigned cc26xx_rfcore_rx_start(void);
unsigned cc26xx_rfcore_rx_stop(void);

void cc26xx_rfcore_irq_set_handler(void(*handler)(void *), void * arg);
void cc26xx_rfcore_irq_rx_ok_enable(void);
void cc26xx_rfcore_irq_rx_ok_disable(void);

void cc26xx_rfcore_irq_tx_done_enable(void);
void cc26xx_rfcore_irq_tx_done_disable(void);

void cc2650RadioProcessReceiveQueue(void);

void cc26xx_rfcore_get_ieee_eui64(uint8_t *aIeeeEui64);

void cc26xx_rfcore_set_pan(uint16_t aPanid);
void cc26xx_rfcore_set_chan(uint16_t channel);
uint16_t cc26xx_rfcore_get_pan(void);
uint8_t cc26xx_rfcore_get_chan(void);
void cc26xx_rfcore_set_promiscuous(bool enable);
bool cc26xx_rfcore_get_promiscuous(void);

unsigned cc26xx_get_flags(void);

int cc26xx_rfcore_recv_avail(void);
int cc26xx_rfcore_recv(void *buf, size_t len, netdev_ieee802154_rx_info_t *rx_info);
int cc26xx_rfcore_send(const iolist_t *iolist);

unsigned cc26xx_rfcore_irq_is_enabled(unsigned irq);
void cc26xx_rfcore_irq_enable(unsigned irq);
void cc26xx_rfcore_irq_disable(unsigned irq);

extern volatile cc2650_PhyState cc26xx_rfcore_state;

#endif /* CC26XX_RFCORE_H */
