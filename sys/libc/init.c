#include "stdio_uart.h"

void libc_init(void)
{
#ifdef MODULE_STDIO_UART
    stdio_uart_init();
#endif
}
