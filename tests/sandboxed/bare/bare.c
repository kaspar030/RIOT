#include <stdint.h>

#include "syscall.h"

void _start_c(void *arg);

extern const char _srelocate, _erelocate, _etext;

void __attribute((section(".crt"))) _start(void *arg){
    /* RIOT passes the stack top in R9.
     * This is also the start of this sandbox' data/bss memory */

    register uintptr_t *stack_top;
    __asm__ ("mov %0, r9\n" : "=r" (stack_top));

    /* this relocates .data from the binary (which is maybe on flash)
     * into RAM */
    uintptr_t *reloc_start = (uintptr_t *)&_etext;
    uintptr_t len = &_erelocate - &_srelocate;
    uintptr_t *reloc_end = (uintptr_t *)(((uintptr_t)reloc_start) + len);

    for (const uintptr_t *p = reloc_start; p < reloc_end;) {
        *stack_top++ = *p++;
    }

    /* RIOT cleans bss */
    sys_puts("_start(): reloc done, jumping to _start_c()");
    _start_c(arg);
}

char writable[11] = "0123456789";
void _start_c(void *arg)
{
    sys_puts("_start_c() arg:");
    sys_putu32x((uintptr_t)arg);
    writable[5] = 'x';
    sys_puts(writable);
}
