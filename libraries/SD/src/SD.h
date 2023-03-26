/*
 SD.h - A thin shim for Arduino ESP8266 Filesystems
 Copyright (c) 2019 Earle F. Philhower, III.  All rights reserved.

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

#ifndef __SD_H__
#define __SD_H__

#include <Arduino.h>
#include <FS.h>
#include <SDFS.h>

// Avoid type ambiguity, force u8 instead of untyped literal
// ref. #6106 as to why we add APPEND to WRITE

inline constexpr uint8_t SDClassFileRead { FILE_READ };
#undef FILE_READ
#define FILE_READ SDClassFileRead

inline constexpr uint8_t SDClassFileWrite { FILE_WRITE | O_APPEND };
#undef FILE_WRITE
#define FILE_WRITE SDClassFileWrite

static inline constexpr const char* SDClassFileMode(uint8_t mode) {
    bool read = false;
    bool write = false;

    switch (mode & O_ACCMODE) {
    case O_RDONLY:
        read = true;
        break;
    case O_WRONLY:
        write = true;
        break;
    case O_RDWR:
        read = true;
        write = true;
        break;
    }

    const bool append = (mode & O_APPEND) > 0;

    if      (  read && !write )            { return "r";  }
    else if ( !read &&  write && !append ) { return "w+"; }
    else if ( !read &&  write &&  append ) { return "a";  }
    else if (  read &&  write && !append ) { return "w+"; } // may be a bug in FS::mode interpretation, "r+" seems proper
    else if (  read &&  write &&  append ) { return "a+"; }

    return "r";
}

class SDClass {
public:
    bool begin(uint8_t csPin, uint32_t cfg = SPI_HALF_SPEED) {
        SDFS.setConfig(SDFSConfig(csPin, cfg));
        return (boolean)SDFS.begin();
    }

    void end(bool endSPI = true) {
        SDFS.end();
        if (endSPI) {
            SPI.end();
        }
    }

    fs::File open(const char *filename, uint8_t mode = FILE_READ) {
        return SDFS.open(filename, SDClassFileMode(mode));
    }

    fs::File open(const char *filename, const char *mode) {
        return SDFS.open(filename, mode);
    }

    fs::File open(const String &filename, uint8_t mode = FILE_READ) {
        return open(filename.c_str(), mode);
    }

    fs::File open(const String &filename, const char *mode) {
        return open(filename.c_str(), mode);
    }

    boolean exists(const char *filepath) {
        return (boolean)SDFS.exists(filepath);
    }

    boolean exists(const String &filepath) {
        return (boolean)SDFS.exists(filepath.c_str());
    }

    boolean rename(const char* filepathfrom, const char* filepathto) {
        return (boolean)SDFS.rename(filepathfrom, filepathto);
    }

    boolean rename(const String &filepathfrom, const String &filepathto) {
        return (boolean)rename(filepathfrom.c_str(), filepathto.c_str());
    }

    boolean mkdir(const char *filepath) {
        return (boolean)SDFS.mkdir(filepath);
    }

    boolean mkdir(const String &filepath) {
        return (boolean)SDFS.mkdir(filepath.c_str());
    }
  
    boolean remove(const char *filepath) {
        return (boolean)SDFS.remove(filepath);
    }

    boolean remove(const String &filepath) {
        return remove(filepath.c_str());
    }
  
    boolean rmdir(const char *filepath) {
        return (boolean)SDFS.rmdir(filepath);
    }

    boolean rmdir(const String &filepath) {
        return rmdir(filepath.c_str());
    }

    uint8_t type() {
        sdfs::SDFSImpl* sd = static_cast<sdfs::SDFSImpl*>(SDFS.getImpl().get());
        return sd->type();
    }

    uint8_t fatType() {
        sdfs::SDFSImpl* sd = static_cast<sdfs::SDFSImpl*>(SDFS.getImpl().get());
        return sd->fatType();
    }

    size_t blocksPerCluster() {
        sdfs::SDFSImpl* sd = static_cast<sdfs::SDFSImpl*>(SDFS.getImpl().get());
        return sd->blocksPerCluster();
    }

    size_t totalClusters() {
        sdfs::SDFSImpl* sd = static_cast<sdfs::SDFSImpl*>(SDFS.getImpl().get());
        return sd->totalClusters();
    }

    size_t blockSize() {
        return 512;
    }

    size_t totalBlocks() {
        return (totalClusters() / blocksPerCluster());
    }

    size_t clusterSize() {
        return blocksPerCluster() * blockSize();
    }

    size_t size() {
        uint64_t sz = size64();
#ifdef DEBUG_ESP_PORT
	if (sz > std::numeric_limits<uint32_t>::max()) {
            DEBUG_ESP_PORT.printf_P(PSTR("WARNING: SD card size overflow (%lld >= 4GB).  Please update source to use size64().\n"), (long long)sz);
        }
#endif
        return (size_t)sz;
    }

    uint64_t size64() {
        return ((uint64_t)clusterSize() * (uint64_t)totalClusters());
    }

    void setTimeCallback(time_t (*cb)(void)) {
        SDFS.setTimeCallback(cb);
    }

    // Wrapper to allow obsolete datetimecallback use, silently convert to time_t in wrappertimecb
    void dateTimeCallback(void (*cb)(uint16_t*, uint16_t*)) {
        extern void (*__SD__userDateTimeCB)(uint16_t*, uint16_t*);
        __SD__userDateTimeCB = cb;
	SDFS.setTimeCallback(wrapperTimeCB);
    }

private:
    static time_t wrapperTimeCB(void) {
        extern void (*__SD__userDateTimeCB)(uint16_t*, uint16_t*);
        if (__SD__userDateTimeCB) {
            uint16_t d, t;
            __SD__userDateTimeCB(&d, &t);
            return sdfs::SDFSImpl::FatToTimeT(d, t);
        }
        return time(nullptr);
    }

};


static inline uint16_t FAT_YEAR(uint16_t fatDate) {
  return 1980 + (fatDate >> 9);
}
static inline uint8_t FAT_MONTH(uint16_t fatDate) {
  return (fatDate >> 5) & 0XF;
}
static inline uint8_t FAT_DAY(uint16_t fatDate) {
  return fatDate & 0X1F;
}
static inline uint8_t FAT_HOUR(uint16_t fatTime) {
  return fatTime >> 11;
}
static inline uint8_t FAT_MINUTE(uint16_t fatTime) {
  return (fatTime >> 5) & 0X3F;
}
static inline uint8_t FAT_SECOND(uint16_t fatTime) {
  return 2*(fatTime & 0X1F);
}


#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_SD)
extern SDClass SD;
#endif

#endif
