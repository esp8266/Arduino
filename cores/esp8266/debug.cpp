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
#include "osapi.h"

#ifdef DEBUG_ESP_CORE
void __iamslow(const char* what)
{
    DEBUGV("%s should be overridden for better efficiency\r\n", what);
}
#endif

IRAM_ATTR
void hexdump(const void *mem, uint32_t len, uint8_t cols)
{
    const char* src = (const char*)mem;
    os_printf("\n[HEXDUMP] Address: %p len: 0x%X (%d)", src, len, len);
    while (len > 0)
    {
        uint32_t linesize = cols > len ? len : cols;
        os_printf("\n[%p] 0x%04x: ", src, (int)(src - (const char*)mem));
        for (uint32_t i = 0; i < linesize; i++)
        {
            os_printf("%02x ", *(src + i));
        }
        os_printf("  ");
        for (uint32_t i = linesize; i < cols; i++)
        {
            os_printf("   ");
        }
        for (uint32_t i = 0; i < linesize; i++)
        {
            unsigned char c = *(src + i);
            os_putc(isprint(c) ? c : '.');
        }
        src += linesize;
        len -= linesize;
        optimistic_yield(10000);
    }
    os_printf("\n");
}
