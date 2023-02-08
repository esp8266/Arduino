/*
 core_esp8266_features.h - list of features integrated in to ESP8266 core

 Copyright (c) 2014 Ivan Grokhotkov. All rights reserved.
 This file is part of the esp8266 core for Arduino environment.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

 */

#ifndef CORE_ESP8266_FEATURES_H
#define CORE_ESP8266_FEATURES_H

#define CORE_HAS_LIBB64
#define CORE_HAS_BASE64_CLASS
#define CORE_HAS_CXA_GUARD
#define CORE_HAS_UMM

#define WIFI_HAS_EVENT_CALLBACK
#define WIFI_IS_OFF_AT_BOOT

#include <stdbool.h> // bool
#include <stddef.h> // size_t
#include <stdint.h>
#include <stdlib.h> // malloc()

#ifndef __STRINGIFY
#define __STRINGIFY(a) #a
#endif

// these low level routines provide a replacement for SREG interrupt save that AVR uses
// but are esp8266 specific. A normal use pattern is like
//
//{
//    uint32_t savedPS = xt_rsil(1); // this routine will allow level 2 and above
//    // do work here
//    xt_wsr_ps(savedPS); // restore the state
//}
//
// level (0-15), interrupts of the given level and above will be active
// level 15 will disable ALL interrupts,
// level 0 will enable ALL interrupts,
//
#ifndef CORE_MOCK

#define xt_rsil(level) (__extension__({uint32_t state; __asm__ __volatile__("rsil %0," __STRINGIFY(level) : "=a" (state) :: "memory"); state;}))
#define xt_wsr_ps(state)  __asm__ __volatile__("wsr %0,ps; isync" :: "a" (state) : "memory")

inline uint32_t esp_get_cycle_count() __attribute__((always_inline));
inline uint32_t esp_get_cycle_count() {
  uint32_t ccount;
  __asm__ __volatile__("rsr %0,ccount":"=a"(ccount));
  return ccount;
}

inline uint32_t esp_get_program_counter() __attribute__((always_inline));
inline uint32_t esp_get_program_counter() {
  uint32_t pc;
  __asm__ __volatile__("movi %0, ." : "=r" (pc) : : ); // ©earlephilhower
  return pc;
}

#else // CORE_MOCK

#define xt_rsil(level) (level)
#define xt_wsr_ps(state) do { (void)(state); } while (0)

inline uint32_t esp_get_program_counter() { return 0; }

#endif // CORE_MOCK


// Tools for preloading code into the flash cache
#define PRECACHE_ATTR __attribute__((optimize("no-reorder-blocks"))) \
                      __attribute__((noinline))

#define PRECACHE_START(tag) \
    precache(NULL,(uint8_t *)&&_precache_end_##tag - (uint8_t*)&&_precache_start_##tag); \
    _precache_start_##tag:

#define PRECACHE_END(tag) \
    _precache_end_##tag:

#ifdef __cplusplus
extern "C" {
#endif

void precache(void *f, uint32_t bytes);
unsigned long millis(void);
unsigned long micros(void);
uint64_t micros64(void);
void delay(unsigned long);
void delayMicroseconds(unsigned int us);

#if defined(F_CPU) || defined(CORE_MOCK)
#ifdef __cplusplus
constexpr
#else
inline
#endif
int esp_get_cpu_freq_mhz()
{
    return F_CPU / 1000000L;
}
#else
inline int esp_get_cpu_freq_mhz()
{
    uint8_t system_get_cpu_freq(void);
    return system_get_cpu_freq();
}
#endif


// Call this function in your setup() to cause the phase locked version of the generator to
// be linked in automatically.  Otherwise, the default PWM locked version will be used.
void enablePhaseLockedWaveform(void);

// Determine when the sketch runs on ESP8285
#if !defined(CORE_MOCK)
bool esp_is_8285() __attribute__((const, nothrow));
#else
inline bool esp_is_8285()
{
    return false;
}
#endif

#ifdef __cplusplus
}
#endif

#endif // CORE_ESP8266_FEATURES_H
