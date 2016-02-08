#include "periph_conf.h"
uart_t *uart_devs[] = {
    (uart_t*) &samd21_uart_devs[0],
    (uart_t*) &samd21_uart_devs[1],
};

