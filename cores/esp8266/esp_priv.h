/*
    esp_priv.h - private esp8266 helpers
    Copyright (c) 2020 esp8266/Arduino community.  All right reserved.

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


#ifndef __ESP_PRIV
#define __ESP_PRIV

#if defined(CORE_MOCK)

constexpr bool __byteAddressable(const void*)
{
    return true;
}

#else // on hardware

#include <sys/config.h>

// returns true when addr can be used without "pgm_" functions or non32xfer service
inline bool __byteAddressable(const void* addr)
{
    return addr < (const void*)(XCHAL_DATARAM0_VADDR + XCHAL_DATARAM0_SIZE);
}

#endif // on hardware

#endif // __ESP_PRIV
