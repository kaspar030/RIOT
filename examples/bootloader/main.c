#include "cpu.h"
#include "panic.h"

void kernel_init(void)
{
    cpu_jump_to_image(0x2000);
}

NORETURN void core_panic(core_panic_t crash_code, const char *message)
{
    (void)crash_code;
    (void)message;
    while(1) {}
}
