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

uint8_t EspClass::getHeapFragmentation (uint32_t* freeHeap, uint16_t* maxBlock)
{
    // L2 / Euclidian norm of free block sizes.
    // Having getFreeHeap()=sum(hole-size), un-fragmentation is given by
    // sqrt(sum(hole-size²)) / sum(hole-size)

    umm_info(NULL, 0);
    uint32_t block_size = umm_block_size();
    uint32_t fh = ummHeapInfo.freeBlocks * block_size;
    if (freeHeap)
        *freeHeap = fh;
    if (maxBlock)
        *maxBlock = ummHeapInfo.maxFreeContiguousBlocks * block_size;
    return 100 - (sqrt32(ummHeapInfo.ummFreeSize2) * 100) / fh;
}
