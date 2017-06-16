#include "firmware.h"
#include "cpu.h"
#include "panic.h"

void kernel_init(void)
{
    firmware_jump_to_slot(1);
}

NORETURN void core_panic(core_panic_t crash_code, const char *message)
{
    (void)crash_code;
    (void)message;
    while(1) {}
}
