/**
 * Print thread information.
 *
 * Copyright (C) 2013, INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup sys_ps
 * @{
 * @file
 * @brief   UNIX like ps command
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @note    The entry 'runtime_usec' in 'MODULE_SCHEDSTATISTICS' is limited
 *          to 2**32 microseconds. So the entry gets reset after ~1.2 hours.
 * @}
 */

#include <stdio.h>
#include <assert.h>

#include "architecture.h"
#include "thread.h"
#include "sched.h"

#ifdef MODULE_SCHEDSTATISTICS
#include "schedstatistics.h"
#include "ztimer.h"
#endif

#ifdef MODULE_TLSF_MALLOC
#include "tlsf.h"
#include "tlsf-malloc.h"
#endif

/**
 * @brief Prints a list of running threads including stack usage to stdout.
 */
void ps(void)
{
#ifdef DEVELHELP
    int overall_stacksz = 0, overall_used = 0;
#endif

    printf("\tpid | "
#ifdef CONFIG_THREAD_NAMES
            "%-21s| "
#endif
            "%-9sQ | pri "
#ifdef DEVELHELP
           "| stack  ( used) ( free) | base addr  | current     "
#endif
#ifdef MODULE_SCHEDSTATISTICS
           "| runtime  | switches  | runtime_usec "
#endif
           "\n",
#ifdef CONFIG_THREAD_NAMES
           "name",
#endif
           "state");

#if defined(DEVELHELP) && ISR_STACKSIZE
    int isr_usage = thread_isr_stack_usage();
    void *isr_start = thread_isr_stack_start();
    void *isr_sp = thread_isr_stack_pointer();
    printf("\t  - | isr_stack            | -        - |"
           "   - | %6i (%5i) (%5i) | %10p | %10p\n",
           ISR_STACKSIZE, isr_usage, ISR_STACKSIZE - isr_usage,
           isr_start, isr_sp);
    overall_stacksz += ISR_STACKSIZE;
    if (isr_usage > 0) {
        overall_used += isr_usage;
    }
#endif

#ifdef MODULE_SCHEDSTATISTICS
    uint64_t rt_sum = 0;
    if (!IS_ACTIVE(MODULE_CORE_IDLE_THREAD)) {
        rt_sum = sched_pidlist[KERNEL_PID_UNDEF].runtime_us;
    }
    for (kernel_pid_t i = KERNEL_PID_FIRST; i <= KERNEL_PID_LAST; i++) {
        thread_t *p = thread_get(i);
        if (p != NULL) {
            rt_sum += sched_pidlist[i].runtime_us;
        }
    }
#endif /* MODULE_SCHEDSTATISTICS */

    for (kernel_pid_t i = KERNEL_PID_FIRST; i <= KERNEL_PID_LAST; i++) {
        thread_t *p = thread_get(i);

        if (p != NULL) {
            thread_status_t state = thread_get_status(p);                   /* copy state */
            const char *sname = thread_state_to_string(state);              /* get state name */
            const char *queued = thread_is_active(p) ? "Q" : "_";           /* get queued flag */
#ifdef DEVELHELP
            int stacksz = thread_get_stacksize(p);                          /* get stack size */
            overall_stacksz += stacksz;
            int stack_free = thread_measure_stack_free(p);
            stacksz -= stack_free;
            overall_used += stacksz;
#endif
#ifdef MODULE_SCHEDSTATISTICS
            /* multiply with 100 for percentage and to avoid floats/doubles */
            uint64_t runtime_us = sched_pidlist[i].runtime_us * 100;
            uint32_t ztimer_us = {sched_pidlist[i].runtime_us};
            unsigned runtime_major = runtime_us / rt_sum;
            unsigned runtime_minor = ((runtime_us % rt_sum) * 1000) / rt_sum;
            unsigned switches = sched_pidlist[i].schedules;
#endif
            printf("\t%3" PRIkernel_pid
#ifdef CONFIG_THREAD_NAMES
                   " | %-20s"
#endif
                   " | %-8s %.1s | %3i"
#ifdef DEVELHELP
                   " | %6" PRIuSIZE " (%5i) (%5i) | %10p | %10p "
#endif
#ifdef MODULE_SCHEDSTATISTICS
                   " | %2d.%03d%% |  %8u  | %10"PRIu32" "
#endif
                   "\n",
                   thread_getpid_of(p),
#ifdef CONFIG_THREAD_NAMES
                   thread_get_name(p),
#endif
                   sname, queued, thread_get_priority(p)
#ifdef DEVELHELP
                   , thread_get_stacksize(p), stacksz, stack_free,
                   thread_get_stackstart(p), thread_get_sp(p)
#endif
#ifdef MODULE_SCHEDSTATISTICS
                   , runtime_major, runtime_minor, switches, ztimer_us
#endif
                  );
        }
    }

#ifdef DEVELHELP
    printf("\t%5s %-21s|%13s%6s %6i (%5i) (%5i)\n", "|", "SUM", "|", "|",
           overall_stacksz, overall_used, overall_stacksz - overall_used);
#   ifdef MODULE_TLSF_MALLOC
    puts("\nHeap usage:");
    tlsf_size_container_t sizes = { .free = 0, .used = 0 };
    tlsf_walk_pool(tlsf_get_pool(_tlsf_get_global_control()), tlsf_size_walker, &sizes);
    printf("\tTotal free size: %u\n", sizes.free);
    printf("\tTotal used size: %u\n", sizes.used);
#   endif
#endif
}
