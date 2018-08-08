#include "smbus.h"
#include "smbus/battery.h"

// Info from: Smart Battery Data Specification Revision 1.1
#define MANUFACTURER_ACCESS         0x00
#define REMAINING_CAPACITY_ALARM    0x01
#define REMAINING_TIME_ALARM        0x02
#define BATTERY_MODE            0x03
#define AT_RATE             0x04
#define AT_RATE_TIME_TO_FULL        0x05
#define AT_RATE_TIME_TO_EMPTY       0x06
#define AT_RATE_OK          0x07
#define TEMPERATURE             0x08
#define VOLTAGE             0x09
#define CURRENT             0x0A
#define AVERAGE_CURRENT         0x0B
#define MAX_ERROR           0x0C
#define RELATIVE_STATE_OF_CHARGE    0x0D
#define ABSOLUTE_STATE_OF_CHARGE    0x0E
#define REMAINING_CAPACITY      0x0F
#define FULL_CHARGE_CAPACITY        0x10
#define RUNTIME_TO_EMPTY        0x11
#define AVERAGE_TIME_TO_EMPTY       0x12
#define AVERAGE_TIME_TO_FULL        0x13
#define BATTERY_STATUS          0x16
#define CYCLE_COUNT             0x17
#define DESIGN_CAPACITY         0x18
#define DESIGN_VOLTAGE          0x19
#define SPECIFICATION_INFO      0x1A
#define MANUFACTURE_DATE        0x1B
#define SERIALNUMBER            0x1C
#define MANUFACTURER_NAME       0x20
#define DEVICE_NAME             0x21
#define DEVICE_CHEMISTRY        0x22
#define MANUFACTURER_DATA       0x23

// BATTERYSTATUS
// Alarm Bits
#define OVER_CHARGED_ALARM_BIT          0x8000
#define TERMINATE_CHARGE_ALARM_BIT      0x4000
#define OVER_TEMP_ALARM_BIT             0x1000
#define TERMINATE_DISCHARGE_ALARM_BIT   0x0800  // Battery capacity isdepleted
#define REMAINING_CAPACITY_ALARM_BIT    0x0200
#define REMAINING_TIME_ALARM_BIT        0x0100
// Status Bits
#define INITIALIZED_BIT                 0x0080
#define DISCHARGING_BIT                 0x0040
#define FULLY_CHARGED_BIT               0x0020
#define FULLY_DISCHARGED_BIT            0x0010
// Error Codes, see specification Appendix C.
#define OK_BIT                          0x0000
#define BUSY_BIT                        0x0001
#define RESERVED_COMMAND_BIT            0x0002
#define UNSUPPORTED_COMMAND_BIT         0x0003
#define ACCESS_DENIED_BIT               0x0004
#define OVER_UNDER_FLOW_BIT             0x0005
#define BAD_SIZE_BIT                    0x0006
#define UNKNOWN_ERROR_BIT               0x0007

int smbus_read_reg(const smbus_t *dev, uint8_t reg, uint8_t *out)
{
    i2c_acquire(dev->i2c);
    int res = i2c_read_regs(dev->i2c, dev->addr, reg, out, 2);
    i2c_release(dev->i2c);
    return res;
}

int smbus_battery_voltage(const smbus_t *dev, uint16_t *out)
{
    return smbus_read_reg(dev, VOLTAGE, (uint8_t *)out);
}

int smbus_battery_current(const smbus_t *dev, int16_t *out)
{
    return smbus_read_reg(dev, CURRENT, (uint8_t *)out);
}

int smbus_battery_cycle_count(const smbus_t *dev, uint16_t *out)
{
    return smbus_read_reg(dev, CYCLE_COUNT, (uint8_t *)out);
}

int smbus_battery_temp(const smbus_t *dev, uint16_t *out)
{
    return smbus_read_reg(dev, TEMPERATURE, (uint8_t *)out);
}

int smbus_battery_relative_soc(const smbus_t *dev, uint16_t *out)
{
    return smbus_read_reg(dev, RELATIVE_STATE_OF_CHARGE, (uint8_t *)out);
}

int smbus_battery_runtime_to_empty(const smbus_t *dev, uint16_t *out)
{
    return smbus_read_reg(dev, RUNTIME_TO_EMPTY, (uint8_t *)out);
}

int smbus_battery_at_rate_time_to_full(const smbus_t *dev, uint16_t *out)
{
    return smbus_read_reg(dev, AT_RATE_TIME_TO_FULL, (uint8_t *)out);
}

int smbus_battery_full_charge_capacity(const smbus_t *dev, uint16_t *out)
{
    return smbus_read_reg(dev, FULL_CHARGE_CAPACITY, (uint8_t *)out);
}
