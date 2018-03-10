/*
 ESP_EEPROM.cpp - improved esp8266 EEPROM emulation
 
 Copyright (c) 2018 James Watson. All rights reserved.
 
 Based on API defined for ESP8266 EEPROM library, part of standard
 esp8266 core for Arduino environment by Ivan Grokhotkov.
 
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

#ifndef ESP_EEPROM_h
#define ESP_EEPROM_h

#include <stddef.h>
#include <stdint.h>

// If you are using a tiny amount then allocation map takes a lot of room and a long time to check
// so minimum size is limited
const size_t EEPROM_MIN_SIZE = 16;

class EEPROMClass {
public:
    
    EEPROMClass(void);
    
    void begin(size_t size);
    uint8_t read(int const address);
    void write(int const address, uint8_t const val);
    bool commit();
    bool commitReset();
    bool wipe();
    int percentUsed();
    void end();
    
    template<typename T>
    T &get(int const address, T &t) {
        if (_data && (address >= 0) && (address + sizeof(T) <= _size)) {
            memcpy((uint8_t*) &t, _data + address, sizeof(T));
        }
        return t;
    }
    
    template<typename T>
    const T &put(int const address, const T &t) {
        if (_data && (address >= 0) && (address + sizeof(T) <= _size)) {
            
            // only flag as dirty and copied if different - if already dirty, just get on with copy
            if (_dirty || memcmp(_data + address, (const uint8_t*)&t, sizeof(T)) != 0) {
                _dirty = true;
                memcpy(_data + address, (const uint8_t*)&t, sizeof(T));
            }
        }
        return t;
    }
    
    size_t length() {
        return _size;
    }
    
protected:
    EEPROMClass(uint32_t sector);
    
    uint32_t _sector;
    uint8_t* _data;
    uint32_t _size;
    uint8_t* _bitmap;
    uint16_t _bitmapSize;
    uint16_t _offset;
    bool _dirty;
    
    uint16_t offsetFromBitmap();
    int flagUsedOffset();
    uint16_t computeBitmapSize(size_t size);
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_EEPROM)
extern EEPROMClass EEPROM;
#endif

#endif

