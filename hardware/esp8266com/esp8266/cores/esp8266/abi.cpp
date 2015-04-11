/*
 Copyright (c) 2014 Arduino.  All right reserved.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 See the GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <stdlib.h>
extern "C" {
#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"
#include "user_interface.h"
}

void *operator new(size_t size) {
    return os_malloc(size);
}

void *operator new[](size_t size) {
    return os_malloc(size);
}

void operator delete(void * ptr) {
    os_free(ptr);
}

void operator delete[](void * ptr) {
    os_free(ptr);
}

extern "C" void __cxa_pure_virtual(void) __attribute__ ((__noreturn__));
extern "C" void __cxa_deleted_virtual(void) __attribute__ ((__noreturn__));

void __cxa_pure_virtual(void) {
    abort();
}

void __cxa_deleted_virtual(void) {
    abort();
}

namespace std {
void __throw_bad_function_call() {
    abort();
}
}

// TODO: rebuild windows toolchain to make this unnecessary:
void* __dso_handle;

