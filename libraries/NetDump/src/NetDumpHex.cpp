/*
 NetDump library - tcpdump-like packet logger facility

 Copyright (c) 2018 David Gauchard. All rights reserved.
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

#include <NetDump.h>

void netDumpHex (Print& out, const char* data, size_t size, bool show_hex, bool show_ascii, size_t per_line)
{
    size_t start = 0;
    
    while (start < size)
    {
        size_t end = start + per_line;
        if (end > size)
            end = size;
        if (show_hex)
            for (size_t i = start; i < end; i++)
                out.printf("%02x ", (unsigned char)data[i]);
        if (show_ascii)
        {
            if (show_hex)
                for (size_t i = end; i < start + per_line; i++)
                    out.print("   ");
            for (size_t i = start; i < end; i++)
                out.printf("%c", data[i] >= 32 && data[i] < 128? data[i]: '.');
        }
        out.println();

        start += per_line;
    }
}

