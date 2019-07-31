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
    T* new4arduino (size_t n, TConstructorArgs... TconstructorArgs)
    {
        if (n == 0)
            return nullptr;
        size_t offs = n == 1? 0: sizeof(size_t);
        T* ptr = (T*)malloc(offs + (n * sizeof(T)));
        if (ptr)
        {
            if (n > 1)
                *(size_t*)(ptr) = n;
            for (size_t i = 0; i < n; i++)
                new (ptr + offs + i * sizeof(T)) T(TconstructorArgs...);
            return ptr + offs;
        }
        return nullptr;
    }

    // new0<>() and new0array<>() below are necessary only
    // because of variadic macro lack of feature, for example with:
    //   #define arduino_new(Type, ...) arduino::new0array<Type>(1, __VA_ARGS__)
    // this arduino_new ^^ macro cannot be used with no arguments after 'Type'
    // hint: VA_OPT is c++20

    extern "C++"
    template <typename T, typename ...TConstructorArgs>
    T* new0 (TConstructorArgs... TconstructorArgs)
    {
        return new4arduino<T>(1, TconstructorArgs...);
    }

    extern "C++"
    template <typename T, size_t n, typename ...TConstructorArgs>
    T* new0array (TConstructorArgs... TconstructorArgs)
    {
        return new4arduino<T>(n, TconstructorArgs...);
    }
}

#define arduino_new(Type, ...) arduino::new0<Type>(__VA_ARGS__)
#define arduino_newarray(Type, n, ...) arduino::new0array<Type, n>(__VA_ARGS__)

#endif // __cplusplus

#endif // CORE_ESP8266_FEATURES_H
