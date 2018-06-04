#ifndef SMBUS_BATTERY_H
#define SMBUS_BATTERY_H

#include "smbus.h"

int smbus_read_reg(const smbus_t *dev, uint8_t reg, uint8_t *out);
int smbus_battery_voltage(const smbus_t *dev, uint16_t *out);
int smbus_battery_current(const smbus_t *dev, int16_t *out);
int smbus_battery_cycle_count(const smbus_t *dev, uint16_t *out);

#endif /* SMBUS_BATTERY_H */
