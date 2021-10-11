#include <stdint.h>

static inline __attribute__((always_inline)) uint32_t syscall_dispatch(
    unsigned num,
    uint32_t arg)
{
    register uint32_t arg0 __asm("r0") = arg;
    register uint32_t result __asm("r0");
    __asm volatile (
        "svc %[num] \n"
        : "=r" (result)
        : [num] "i" (num), "0" (arg0)
        : "memory"
        );

    return result;
}

typedef enum {
    SYSCALL_THREAD_YIELD    = 1,
    SYSCALL_THREAD_EXIT     = 2,
    SYSCALL_MUTEX_LOCK,
    SYSCALL_MUTEX_UNLOCK,
    SYSCALL_PUTS,
    SYSCALL_PUTU32X,
} syscall_num_t;

static inline void sys_puts(char *str)
{
    syscall_dispatch(SYSCALL_PUTS, (uint32_t)(intptr_t)str);
}

static inline void sys_putu32x(uint32_t val)
{
    syscall_dispatch(SYSCALL_PUTU32X, val);
}


void _start_c(void *arg);

extern const char _srelocate, _erelocate, _etext;

void __attribute((section(".crt"))) _start(void *arg){
    /* RIOT passes the stack top in R9 */
    register uintptr_t *stack_top;

    __asm__ ("mov %0, r9\n" : "=r" (stack_top));
    //stack_top -= 0x8000000;

    /* this relocates .data from the binary (which is maybe on flash)
     * into RAM */
    uintptr_t *reloc_start = (uintptr_t *)&_etext;
    uintptr_t len = &_erelocate - &_srelocate;
    uintptr_t *reloc_end = (uintptr_t *)(((uintptr_t)reloc_start) + len);

    for (const uintptr_t *p = reloc_start; p < reloc_end;) {
        *stack_top++ = *p++;
    }
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
