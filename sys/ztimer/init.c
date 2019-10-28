/*
 * Copyright (C) 2019 Kaspar Schleiser <kaspar@schleiser.de>
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
 * @brief       ztimer initialization code
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include "board.h"
#include "ztimer.h"
#include "ztimer/convert.h"
#include "ztimer/extend.h"
#include "ztimer/periph.h"
#include "ztimer/rtt.h"

#define ENABLE_DEBUG (1)
#include "debug.h"

#define ZTIMER_TYPE_PERIPH 1

/* for ZTIMER_USEC, use xtimer configuration if available and no ztimer
 * specific configuration is set. */
#ifndef CONFIG_ZTIMER_USEC_DEV
#  ifdef XTIMER_DEV
#    define CONFIG_ZTIMER_USEC_DEV      XTIMER_DEV
#  endif
#  ifdef XTIMER_HZ
#    define CONFIG_ZTIMER_USEC_FREQ     XTIMER_HZ
#  endif
#  ifdef XTIMER_WIDTH
#    define CONFIG_ZTIMER_USEC_WIDTH    XTIMER_WIDTH
#  endif
#endif

/*
 * The configuration logic is as follows:
 *
 * 1. if ztimer_usec is selected:
 *    1.1 without any configuration given, use periph/timer 0 with 1MHz,
 *        assume 32bit
 *    1.2 if CONFIG_ZTIMER_USEC_TYPE is given, use that
 *    1.3 if CONFIG_ZTIMER_USEC_FREQ is given, use that and convert/extend
 *        accordingly
 *    1.4 if CONFIG_ZTIMER_USEC_WIDTH is given, extend accordingly. Take conversion
 *        into account
 *
 * TODO:
 *
 * 2. if ztimer_msec is selected:
 *    2.1 if CONFIG_ZTIMER_USEC_TYPE is given
 *        2.1.1 set up accordingly
 *    2.2 if MODULE_RTT is available and RTT_FREQUENCY >= 1000, use that
 *    2.3 otherwise, use ZTIMER_MSEC, convert & extend to ms
 *
 * 3. if ztimer_sec is selected:
 *    3.1 if CONFIG_ZTIMER_SEC_TYPE is given
 *        3.1.1 set up accordingly
 *    3.2 if ztimer_msec is using rtt or rtc, use that (convert & extend to seconds)
 *    3.3 if MODULE_RTT is available, use that
 *    3.4 if MODULE_RTC is available, use that
 *    3.5 else, use ZTIMER_MSEC
 *
 * Nested ifdefs are used here as this is all compile-time logic.
 * This would benefit a lot from code generation...
 */

#ifdef MODULE_ZTIMER_USEC                               /* 1.  */
#  ifndef CONFIG_ZTIMER_USEC_TYPE                       /* 1.1, 1.2 */
#    define CONFIG_ZTIMER_USEC_TYPE ZTIMER_TYPE_PERIPH
#  endif
#  if CONFIG_ZTIMER_USEC_TYPE == ZTIMER_TYPE_PERIPH
#    ifndef CONFIG_ZTIMER_USEC_DEV
#      define CONFIG_ZTIMER_USEC_DEV    (0)             /* 1.1 */
#    endif
#    ifndef CONFIG_ZTIMER_USEC_FREQ
#        define CONFIG_ZTIMER_USEC_FREQ   (1000000LU)   /* 1.1 */
#    endif
#    ifndef CONFIG_ZTIMER_USEC_WIDTH
#        define CONFIG_ZTIMER_USEC_WIDTH (32)           /* 1.1 */
#    endif
#    ifndef CONFIG_ZTIMER_USEC_CHAN     /* currently unused! */
#      define CONFIG_ZTIMER_USEC_CHAN   (0)
#    endif

#    if CONFIG_ZTIMER_USEC_FREQ == 1000000LU            /* 1.3 */
#      define ZTIMER_USEC_DIV           0
#      define ZTIMER_USEC_MUL           0
#      define ZTIMER_USEC_CONVERT_BITS  0
#    elif CONFIG_ZTIMER_USEC_FREQ == 250000LU
#      define ZTIMER_USEC_DIV           4
#      define ZTIMER_USEC_MUL           0
#      define ZTIMER_USEC_CONVERT_BITS  2
#    elif CONFIG_ZTIMER_USEC_FREQ == 125000LU
#      define ZTIMER_USEC_DIV           8
#      define ZTIMER_USEC_MUL           0
#      define ZTIMER_USEC_CONVERT_BITS  3
#    elif CONFIG_ZTIMER_USEC_FREQ == 62500LU
#      define ZTIMER_USEC_DIV           16
#      define ZTIMER_USEC_MUL           0
#      define ZTIMER_USEC_CONVERT_BITS  4
#    elif CONFIG_ZTIMER_USEC_FREQ == 32768LU
#      define ZTIMER_USEC_DIV           15625
#      define ZTIMER_USEC_MUL           512
#      define ZTIMER_USEC_CONVERT_BITS  9
#    else
#      error unhandled CONFIG_ZTIMER_USEC_FREQ!
#    endif

     static ztimer_periph_t _ztimer_usec_periph;
#    define ZTIMER_USEC_INIT_PERIPH() \
            ztimer_periph_init(&_ztimer_usec_periph, CONFIG_ZTIMER_USEC_DEV, \
                             CONFIG_ZTIMER_USEC_FREQ)
#    define _ZTIMER_USEC_DEV _ztimer_usec_periph
#  else
#    error unknown CONFIG_ZTIMER_USEC_TYPE!
#  endif /* CONFIG_ZTIMER_USEC_TYPE == ZTIMER_TYPE_PERIPH */

# if (CONFIG_ZTIMER_USEC_WIDTH == 32) && (ZTIMER_USEC_CONVERT_BITS == 0)
    ztimer_dev_t *const ZTIMER_USEC = (ztimer_dev_t *) &_ZTIMER_USEC_DEV;
# else
#   if (ZTIMER_USEC_DIV != 0) || (ZTIMER_USEC_MUL != 0)
      static ztimer_convert_t _ztimer_usec_convert;
#     define ZTIMER_USEC_INIT_CONVERT() \
          ztimer_convert_init(&_ztimer_usec_convert, \
                              (ztimer_dev_t *)&_ZTIMER_USEC_DEV, \
                              ZTIMER_USEC_DIV, ZTIMER_USEC_MUL)
#     define _ZTIMER_USEC_CONVERT _ztimer_usec_convert
#   else
#     define _ZTIMER_USEC_CONVERT _ZTIMER_USEC_DEV
#   endif
    /* extend ztimer usec to full 32bit width */
    static ztimer_extend_t _ztimer_usec_periph_extend;
#   define ZTIMER_USEC_INIT_EXTEND() \
        ztimer_extend_init(&_ztimer_usec_periph_extend, \
                           (ztimer_dev_t *)&_ZTIMER_USEC_CONVERT, \
                           CONFIG_ZTIMER_USEC_WIDTH - ZTIMER_USEC_CONVERT_BITS)

    ztimer_dev_t *const ZTIMER_USEC = (ztimer_dev_t *) &_ztimer_usec_periph_extend;
# endif
#endif /* MODULE_ZTIMER_USEC */

void ztimer_init(void)
{
#ifdef MODULE_ZTIMER_USEC
# ifdef ZTIMER_USEC_INIT_PERIPH
    DEBUG("ztimer_init(): ZTIMER_USEC using periph timer %u freq %lu\n",
            _ztimer_usec_periph.dev, CONFIG_ZTIMER_USEC_FREQ);
    ZTIMER_USEC_INIT_PERIPH();
# endif
# ifdef ZTIMER_USEC_INIT_CONVERT
    DEBUG("ztimer_init(): ZTIMER_USEC converting div=%u mul=%u\n",
            ZTIMER_USEC_DIV, ZTIMER_USEC_MUL);
    ZTIMER_USEC_INIT_CONVERT();
# endif
# ifdef ZTIMER_USEC_INIT_EXTEND
    DEBUG("ztimer_init(): ZTIMER_USEC extending. base=%u convert=%u\n",
            CONFIG_ZTIMER_USEC_WIDTH, ZTIMER_USEC_CONVERT_BITS);
    ZTIMER_USEC_INIT_EXTEND();
# endif
#endif
}
