#include <string.h> /* just for NULL ... */

#include "nanonet.h"

#define ENABLE_DEBUG ENABLE_NANONET_DEBUG
#include "debug.h"

int nanonet_init_dev_eth(int n);

nano_dev_t nanonet_devices[NANO_NUM_DEVS];

void nanonet_init_devices(void)
{
    nanonet_init_dev_eth(NANO_DEV_ETH);
}
