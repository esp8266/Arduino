/*
 Esp.cpp - ESP8266-specific APIs
 Copyright (c) 2015 Ivan Grokhotkov. All rights reserved.
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

#include "umm_malloc/umm_malloc.h"
#include "umm_malloc/umm_malloc_cfg.h"
#include "coredecls.h"
#include "Esp.h"

uint16_t EspClass::getHeapUnfragness(uint32_t* freeHeap)
{
    ummFreeSize2 = 1; // enable processing
    umm_info(NULL, 0);
    ummFreeSize2 -= 1;
    uint32_t fh = ummHeapInfo.freeBlocks * umm_block_size();
    uint16_t ret = 1000.0 * sqrt(ummFreeSize2) / fh;
    ummFreeSize2 = 0; // disable processing
    if (freeHeap)
        *freeHeap = fh;
    return ret;
}
