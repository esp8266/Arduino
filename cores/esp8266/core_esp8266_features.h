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

#ifdef __cplusplus

#include <stdlib.h> // malloc()
#include <stddef.h> // size_t

namespace arduino
{
    extern "C++"
    template <typename T, typename ...TConstructorArgs>
    T* new0 (size_t n, TConstructorArgs... TconstructorArgs)
    {
        // n==0: single allocation, otherwise it is an array
        size_t offset = n? sizeof(size_t): 0;
        size_t arraysize = n? n: 1;
        T* ptr = (T*)malloc(offset + (arraysize * sizeof(T)));
        if (ptr)
        {
            if (n)
                *(size_t*)(ptr) = n;
            for (size_t i = 0; i < arraysize; i++)
                new (ptr + offset + i * sizeof(T)) T(TconstructorArgs...);
            return ptr + offset;
        }
        return nullptr;
    }
}

#define arduino_new(Type, ...) arduino::new0<Type>(0, ##__VA_ARGS__)
#define arduino_newarray(Type, n, ...) arduino::new0<Type>(n, ##__VA_ARGS__)

#endif // __cplusplus

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
#endif // not CORE_MOCK

#endif // CORE_ESP8266_FEATURES_H
