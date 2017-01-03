#include "cpu.h"
#include "periph_conf.h"

#ifdef MODULE_SYSTICK

void systick_init(void) {
    SysTick_Config(CLOCK_CORECLOCK / 1000);
}

#endif /* MODULE_SYSTICK */
