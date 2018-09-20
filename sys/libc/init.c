#include "stdio_uart.h"

void libc_init(void)
{
#if defined(MODULE_STDIO_UART) || defined(MODULE_STDIO_RTT)
    stdio_init();
#endif
}
