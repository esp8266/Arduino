/*
    SDFSFormatter.cpp - Formatter for SdFat SD cards
    Copyright (c) 2019 Earle F. Philhower, III. All rights reserved.

    A C++ implementation of the SdFat/examples/SdFormatter sketch:
    | Copyright (c) 2011-2018 Bill Greiman

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

#ifndef _SDFSFORMATTER_H
#define _SDFSFORMATTER_H

#include "SDFS.h"
#include <FS.h>
#include <PolledTimeout.h>

namespace sdfs
{

class SDFSFormatter
{
private:
    // Taken from main FS object
    sdfat::Sd2Card *card;
    sdfat::cache_t *cache;

    uint32_t cardSizeBlocks;
    uint32_t cardCapacityMB;


    // MBR information
    uint8_t partType;
    uint32_t relSector;
    uint32_t partSize;

    // Fake disk geometry
    uint8_t numberOfHeads;
    uint8_t sectorsPerTrack;

    // FAT parameters
    uint16_t reservedSectors;
    uint8_t sectorsPerCluster;
    uint32_t fatStart;
    uint32_t fatSize;
    uint32_t dataStart;

    uint8_t writeCache(uint32_t lbn)
    {
        return card->writeBlock(lbn, cache->data);
    }

    void clearCache(uint8_t addSig)
    {
        memset(cache, 0, sizeof(*cache));
        if (addSig)
        {
            cache->mbr.mbrSig0 = sdfat::BOOTSIG0;
            cache->mbr.mbrSig1 = sdfat::BOOTSIG1;
        }
    }

    bool clearFatDir(uint32_t bgn, uint32_t count)
    {
        clearCache(false);
        if (!card->writeStart(bgn, count))
        {
            DEBUGV("SDFS: Clear FAT/DIR writeStart failed");
            return false;
        }
        esp8266::polledTimeout::periodicFastMs timeToYield(5); // Yield every 5ms of runtime
        for (uint32_t i = 0; i < count; i++)
        {
            if (timeToYield)
            {
                delay(0); // WDT feed
            }
            if (!card->writeData(cache->data))
            {
                DEBUGV("SDFS: Clear FAT/DIR writeData failed");
                return false;
            }
        }
        if (!card->writeStop())
        {
            DEBUGV("SDFS: Clear FAT/DIR writeStop failed");
            return false;
        }
        return true;
    }

    uint16_t lbnToCylinder(uint32_t lbn)
    {
        return lbn / (numberOfHeads * sectorsPerTrack);
    }

    uint8_t lbnToHead(uint32_t lbn)
    {
        return (lbn % (numberOfHeads * sectorsPerTrack)) / sectorsPerTrack;
    }

    uint8_t lbnToSector(uint32_t lbn)
    {
        return (lbn % sectorsPerTrack) + 1;
    }

    bool writeMbr()
    {
        clearCache(true);
        sdfat::part_t* p = cache->mbr.part;
        p->boot = 0;
        uint16_t c = lbnToCylinder(relSector);
        if (c > 1023)
        {
            DEBUGV("SDFS: MBR CHS");
            return false;
        }
        p->beginCylinderHigh = c >> 8;
        p->beginCylinderLow = c & 0XFF;
        p->beginHead = lbnToHead(relSector);
        p->beginSector = lbnToSector(relSector);
        p->type = partType;
        uint32_t endLbn = relSector + partSize - 1;
        c = lbnToCylinder(endLbn);
        if (c <= 1023)
        {
            p->endCylinderHigh = c >> 8;
            p->endCylinderLow = c & 0XFF;
            p->endHead = lbnToHead(endLbn);
            p->endSector = lbnToSector(endLbn);
        }
        else
        {
            // Too big flag, c = 1023, h = 254, s = 63
            p->endCylinderHigh = 3;
            p->endCylinderLow = 255;
            p->endHead = 254;
            p->endSector = 63;
        }
        p->firstSector = relSector;
        p->totalSectors = partSize;
        if (!writeCache(0))
        {
            DEBUGV("SDFS: write MBR");
            return false;
        }
        return true;
    }

    uint32_t volSerialNumber()
    {
        return (cardSizeBlocks << 8) + micros();
    }

    bool makeFat16()
    {
        uint16_t const BU16 = 128;
        uint32_t nc;
        for (dataStart = 2 * BU16;; dataStart += BU16)
        {
            nc = (cardSizeBlocks - dataStart) / sectorsPerCluster;
            fatSize = (nc + 2 + 255) / 256;
            uint32_t r = BU16 + 1 + 2 * fatSize + 32;
            if (dataStart < r)
            {
                continue;
            }
            relSector = dataStart - r + BU16;
            break;
        }
        // check valid cluster count for FAT16 volume
        if (nc < 4085 || nc >= 65525)
        {
            DEBUGV("SDFS: Bad cluster count");
        }
        reservedSectors = 1;
        fatStart = relSector + reservedSectors;
        partSize = nc * sectorsPerCluster + 2 * fatSize + reservedSectors + 32;
        if (partSize < 32680)
        {
            partType = 0X01;
        }
        else if (partSize < 65536)
        {
            partType = 0X04;
        }
        else
        {
            partType = 0X06;
        }
        // write MBR
        if (!writeMbr())
        {
            DEBUGV("SDFS: writembr failed");
            return false;
        }

        clearCache(true);
        sdfat::fat_boot_t* pb = &cache->fbs;
        pb->jump[0] = 0XEB;
        pb->jump[1] = 0X00;
        pb->jump[2] = 0X90;
        for (uint8_t i = 0; i < sizeof(pb->oemId); i++)
        {
            pb->oemId[i] = ' ';
        }
        pb->bytesPerSector = 512;
        pb->sectorsPerCluster = sectorsPerCluster;
        pb->reservedSectorCount = reservedSectors;
        pb->fatCount = 2;
        pb->rootDirEntryCount = 512;
        pb->mediaType = 0XF8;
        pb->sectorsPerFat16 = fatSize;
        pb->sectorsPerTrack = sectorsPerTrack;
        pb->headCount = numberOfHeads;
        pb->hidddenSectors = relSector;
        pb->totalSectors32 = partSize;
        pb->driveNumber = 0X80;
        pb->bootSignature = sdfat::EXTENDED_BOOT_SIG;
        pb->volumeSerialNumber = volSerialNumber();
        memcpy_P(pb->volumeLabel, PSTR("NO NAME    "), sizeof(pb->volumeLabel));
        memcpy_P(pb->fileSystemType, PSTR("FAT16   "), sizeof(pb->fileSystemType));
        // write partition boot sector
        if (!writeCache(relSector))
        {
            DEBUGV("SDFS: FAT16 write PBS failed");
            return false;
        }
        // clear FAT and root directory
        if (!clearFatDir(fatStart, dataStart - fatStart))
        {
            DEBUGV("SDFS: FAT16 clear root failed\n");
            return false;
        }
        clearCache(false);
        cache->fat16[0] = 0XFFF8;
        cache->fat16[1] = 0XFFFF;
        // write first block of FAT and backup for reserved clusters
        if (!writeCache(fatStart) || !writeCache(fatStart + fatSize))
        {
            DEBUGV("FAT16 reserve failed");
            return false;
        }
        return true;
    }

    bool makeFat32()
    {
        uint16_t const BU32 = 8192;
        uint32_t nc;
        relSector = BU32;
        for (dataStart = 2 * BU32;; dataStart += BU32)
        {
            nc = (cardSizeBlocks - dataStart) / sectorsPerCluster;
            fatSize = (nc + 2 + 127) / 128;
            uint32_t r = relSector + 9 + 2 * fatSize;
            if (dataStart >= r)
            {
                break;
            }
        }
        // error if too few clusters in FAT32 volume
        if (nc < 65525)
        {
            DEBUGV("SDFS: Bad cluster count");
            return false;
        }
        reservedSectors = dataStart - relSector - 2 * fatSize;
        fatStart = relSector + reservedSectors;
        partSize = nc * sectorsPerCluster + dataStart - relSector;
        // type depends on address of end sector
        // max CHS has lbn = 16450560 = 1024*255*63
        if ((relSector + partSize) <= 16450560)
        {
            // FAT32
            partType = 0X0B;
        }
        else
        {
            // FAT32 with INT 13
            partType = 0X0C;
        }
        if (!writeMbr())
        {
            DEBUGV("SDFS: writembr failed");
            return false;
        }

        clearCache(true);

        sdfat::fat32_boot_t* pb = &cache->fbs32;
        pb->jump[0] = 0XEB;
        pb->jump[1] = 0X00;
        pb->jump[2] = 0X90;
        for (uint8_t i = 0; i < sizeof(pb->oemId); i++)
        {
            pb->oemId[i] = ' ';
        }
        pb->bytesPerSector = 512;
        pb->sectorsPerCluster = sectorsPerCluster;
        pb->reservedSectorCount = reservedSectors;
        pb->fatCount = 2;
        pb->mediaType = 0XF8;
        pb->sectorsPerTrack = sectorsPerTrack;
        pb->headCount = numberOfHeads;
        pb->hidddenSectors = relSector;
        pb->totalSectors32 = partSize;
        pb->sectorsPerFat32 = fatSize;
        pb->fat32RootCluster = 2;
        pb->fat32FSInfo = 1;
        pb->fat32BackBootBlock = 6;
        pb->driveNumber = 0X80;
        pb->bootSignature = sdfat::EXTENDED_BOOT_SIG;
        pb->volumeSerialNumber = volSerialNumber();
        memcpy_P(pb->volumeLabel, PSTR("NO NAME    "), sizeof(pb->volumeLabel));
        memcpy_P(pb->fileSystemType, PSTR("FAT32   "), sizeof(pb->fileSystemType));
        // write partition boot sector and backup
        if (!writeCache(relSector) || !writeCache(relSector + 6))
        {
            DEBUGV("SDFS: FAT32 write PBS failed");
            return false;
        }
        clearCache(true);
        // write extra boot area and backup
        if (!writeCache(relSector + 2) || !writeCache(relSector + 8))
        {
            DEBUGV("SDFS: FAT32 PBS ext failed");
            return false;
        }
        sdfat::fat32_fsinfo_t* pf = &cache->fsinfo;
        pf->leadSignature = sdfat::FSINFO_LEAD_SIG;
        pf->structSignature = sdfat::FSINFO_STRUCT_SIG;
        pf->freeCount = 0XFFFFFFFF;
        pf->nextFree = 0XFFFFFFFF;
        // write FSINFO sector and backup
        if (!writeCache(relSector + 1) || !writeCache(relSector + 7))
        {
            DEBUGV("SDFS: FAT32 FSINFO failed");
            return false;
        }
        clearFatDir(fatStart, 2 * fatSize + sectorsPerCluster);
        clearCache(false);
        cache->fat32[0] = 0x0FFFFFF8;
        cache->fat32[1] = 0x0FFFFFFF;
        cache->fat32[2] = 0x0FFFFFFF;
        // write first block of FAT and backup for reserved clusters
        if (!writeCache(fatStart) || !writeCache(fatStart + fatSize))
        {
            DEBUGV("SDFS: FAT32 reserve failed");
            return false;
        }
        return true;
    }

public:
    bool format(sdfat::SdFat *_fs, int8_t _csPin, SPISettings _spiSettings)
    {
        card  = static_cast<sdfat::Sd2Card*>(_fs->card());
        cache = _fs->cacheClear();

        if (!card->begin(_csPin, _spiSettings))
        {
            return false;
        }
        cardSizeBlocks = card->cardSize();
        if (cardSizeBlocks == 0)
        {
            return false;
        }

        cardCapacityMB = (cardSizeBlocks + 2047) / 2048;

        if (cardCapacityMB <= 6)
        {
            return false; // Card is too small
        }
        else if (cardCapacityMB <= 16)
        {
            sectorsPerCluster = 2;
        }
        else if (cardCapacityMB <= 32)
        {
            sectorsPerCluster = 4;
        }
        else if (cardCapacityMB <= 64)
        {
            sectorsPerCluster = 8;
        }
        else if (cardCapacityMB <= 128)
        {
            sectorsPerCluster = 16;
        }
        else if (cardCapacityMB <= 1024)
        {
            sectorsPerCluster = 32;
        }
        else if (cardCapacityMB <= 32768)
        {
            sectorsPerCluster = 64;
        }
        else
        {
            // SDXC cards
            sectorsPerCluster = 128;
        }

        // set fake disk geometry
        sectorsPerTrack = cardCapacityMB <= 256 ? 32 : 63;

        if (cardCapacityMB <= 16)
        {
            numberOfHeads = 2;
        }
        else if (cardCapacityMB <= 32)
        {
            numberOfHeads = 4;
        }
        else if (cardCapacityMB <= 128)
        {
            numberOfHeads = 8;
        }
        else if (cardCapacityMB <= 504)
        {
            numberOfHeads = 16;
        }
        else if (cardCapacityMB <= 1008)
        {
            numberOfHeads = 32;
        }
        else if (cardCapacityMB <= 2016)
        {
            numberOfHeads = 64;
        }
        else if (cardCapacityMB <= 4032)
        {
            numberOfHeads = 128;
        }
        else
        {
            numberOfHeads = 255;
        }

        // Erase all data on card (TRIM)
        uint32_t const ERASE_SIZE = 262144L;
        uint32_t firstBlock = 0;
        uint32_t lastBlock;
        do
        {
            lastBlock = firstBlock + ERASE_SIZE - 1;
            if (lastBlock >= cardSizeBlocks)
            {
                lastBlock = cardSizeBlocks - 1;
            }
            if (!card->erase(firstBlock, lastBlock))
            {
                return false; // Erase fail
            }
            delay(0); // yield to the OS to avoid WDT
            firstBlock += ERASE_SIZE;
        } while (firstBlock < cardSizeBlocks);

        if (!card->readBlock(0, cache->data))
        {
            return false;
        }

        if (card->type() != sdfat::SD_CARD_TYPE_SDHC)
        {
            return makeFat16();
        }
        else
        {
            return makeFat32();
        }
    }
}; // class SDFSFormatter

}; // namespace sdfs


#endif // _SDFSFORMATTER_H
