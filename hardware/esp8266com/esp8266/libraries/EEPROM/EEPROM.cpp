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

 extern "C" {
#include "c_types.h"
#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "spi_flash.h"
}

#define CONFIG_START_SECTOR 0x7b
#define CONFIG_SECTOR (CONFIG_START_SECTOR + 0)
#define CONFIG_ADDR (SPI_FLASH_SEC_SIZE * CONFIG_SECTOR)

EEPROMClass::EEPROMClass()
: _data(0), _size(0), _dirty(false)
{
}

void EEPROMClass::begin(size_t size)
{
    if (size < 0)
        return;
    if (size > SPI_FLASH_SEC_SIZE)
        size = SPI_FLASH_SEC_SIZE;

    _data = new uint8_t[size];
    _size = size;

    spi_flash_read(CONFIG_ADDR, reinterpret_cast<uint32_t*>(_data), _size);
}

void EEPROMClass::end()
{
    if (!_size)
        return;

    commit();

    delete[] _data;
    _data = 0;
    _size = 0;
}


uint8_t EEPROMClass::read(int address)
{
    if (address < 0 || (size_t)address >= _size)
        return 0;

    return _data[address];
}

void EEPROMClass::write(int address, uint8_t value)
{
    if (address < 0 || (size_t)address >= _size)
        return;

    _data[address] = value;
    _dirty = true;
}

bool EEPROMClass::commit()
{
    bool ret = false;
    if (!_size)
        return false;
    if(!_dirty)
        return true;

    ETS_UART_INTR_DISABLE();
    if(spi_flash_erase_sector(CONFIG_SECTOR) == SPI_FLASH_RESULT_OK) {
        if(spi_flash_write(CONFIG_ADDR, reinterpret_cast<uint32_t*>(_data), _size) == SPI_FLASH_RESULT_OK) {
            _dirty = false;
            ret = true;
        }
    }
    ETS_UART_INTR_ENABLE();
    return ret;
}

uint8_t * EEPROMClass::getDataPtr()
{
    return &_data[0];
}


EEPROMClass EEPROM;
