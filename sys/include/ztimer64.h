/*
 * Copyright (C) 2021 Inria
 *               2021 Freie Universität Berlin
 *               2021 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_ztimer
 * @brief       ztimer 64bit version
 *
 * # Introduction
 *
 *
 * @{
 *
 * @file
 * @brief       ztimer 64bit API
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef ZTIMER64_H
#define ZTIMER64_H

#include <stdint.h>

#include "mutex.h"
#include "msg.h"
#include "ztimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ztimer64_t forward declaration
 */
typedef struct ztimer64_base ztimer64_base_t;
/**
 * @brief ztimer64_clock_t forward declaration
 */
typedef struct ztimer64_clock ztimer64_clock_t;

/**
 * @brief   Minimum information for each timer
 */
struct ztimer64_base {
    ztimer64_base_t *next;        /**< next timer in list */
    uint64_t target;              /**< absolute target time */
};

/**
 * @brief   ztimer64 structure
 *
 * This type represents an instance of a timer, which is set on an
 * underlying clock object
 */
typedef struct {
    ztimer64_base_t base;             /**< clock list entry */
    void (*callback)(void *arg);      /**< timer callback function pointer */
    void *arg;                        /**< timer callback argument */
} ztimer64_t;

/**
 * @brief   ztimer64 clock structure
 */
struct ztimer64_clock {
    ztimer64_base_t *list;          /**< list of active timers              */
    ztimer_clock_t *base_clock;     /**< 32bit clock backend                */
    ztimer_t base_timer;            /**< 32bit backend timer                */
    uint32_t base_last;
    uint64_t checkpoint;
    uint16_t adjust_set;            /**< will be subtracted on every set()  */
    uint16_t adjust_sleep;          /**< will be subtracted on every sleep(),
                                         in addition to adjust_set          */
#if MODULE_PM_LAYERED || DOXYGEN
    uint8_t block_pm_mode;          /**< min. pm mode to block for the clock to run */
#endif
};

/* User API */
/**
 * @brief   Set a timer on a clock
 *
 * This will place @p timer in the timer targets queue of @p clock.
 *
 * @note The memory pointed to by @p timer is not copied and must
 *       remain in scope until the callback is fired or the timer
 *       is removed via @ref ztimer64_remove
 *
 * @param[in]   clock       ztimer clock to operate on
 * @param[in]   timer       timer entry to set
 * @param[in]   offset      relative target time
 */
void ztimer64_set(ztimer64_clock_t *clock, ztimer64_t *timer, uint64_t offset);

/**
 * @brief   Check if a timer is currently active
 *
 * @param[in]   timer       timer to check
 *
 * @return 1 if timer is active
 * @return 0 if timer is not active
 */
unsigned ztimer64_is_set(const ztimer64_t *timer);

/**::
 * @brief   Remove a timer from a clock
 *
 * This will place @p timer in the timer targets queue for @p clock.
 *
 * This function does nothing if @p timer is not found in the timer queue of
 * @p clock.
 *
 * @param[in]   clock       ztimer clock to operate on
 * @param[in]   timer       timer entry to remove
 */
void ztimer64_remove(ztimer64_clock_t *clock, ztimer64_t *timer);

/**
 * @brief   Post a message after a delay
 *
 * This function sets a timer that will send a message @p offset ticks
 * from now.
 *
 * @note The memory pointed to by @p timer and @p msg will not be copied, i.e.
 *       `*timer` and `*msg` needs to remain valid until the timer has triggered.
 *
 * @param[in]   clock           ztimer clock to operate on
 * @param[in]   timer           ztimer timer struct to use
 * @param[in]   offset          ticks from now
 * @param[in]   msg             pointer to msg that will be sent
 * @param[in]   target_pid      pid the message will be sent to
 */
void ztimer64_set_msg(ztimer64_clock_t *clock, ztimer64_t *timer, uint32_t offset,
                    msg_t *msg, kernel_pid_t target_pid);

/**
 * @brief receive a message (blocking, with timeout)
 *
 * Similar to msg_receive(), but with a timeout parameter.
 * The function will return after waiting at most @p timeout ticks.
 *
 * @note: This might function might leave a message with type MSG_ZTIMER64 in the
 *        thread's message queue, which must be handled (ignored).
 *
 * @param[in]   clock           ztimer clock to operate on
 * @param[out]  msg             pointer to buffer which will be filled if a
 *                              message is received
 * @param[in]   timeout         relative timeout, in @p clock time units
 *
 * @return  >=0 if a message was received
 * @return  -ETIME on timeout
 */
int ztimer64_msg_receive_timeout(ztimer64_clock_t *clock, msg_t *msg,
                               uint32_t timeout);

/* created with dist/tools/define2u16.py */
#define MSG_ZTIMER64 0xc83e   /**< msg type used by ztimer64_msg_receive_timeout */

/**
 * @brief   Get the current time from a clock
 *
 * @param[in]   clock          ztimer clock to operate on
 *
 * @return  Current count on @p clock
 */
uint64_t ztimer64_now(ztimer64_clock_t *clock);

/**
 * @brief Suspend the calling thread until the time (@p last_wakeup + @p period)
 *
 * This function can be used to create periodic wakeups.
 *
 * When the function returns, @p last_wakeup is set to
 * (@p last_wakeup + @p period).
 *
 * @c last_wakeup should be set to ztimer64_now(@p clock) before first call of the
 * function.
 *
 * If the time (@p last_wakeup + @p period) has already passed, the function
 * sets @p last_wakeup to @p last_wakeup + @p period and returns immediately.
 *
 * @param[in]   clock           ztimer clock to operate on
 * @param[in]   last_wakeup     base time stamp for the wakeup
 * @param[in]   period          time in ticks that will be added to @p last_wakeup
 */
void ztimer64_periodic_wakeup(ztimer64_clock_t *clock, uint64_t *last_wakeup,
                            uint64_t period);

/**
 * @brief   Put the calling thread to sleep for the specified number of ticks
 *
 * @param[in]   clock           ztimer clock to use
 * @param[in]   duration        duration of sleep, in @p ztimer time units
 */
void ztimer64_sleep(ztimer64_clock_t *clock, uint64_t duration);

/**
 * @brief   Busy-wait specified duration
 *
 * @note: This blocks lower priority threads. Use only for *very* short delays.
 *
 * @param[in]   clock           ztimer clock to use
 * @param[in]   duration        duration to spin, in @p clock time units
 */
static inline void ztimer64_spin(ztimer64_clock_t *clock, uint64_t duration)
{
    uint32_t end = ztimer64_now(clock) + duration;

    while (ztimer64_now(clock) <= end) {}
}

/**
 * @brief Set a timer that wakes up a thread
 *
 * This function sets a timer that will wake up a thread when the timer has
 * expired.
 *
 * @param[in] clock         ztimer clock to operate on
 * @param[in] timer         timer struct to work with.
 * @param[in] offset        clock ticks from now
 * @param[in] pid           pid of the thread that will be woken up
 */
void ztimer64_set_wakeup(ztimer64_clock_t *clock, ztimer64_t *timer, uint64_t offset,
                       kernel_pid_t pid);

/**
 * @brief    Set timeout thread flag after @p timeout
 *
 * This function will set THREAD_FLAG_TIMEOUT on the current thread after @p
 * timeout usec have passed.
 *
 * @param[in]   clock           ztimer clock to operate on
 * @param[in]   timer           timer struct to use
 * @param[in]   timeout         timeout in ztimer64_clock's ticks
 */
void ztimer64_set_timeout_flag(ztimer64_clock_t *clock, ztimer64_t *timer,
                             uint64_t timeout);

/**
 * @brief   Try to lock the given mutex, but give up after @p timeout
 *
 * @param[in]       clock       ztimer clock to operate on
 * @param[in,out]   mutex       Mutex object to lock
 * @param[in]       timeout     timeout after which to give up
 *
 * @retval  0               Success, caller has the mutex
 * @retval  -ECANCELED      Failed to obtain mutex within @p timeout
 */
int ztimer64_mutex_lock_timeout(ztimer64_clock_t *clock, mutex_t *mutex,
                              uint64_t timeout);

/**
 * @brief   Try to lock the given rmutex, but give up after @p timeout
 *
 * @param[in]       clock       ztimer clock to operate on
 * @param[in,out]   rmutex      rmutex object to lock
 * @param[in]       timeout     timeout after which to give up
 *
 * @retval  0               Success, caller has the rmutex
 * @retval  -ECANCELED      Failed to obtain rmutex within @p timeout
 */
int ztimer64_rmutex_lock_timeout(ztimer64_clock_t *clock, rmutex_t *rmutex,
                               uint64_t timeout);

/**
 * @brief   Update ztimer clock head list offset
 *
 * @internal
 *
 * @param[in]   clock  ztimer clock to work on
 */
void ztimer64_update_head_offset(ztimer64_clock_t *clock);

/**
 * @brief   Initialize the board-specific default ztimer configuration
 */
void ztimer64_init(void);
void ztimer64_clock_init(ztimer64_clock_t *clock, ztimer_clock_t *base_clock);

/* default ztimer virtual devices */
/**
 * @brief   Default ztimer microsecond clock
 */
extern ztimer64_clock_t *const ZTIMER64_USEC;

/**
 * @brief   Default ztimer millisecond clock
 */
extern ztimer64_clock_t *const ZTIMER64_MSEC;

/**
 * @brief   Default ztimer second clock
 */
extern ztimer64_clock_t *const ZTIMER64_SEC;

/**
 * @brief   Base ztimer for the microsecond clock (ZTIMER64_USEC)
 *
 * This ztimer will reference the counter device object at the end of the
 * chain of ztimer64_clock_t for ZTIMER64_USEC.
 *
 * If the base counter device object's frequency (CONFIG_ZTIMER64_USEC_BASE_FREQ)
 * is not 1MHz then ZTIMER64_USEC will be converted on top of this one. Otherwise
 * they will reference the same ztimer64_clock.
 *
 * To avoid chained conversions its better to base new ztimer64_clock on top of
 * ZTIMER64_USEC_BASE running at CONFIG_ZTIMER64_USEC_BASE_FREQ.
 *
 */
extern ztimer64_clock_t *const ZTIMER64_USEC_BASE;

/**
 * @brief   Base ztimer for the millisecond clock (ZTIMER64_MSEC)
 *
 * This ztimer will reference the counter device object at the end of the
 * chain of ztimer64_clock_t for ZTIMER64_MSEC.
 *
 * If ztimer64_periph_rtt is not used then ZTIMER64_MSEC_BASE will reference the
 * same base as ZTIMER64_USEC_BASE.
 *
 * If the base counter device object's frequency (CONFIG_ZTIMER64_MSEC_BASE_FREQ)
 * is not 1KHz then ZTIMER64_MSEC will be converted on top of this one. Otherwise
 * they will reference the same ztimer64_clock.
 *
 * To avoid chained conversions its better to base new ztimer64_clock on top of
 * ZTIMER64_MSEC_BASE running at CONFIG_ZTIMER64_MSEC_BASE_FREQ.
 *
 */
extern ztimer64_clock_t *const ZTIMER64_MSEC_BASE;

#ifdef __cplusplus
}
#endif

#endif /* ZTIMER64_H */
/** @} */
