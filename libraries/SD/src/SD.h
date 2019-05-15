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

#undef FILE_READ
#define FILE_READ sdfat::O_READ
#undef FILE_WRITE
#define FILE_WRITE (sdfat::O_READ | sdfat::O_WRITE | sdfat::O_CREAT)

class SDClass {
public:
    boolean begin(uint8_t csPin, SPISettings cfg = SPI_HALF_SPEED) {
	SDFS.setConfig(SDFSConfig(csPin, cfg));
        return (boolean)SDFS.begin();
    }

    void end(bool endSPI = true) {
        SDFS.end();
        if (endSPI) {
            SPI.end();
        }
    }

    File open(const char *filename, uint8_t mode = FILE_READ) {
        return SDFS.open(filename, getMode(mode));
    }

    File open(const String &filename, uint8_t mode = FILE_READ) {
        return open(filename.c_str(), mode);
    }

    boolean exists(const char *filepath) {
        return (boolean)SDFS.exists(filepath);
    }

    boolean exists(const String &filepath) {
        return (boolean)SDFS.exists(filepath.c_str());
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

    uint64_t size() {
        return ((uint64_t)clusterSize() * (uint64_t)totalClusters());
    }

private:
    const char *getMode(uint8_t mode) {
        bool read = (mode & sdfat::O_READ) ? true : false;
        bool write = (mode & sdfat::O_WRITE) ? true : false;
        bool append = (mode & sdfat::O_APPEND) ? true : false;
        if      (  read & !write )           { return "r";  }
        else if ( !read &  write & !append ) { return "w+"; }
        else if ( !read &  write &  append ) { return "a";  }
        else if (  read &  write & !append ) { return "w+"; } // may be a bug in FS::mode interpretation, "r+" seems proper
        else if (  read &  write &  append ) { return "a+"; }
        else                                 { return "r";  }
    }

};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_SD)
extern SDClass SD;
#endif

#endif
