#ifndef SMBUS_H
#define SMBUS_H

#include <stdint.h>

#include "periph/i2c.h"

enum {
    SMBUS_BATTERY,
};

typedef struct {
    i2c_t i2c;
    uint8_t addr;
    uint8_t type;
} smbus_t;

int smbus_init(const smbus_t *dev);

#endif /* SMBUS_H */
