/*
    debug.cpp - debug helper functions
    Copyright (c) 2015 Markus Sattler. All rights reserved.
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

#include "Arduino.h"
#include "debug.h"

void ICACHE_RAM_ATTR hexdump(const void *mem, uint32_t len, uint8_t cols)
{
    const uint8_t* src = (const uint8_t*) mem;
    os_printf("\n[HEXDUMP] Address: 0x%08X len: 0x%X (%d)", (ptrdiff_t)src, len, len);
    for (uint32_t i = 0; i < len; i++)
    {
        if (i % cols == 0)
        {
            os_printf("\n[0x%08X] 0x%08X: ", (ptrdiff_t)src, i);
            yield();
        }
        os_printf("%02X ", *src);
        src++;
    }
    os_printf("\n");
}
