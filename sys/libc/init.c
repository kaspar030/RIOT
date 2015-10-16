#include "uart_stdio.h"

void libc_init(void)
{
#ifdef MODULE_UART_STDIO
    uart_stdio_init();
#endif
}
