#include <stdint.h>

static inline __attribute__((always_inline)) uint32_t syscall_dispatch(unsigned num,
                                                                       uint32_t arg)
{
    register uint32_t arg0 __asm ("r0") = arg;
    register uint32_t result __asm ("r0");
    __asm volatile (
            "svc %[num] \n"
            : "=r" (result)
            : [num] "i" (num), "0" (arg0)
            : "memory"
            );
    return result;
}

typedef enum {
    SYSCALL_THREAD_YIELD = 1,
    SYSCALL_THREAD_EXIT = 2,
    SYSCALL_MUTEX_LOCK,
    SYSCALL_MUTEX_UNLOCK,
    SYSCALL_PUTS,
} syscall_num_t;

static inline void sys_puts(char *str)
{
    syscall_dispatch(SYSCALL_PUTS, (uint32_t)(intptr_t)str);
}

void _start_c(void);

extern const uintptr_t _srelocate, _erelocate;

void __attribute((section(".crt"))) _start(uintptr_t *stack_top)
{
    uintptr_t *r9;
    __asm__("\t movl %%r9,%0" : "=r"(r9));
    __asm__("bkpt");
    for (const uintptr_t *p = &_srelocate; p < &_erelocate; p++) {
        *stack_top++ = *p;
    }
    __asm__("bkpt");
    _start_c();
}

char writable[14] = "bare hello()!";
void other_function(void) {
    sys_puts("foo");
}
void _start_c(void) {
    __asm__("svc 89");
    other_function();
    writable[5] = 'x';
    sys_puts(writable);
    other_function();
}

