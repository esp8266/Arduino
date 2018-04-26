
#ifndef __CORE_HEALTH_H
#define __CORE_HEALTH_H

/*
   These 2 functions reflect a general health status
   User can check them anytime
 */

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    core_warn_heap = 0,
    core_warn_uart = 1,
    core_warn_last = 1,

    core_fatal_string = 16,
    core_fatal_axtls = 17,
} core_condition_e;

// returns 0 if condition is not fatal
int core_condition_is_fatal (void);

// returns 0 if core hasn't got this disease
// returns 1 otherwise, and cure it
int core_condition_has (core_condition_e disease);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __CORE_HEALTH_H
