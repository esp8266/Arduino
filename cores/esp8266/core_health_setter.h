
#ifndef __CORE_HEALTH_SETTER_H
#define __CORE_HEALTH_SETTER_H

/*
   this file is intended to be used by core internals
 */

#include <core_health.h>

#ifdef __cplusplus
extern "C" {
#endif

// bad or fatal things happened
void core_condition_got (core_condition_e disease);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __CORE_HEALTH_SETTER_H
