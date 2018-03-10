/*
 ESP_EEPROM.cpp - esp8266 EEPROM emulation
 
 Copyright (c) 2018 James Watson. All rights reserved.
 
 Based on ESP8266 EEPROM library, part of standard
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
 
 Avoid the significant period with no interrupts required for flash erasure
 - and avoid unnecessary re-flashing
 
 >>> Layout <<<
 4 bytes - size of a block
 bitmap - bit 0 never written - shows state of flash after erase
 subsequent bits are set to the opposite for each block containing data
 the highest block is the latest version
 Data versions follow consecutively - size rounded up to 4 byte boundaries
 and a minimum size
 */


#include "Arduino.h"
#include "ESP_EEPROM.h"

extern "C" {
#include "c_types.h"
#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "spi_flash.h"
}

extern "C" uint32_t _SPIFFS_end;

//------------------------------------------------------------------------------
EEPROMClass::EEPROMClass(uint32_t sector):
_sector(sector),
_data(0),
_size(0),
_bitmapSize(0),
_bitmap(0),
_offset(0),
_dirty(false)
{
}

//------------------------------------------------------------------------------
EEPROMClass::EEPROMClass(void)  :
_sector((((uint32_t) & _SPIFFS_end - 0x40200000) / SPI_FLASH_SEC_SIZE)),
_data(0),
_size(0),
_bitmapSize(0),
_bitmap(0),
_offset(0),
_dirty(false)
{
}

//------------------------------------------------------------------------------
void EEPROMClass::begin(size_t size) {
    _dirty = true;
    if (size <= 0 || size > (SPI_FLASH_SEC_SIZE - 8)) {
        // max size is smaller by 4 bytes for size and 4 byte bitmap - to keep 4 byte aligned
        return;
    } else if (size < EEPROM_MIN_SIZE) {
        size = EEPROM_MIN_SIZE;
    }
    
    size = (size + 3) & ~3; // align to 4 bytes
    _bitmapSize =  computeBitmapSize(size);

    // drop any old allocation and re-allocate buffers
    if (_bitmap) {
        delete[] _bitmap;
    }
    _bitmap = new uint8_t[_bitmapSize];
    if (_data) {
        delete[] _data;
    }
    _data = new uint8_t[size];
    
    noInterrupts();
    spi_flash_read(_sector * SPI_FLASH_SEC_SIZE, reinterpret_cast<uint32_t*>(&_size), 4);
    interrupts();
    
    if (_size != size) {
        // flash structure is all wrong - will need to re-do
        _size = size;
        _offset = 0;    // offset of zero => flash data is garbage
        
    } else {
        // Size is correct so get bitmap/data from flash
        // First read the bitmap from flash
        noInterrupts();
        spi_flash_read(_sector * SPI_FLASH_SEC_SIZE + 4, reinterpret_cast<uint32_t*>(_bitmap), _bitmapSize);
        interrupts();
        
        // flash should contain a good version of the data - find it using the bitmap
        _offset = offsetFromBitmap();
        
        if (_offset == 0 || _offset + _size > SPI_FLASH_SEC_SIZE) {
            // something is screwed up
            // flag that _data[] is bad / uninitialised
            _offset = 0;
        } else {   
            noInterrupts();
            spi_flash_read(_sector * SPI_FLASH_SEC_SIZE + _offset, reinterpret_cast<uint32_t*>(_data), _size);
            interrupts();
            
            // all good 
            _dirty = false;
        }
    }
}

//------------------------------------------------------------------------------
int EEPROMClass::percentUsed() {
    if(_offset == 0 || _size==0) return 0;
    else {
        int nCopies = (SPI_FLASH_SEC_SIZE - 4 - _bitmapSize) / _size;
        int copyNo = 1 + (_offset - 4 - _bitmapSize) / _size;
        return (100 * copyNo) / nCopies;
    }
}

//------------------------------------------------------------------------------
void EEPROMClass::end() {
    if (!_size)
        return;
    
    commit();
    if (_data) {
        delete[] _data;
    }
    if (_bitmap) {
        delete[] _bitmap;
    }
    _bitmap = 0;
    _bitmapSize = 0;
    _data = 0;
    _size = 0;
    _dirty = false;
}

//------------------------------------------------------------------------------
uint8_t EEPROMClass::read(int const address) {
    if (address < 0 || (size_t)address >= _size)
        return 0;
    if (!_data)
        return 0;
    
    return _data[address];
}

//------------------------------------------------------------------------------
void EEPROMClass::write(int const address, uint8_t const value) {
    if (address < 0 || (size_t)address >= _size)
        return;
    if (!_data)
        return;
    
    // Optimise _dirty. Only flagged if data written is different.
    if (_data[address] != value)
    {
        _data[address] = value;
        _dirty = true;
    }
}

//------------------------------------------------------------------------------
bool EEPROMClass::commitReset() {
    // set an offset that ensures flash will be erased before commit
    uint32_t oldOffset = _offset;   // if commit fails, _offset won't be updated
    _offset = SPI_FLASH_SEC_SIZE;
    _dirty = true;                  // ensure writing takes place
    if ( commit() ) {
        return (true);
    } else {
        _offset = oldOffset;
        return (false);
    }
}

//------------------------------------------------------------------------------
bool EEPROMClass::commit() {
    // everything has to be in place to even try a commit
    if (!_size || !_dirty || !_data || !_bitmap || _bitmapSize == 0) {
        return false;
    }
    
    SpiFlashOpResult flashOk = SPI_FLASH_RESULT_OK;
    uint32_t oldOffset = _offset;   // if write fails, _offset won't be updated
    
    // If initial version or not enough room for new version, erase and start anew
    if (_offset == 0 || _offset + _size + _size > SPI_FLASH_SEC_SIZE) {
        
        noInterrupts();
        flashOk = spi_flash_erase_sector(_sector);
        interrupts();
        if (flashOk != SPI_FLASH_RESULT_OK) {
            return false;
        }
        
        // write size
        noInterrupts();
        flashOk = spi_flash_write(_sector * SPI_FLASH_SEC_SIZE, reinterpret_cast<uint32_t*>(&_size), 4);
        interrupts();
        if (flashOk != SPI_FLASH_RESULT_OK) {
            return false;
        }
        
        // read first 4 bytes of bitmap
        noInterrupts();
        spi_flash_read(_sector * SPI_FLASH_SEC_SIZE + 4, reinterpret_cast<uint32_t*>(_bitmap), 4);
        interrupts();
        
        // init the rest of the _bitmap based on value of first byte
        for (int i = 4; i < _bitmapSize; i++ ) _bitmap[i] = _bitmap[0];
        
        // all reset ok - point to where the data needs to go
        _offset = 4 + _bitmapSize;
    } else {
        _offset += _size;
    }
    
    noInterrupts();
    flashOk = spi_flash_write(_sector * SPI_FLASH_SEC_SIZE + _offset, reinterpret_cast<uint32_t*>(_data), _size);
    interrupts();
    
    if (flashOk != SPI_FLASH_RESULT_OK) {
        _offset = oldOffset;
        return false;
    }
    
    // Data written OK so need to update bitmap
    int bitmapByteUpdated = flagUsedOffset();
    
    bitmapByteUpdated &= ~3;    // align to 4 byte for write
    noInterrupts();
    flashOk = spi_flash_write(_sector * SPI_FLASH_SEC_SIZE + bitmapByteUpdated + 4, reinterpret_cast<uint32_t*>(&_bitmap[bitmapByteUpdated]), 4);
    interrupts();
    if (flashOk != SPI_FLASH_RESULT_OK) {
        return false;
    }
    
    // all good!
    interrupts();
    _dirty = false;
    return true;
}

//------------------------------------------------------------------------------
// Force an immedate erase of the flash sector - but nothing is written
// Will need a commit() to write structure (size and bitmap etc.)
// but does re-intitialise internal storage
bool EEPROMClass::wipe() {
    if(_size==0 || _bitmapSize==0) return false;      // must have called begin()
    
    // drop any old allocation and re-allocate buffers
    if (_bitmap) {
        delete[] _bitmap;
    }
    _bitmap = new uint8_t[_bitmapSize];
    if (_data) {
        delete[] _data;
    }
    _data = new uint8_t[_size];

    noInterrupts();
    SpiFlashOpResult flashOk = spi_flash_erase_sector(_sector);
    interrupts();

    // flash is clear - need a commit() to write structure (size and bitmap etc.)
    _dirty = true;
    _offset = 0;
    return (flashOk==SPI_FLASH_RESULT_OK);
}

//------------------------------------------------------------------------------
// Compute the offset of the current version of data using the bitmap
uint16_t EEPROMClass::offsetFromBitmap() {
    
    if (!_bitmap || _bitmapSize <= 0) return 0;
    
    uint16_t offset = 4 + _bitmapSize;
    boolean flash = (_bitmap[0] & 1); // true => 'after flash' state is 1 (else it must be 0)
    
    // Check - the very first entry in the bitmap should indicate a valid _data
    if ((flash && ((_bitmap[0] & 2) != 0)) || (!flash && ((_bitmap[0] & 2) == 0))  ) {
        // something's wrong - Bitmap doesn't have bit recording first data version
        return 0;
    }
    
    for (int bmByte = 0; bmByte < _bitmapSize; bmByte++) {
        for (int bmBit = (bmByte == 0) ? 4 : 1; bmBit < 0x0100; bmBit <<= 1) {
            // looking for bit state that matches the 'after flash' state (i.e. first untouched bit)
            if ((flash && ((_bitmap[bmByte] & bmBit) != 0)) || (!flash && ((_bitmap[bmByte] & bmBit) == 0))  ) {
                return offset; // offset pointed at last written
            } else {
                offset += _size;
            }
        }
    }
    
    // dropped off the bottom - return the offset - but it will be useless
    return offset;
}

//------------------------------------------------------------------------------
// flag within the bitmap the appropriate bit for the current _data version at _offset
// return the byte index within _bitmap that has been changed
int EEPROMClass::flagUsedOffset() {
    int bitNo = 1 + (_offset - 4 - _bitmapSize) / _size;
    int byteNo = bitNo >> 3;
    
    uint8_t bitMask = 1 << (bitNo & 0x7);
    if (_bitmap[0] & 1) {
        // need to clear the bitmap bit
        _bitmap[byteNo] &= ~bitMask;
    } else {
        // need to set the bitmap bit
        _bitmap[byteNo] |= bitMask;
    }
    
    return byteNo;
}

//------------------------------------------------------------------------------
// Computing size of bitmap needed for the number of copies that can be held
uint16_t EEPROMClass::computeBitmapSize(size_t size) {

    // With 1 bit in bitmap and 8 bits per byte
    // This is the max number of copies possible
    uint32_t nCopies = ((SPI_FLASH_SEC_SIZE - 4L) * 8L - 1L) / (size * 8L + 1L);

    // applying alignment constraints - this is the bitmap size needed
    uint32_t bitmapSize = (((nCopies + 1L) + 31L) / 8L)  & ~3;

    return bitmapSize & 0x7fff;
}

//------------------------------------------------------------------------------
#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_EEPROM)
EEPROMClass EEPROM;
#endif
