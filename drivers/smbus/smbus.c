#include "smbus.h"

int smbus_init(const smbus_t *dev)
{
    return i2c_init_master(dev->i2c, I2C_SPEED_NORMAL);
}
