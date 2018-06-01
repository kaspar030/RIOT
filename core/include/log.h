/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  core_util
 * @{
 *
 * @file
 * @brief       System logging header
 *
 * This header offers a bunch of "LOG_*" functions that, with the default
 * implementation, just use printf, but honour a verbosity level.
 *
 * It is possible to define "LOG_PREFIX" to a string, before including "log.h".
 * That string will be prepended to any log message, e.g.:
 *    
 *    #define LOG_PREFI "my_module: "
 *    #include "log.h"
 *    [...]
 *    LOG_WARNING("situation normal, all fucked up\n");
 *
 * ... will result in "my_module: situation normal, all fucked up\n" to be
 * printed.
 *
 * If desired, it is possible to implement a log module which then will be used
 * instead the default printf-based implementation.  In order to do so, the log
 * module has to
 *
 * 1. provide "log_module.h"
 * 2. have a name starting with "log_" *or* depend on the pseudo-module LOG,
 * 3. implement log_write()
 *
 * See "sys/log/log_printfnoformat" for an example.
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef LOG_H
#define LOG_H

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @brief defined log levels
 *
 * These are the logging levels a user can choose.
 * The idea is to set LOG_LEVEL to one of these values in the application's Makefile.
 * That will restrict output of log statements to those with equal or lower log level.
 *
 * The default log level is LOG_INFO, which will print every message.
 *
 * The log function calls of filtered messages will be optimized out at compile
 * time, so a lower log level might result in smaller code size.
 */
enum {
    LOG_NONE,       /**< Lowest log level, will output nothing */
    LOG_ERROR,      /**< Error log level, will print only critical,
                         non-recoverable errors like hardware initialization
                         failures */
    LOG_WARNING,    /**< Warning log level, will print warning messages for
                         temporary errors */
    LOG_INFO,       /**< Informational log level, will print purely
                         informational messages like successful system bootup,
                         network link state, ...*/
    LOG_DEBUG,      /**< Debug log level, printing developer stuff considered
                         too verbose for production use */
    LOG_ALL         /**< print everything */
};

#ifndef LOG_LEVEL
/**
 * @brief Default log level define
 */
#define LOG_LEVEL LOG_INFO
#endif

/**
 * @brief Prefix to prepend to each message
 */
#ifndef LOG_PREFIX
#define LOG_PREFIX ""
#endif

/**
 * @brief Log message if level <= LOG_LEVEL
 */
#ifdef __clang__    /* following pragmas required for clang 3.8.0 */
#define LOG(level, ...) do { \
    _Pragma("clang diagnostic push") \
    _Pragma("clang diagnostic ignored \"-Wtautological-compare\"") \
    if ((level) <= LOG_LEVEL) log_write((level), LOG_PREFIX __VA_ARGS__); } while (0U) \
    _Pragma("clang diagnostic pop")
#else
#define LOG(level, ...) do { \
    if ((level) <= LOG_LEVEL) log_write((level), LOG_PREFIX __VA_ARGS__); } while (0U)
#endif /* __clang__ */

/**
 * @name Logging convenience defines
 * @{
 */
#define LOG_ERROR(...) LOG(LOG_ERROR, __VA_ARGS__)
#define LOG_WARNING(...) LOG(LOG_WARNING, __VA_ARGS__)
#define LOG_INFO(...) LOG(LOG_INFO, __VA_ARGS__)
#define LOG_DEBUG(...) LOG(LOG_DEBUG, __VA_ARGS__)
/** @} */

#ifdef MODULE_LOG
#include "log_module.h"
#else
#include <stdio.h>

/**
 * @brief Default log_write function, just maps to printf
 */
#define log_write(level, ...) printf(__VA_ARGS__)
#endif

#ifdef __cplusplus
}
#endif

#endif /* LOG_H */
/** @} */
