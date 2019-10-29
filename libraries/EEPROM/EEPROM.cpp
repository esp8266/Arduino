/*
  EEPROM.cpp - esp8266 EEPROM emulation

  Copyright (c) 2014 Ivan Grokhotkov. All rights reserved.
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
#include "EEPROM.h"
#include "debug.h"

extern "C" {
#include "c_types.h"
#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "spi_flash.h"
}

#include <flash_hal.h>

EEPROMClass::EEPROMClass(uint32_t sector)
: _sector(sector)
, _data(0)
, _size(0)
, _dirty(false)
{
}

EEPROMClass::EEPROMClass(void)
: _sector(((EEPROM_start - 0x40200000) / SPI_FLASH_SEC_SIZE))
, _data(0)
, _size(0)
, _dirty(false)
{
}

void EEPROMClass::begin(size_t size) {
  if (size <= 0) {
    DEBUGV("EEPROMClass::begin error, size == 0\n");
    return;
  }
  if (size > SPI_FLASH_SEC_SIZE) {
    DEBUGV("EEPROMClass::begin error, %d > %d\n", size, SPI_FLASH_SEC_SIZE);
    size = SPI_FLASH_SEC_SIZE;
  }

  size = (size + 3) & (~3);

  //In case begin() is called a 2nd+ time, don't reallocate if size is the same
  if(_data && size != _size) {
    delete[] _data;
    _data = new uint8_t[size];
  } else if(!_data) {
    _data = new uint8_t[size];
  }

  _size = size;

  if (!ESP.flashRead(_sector * SPI_FLASH_SEC_SIZE, reinterpret_cast<uint32_t*>(_data), _size)) {
    DEBUGV("EEPROMClass::begin flash read failed\n");
  }

  _dirty = false; //make sure dirty is cleared in case begin() is called 2nd+ time
}

void EEPROMClass::end() {
  if (!_size)
    return;

  commit();
  if(_data) {
    delete[] _data;
  }
  _data = 0;
  _size = 0;
  _dirty = false;
}


uint8_t EEPROMClass::read(int const address) {
  if (address < 0 || (size_t)address >= _size) {
    DEBUGV("EEPROMClass::read error, address %d > %d or %d < 0\n", address, _size, address);
    return 0;
  }
  if (!_data) {
    DEBUGV("EEPROMClass::read without ::begin\n");
    return 0;
  }

  return _data[address];
}

void EEPROMClass::write(int const address, uint8_t const value) {
  if (address < 0 || (size_t)address >= _size) {
    DEBUGV("EEPROMClass::write error, address %d > %d or %d < 0\n", address, _size, address);
    return;
  }
  if(!_data) {
    DEBUGV("EEPROMClass::read without ::begin\n");
    return;
  }

  // Optimise _dirty. Only flagged if data written is different.
  uint8_t* pData = &_data[address];
  if (*pData != value)
  {
    *pData = value;
    _dirty = true;
  }
}

bool EEPROMClass::commit() {
  if (!_size)
    return false;
  if(!_dirty)
    return true;
  if(!_data)
    return false;

  if (ESP.flashEraseSector(_sector)) {
    if (ESP.flashWrite(_sector * SPI_FLASH_SEC_SIZE, reinterpret_cast<uint32_t*>(_data), _size)) {
      _dirty = false;
      return true;
    }
  }

  DEBUGV("EEPROMClass::commit failed\n");
  return false;
}

uint8_t * EEPROMClass::getDataPtr() {
  _dirty = true;
  return &_data[0];
}

uint8_t const * EEPROMClass::getConstDataPtr() const {
  return &_data[0];
}

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_EEPROM)
EEPROMClass EEPROM;
#endif
