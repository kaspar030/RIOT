/*
 * Copyright (C) 2021 Kaspar Schleiser <kaspar@schleiser.de>
 *               2021 Freie Universität Berlin
 *               2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     sys_ztimer
 * @{
 *
 * @file
 * @brief       ztimer64 core functionality
 *
 * This file contains ztimer64's main API implementation and functionality
 *
 * TODO:
 * - fix api (base -> relative, add absolute API)
 * - add some kind of tick so base_clock overflows don't get lost
 * - add proper unittests
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */
#include <assert.h>
#include <stdint.h>
#include <inttypes.h>

#include "ztimer64.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static int _add_entry_to_list(ztimer64_clock_t *clock, ztimer64_base_t *entry);
static int _del_entry_from_list(ztimer64_clock_t *clock,
                                ztimer64_base_t *entry);
static void _ztimer64_update(ztimer64_clock_t *clock);
//static void _ztimer64_print(const ztimer64_clock_t *clock);

static unsigned _is_set(const ztimer64_t *t)
{
    return !!(t->base.target);
}

unsigned ztimer64_is_set(const ztimer64_t *timer)
{
    return _is_set(timer);
}

void ztimer64_remove(ztimer64_clock_t *clock, ztimer64_t *timer)
{
    unsigned state = irq_disable();

    if (_is_set(timer)) {
        if (_del_entry_from_list(clock, &timer->base) == 0) {
            _ztimer64_update(clock);
        }
    }

    irq_restore(state);
}

void ztimer64_set(ztimer64_clock_t *clock, ztimer64_t *timer, uint64_t val)
{
    unsigned state = irq_disable();

    if (_is_set(timer)) {
        _del_entry_from_list(clock, &timer->base);
    }

    timer->base.target = val;

    /* optionally subtract a configurable adjustment value */
    if (val > clock->adjust_set) {
        val -= clock->adjust_set;
    }
    else {
        val = 0;
    }

    if (_add_entry_to_list(clock, &timer->base)) {
        _ztimer64_update(clock);
    }

    irq_restore(state);
}

static int _add_entry_to_list(ztimer64_clock_t *clock, ztimer64_base_t *entry)
{
    ztimer64_base_t *list = clock->list;

    if (list) {
        if (list->target > entry->target) {
            /* special case: new entry's target is earlier than old list head */
            entry->next = list;
            clock->list = entry;
            return 1;
        }
        else {
            /* Jump past all entries which are set to an earlier target than the new entry */
            while (list->next && list->next->target < entry->target) {
                list = list->next;
            }
            entry->next = list->next;
            list->next = entry;
            return 0;
        }
    }
    else {
        /* adding first entry */
        entry->next = 0;
        clock->list = entry;
        return 1;
    }
}

static int _del_entry_from_list(ztimer64_clock_t *clock, ztimer64_base_t *entry)
{
    DEBUG("_del_entry_from_list()\n");
    assert(_is_set((ztimer64_t *)entry));
    if (clock->list == entry) {
        /* special case: removing first entry */
        clock->list = entry->next;
        return 1;
    }

    ztimer64_base_t *list_entry = clock->list;

    while (list_entry->next) {
        if (list_entry->next == entry) {
            list_entry->next = entry->next;
            break;
        }
        list_entry = list_entry->next;
    }
    return 0;
}

uint64_t ztimer64_now(ztimer64_clock_t *clock)
{
    uint64_t now;
    unsigned state = irq_disable();
    uint32_t base_now = ztimer_now(clock->base_clock);

    if (clock->checkpoint & 0x80000000) {
        if (base_now & 0x80000000) {}
        else {
            clock->checkpoint += 0x80000000;
        }
    }
    else {
        if (base_now & 0x80000000) {
            clock->checkpoint += 0x80000000;
        }
        else {
        }
    }

    now = clock->checkpoint | base_now;

    irq_restore(state);
    return now;
}

static void _ztimer64_update(ztimer64_clock_t *clock)
{
    if (!clock->list) {
        ztimer_remove(clock->base_clock, &clock->base_timer);
    }
    else {
        uint64_t now = ztimer64_now(clock);
        uint64_t next = clock->list->target;
        uint32_t target;
        if (next > now) {
            uint64_t diff = next - now;
            if (diff <= (1 << 30)) {
                /* timer within range */
                target = diff;
            }
            else {
                /* timer out of range */
                target = (1 << 30);
            }
        }
        else {
            /* timer has passed already, trigger ASAP */
            target = 0;
        }
        printf("ztimer64_update() now=%llu next=%llu target=%u\n", now, next, target);
        ztimer_set(clock->base_clock, &clock->base_timer, target);
    }
}

void ztimer64_handler(void *arg)
{
    (void)arg;
    puts("ztimer64_handler() stub");
}

/* static void _ztimer64_tick(void *arg) */
/* { */
/*     ztimer64_clock_t *clock = arg; */
/*     ztimer64_now(clock); */
/* } */

void ztimer64_clock_init(ztimer64_clock_t *clock, ztimer_clock_t *base_clock)
{
    *clock =
        (ztimer64_clock_t){ .base_clock = base_clock,
                            .base_timer =
                            { .callback = ztimer64_handler, .arg = clock } };

    ztimer64_now(clock);
}

/* static void _ztimer64_print(const ztimer64_clock_t *clock) */
/* { */
/*     const ztimer64_base_t *entry = clock->list; */

/*     while (entry) { */
/*         printf("0x%08x:%" PRIu64 "\n", (unsigned)entry, */
/*                entry->target); */

/*         entry = entry->next; */
/*     } */
/*     puts(""); */
/* } */
