/* ----------------------------------------------------------------------------
 * dbglog.h - A set of macros that cleans up code that needs to produce debug
 *            or log information.
 *
 * Many embedded systems still put a premium on code space and therefore need
 * a way to conditionally compile in debug code. Yes, it can lead to code that
 * runs differently depending on whether the debug code is cmpiled in or not
 * but you need to be able to evaluate the tradeoff.
 *
 * See copyright notice in LICENSE.TXT
 * ----------------------------------------------------------------------------
 * NOTE WELL that this file may be included multiple times - this allows you
 * to set the trace level #define DBGLOG_LEVEL x
 *
 * To update which of the DBGLOG macros are compiled in, you must redefine the
 * DBGLOG_LEVEL macro and the include the dbglog.h file again, like this:
 *
 * #undef  DBGLOG_LEVEL
 * #define DBGLOG_LEVEL 6
 * #include "dbglog/dbglog.txt"
 *
 * To handle multiple inclusion, we need to first undefine any macros we define
 * so that the compiler does not warn us that we are changing a macro.
 * ----------------------------------------------------------------------------
 * The DBGLOG_LEVEL and DBGLOG_FUNCTION should be defined BEFORE this
 * file is included or else the following defaults are used:
 *
 * #define DBGLOG_LEVEL    0
 * #define DBGLOG_FUNCTION printf
 * ----------------------------------------------------------------------------
 * There are macros to handle the following decreasing levels of detail:
 *
 * 6 = TRACE
 * 5 = DEBUG
 * 4 = CRITICAL
 * 3 = ERROR
 * 2 = WARNING
 * 1 = INFO
 * 0 = FORCE - The DBGLOG_FUNCTION is always compiled in and is called only when
 *              the first parameter to the macro is non-0
 * ----------------------------------------------------------------------------
 */

#undef DBGLOG_TRACE
#undef DBGLOG_DEBUG
#undef DBGLOG_CRITICAL
#undef DBGLOG_ERROR
#undef DBGLOG_WARNING
#undef DBGLOG_INFO
#undef DBGLOG_FORCE

#ifndef DBGLOG_LEVEL
#define DBGLOG_LEVEL 0
#endif

#ifndef DBGLOG_FUNCTION
#define DBGLOG_FUNCTION printf
#endif

#define DBGLOG_32_BIT_PTR(x) ((uint32_t)(((uintptr_t)(x)) & 0xffffffff))

/* ------------------------------------------------------------------------- */

#if DBGLOG_LEVEL >= 6
#define DBGLOG_TRACE(format, ...) DBGLOG_FUNCTION(format,##__VA_ARGS__)
#else
#define DBGLOG_TRACE(format, ...)
#endif

#if DBGLOG_LEVEL >= 5
#define DBGLOG_DEBUG(format, ...) DBGLOG_FUNCTION(format,##__VA_ARGS__)
#else
#define DBGLOG_DEBUG(format, ...)
#endif

#if DBGLOG_LEVEL >= 4
#define DBGLOG_CRITICAL(format, ...) DBGLOG_FUNCTION(format,##__VA_ARGS__)
#else
#define DBGLOG_CRITICAL(format, ...)
#endif

#if DBGLOG_LEVEL >= 3
#define DBGLOG_ERROR(format, ...) DBGLOG_FUNCTION(format,##__VA_ARGS__)
#else
#define DBGLOG_ERROR(format, ...)
#endif

#if DBGLOG_LEVEL >= 2
#define DBGLOG_WARNING(format, ...) DBGLOG_FUNCTION(format,##__VA_ARGS__)
#else
#define DBGLOG_WARNING(format, ...)
#endif

#if DBGLOG_LEVEL >= 1
#define DBGLOG_INFO(format, ...) DBGLOG_FUNCTION(format,##__VA_ARGS__)
#else
#define DBGLOG_INFO(format, ...)
#endif

#define DBGLOG_FORCE(force, format, ...) {if (force) {DBGLOG_FUNCTION(format,##__VA_ARGS__);}}
