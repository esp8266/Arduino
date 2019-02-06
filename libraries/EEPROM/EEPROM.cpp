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

extern "C" uint32_t _SPIFFS_end;

EEPROMClass::EEPROMClass(uint32_t sector)
    : _sector(sector)
    , _data(0)
    , _size(0)
    , _dirty(false)
{
}

EEPROMClass::EEPROMClass(void)
    : _sector((((uint32_t) & _SPIFFS_end - 0x40200000) / SPI_FLASH_SEC_SIZE))
    , _data(0)
    , _size(0)
    , _dirty(false)
{
}

void EEPROMClass::begin(size_t size)
{
    if (size <= 0)
    {
        return;
    }
    if (size > SPI_FLASH_SEC_SIZE)
    {
        size = SPI_FLASH_SEC_SIZE;
    }

    size = (size + 3) & (~3);

    //In case begin() is called a 2nd+ time, don't reallocate if size is the same
    if (_data && size != _size)
    {
        delete[] _data;
        _data = new uint8_t[size];
    }
    else if (!_data)
    {
        _data = new uint8_t[size];
    }

    _size = size;

    noInterrupts();
    spi_flash_read(_sector * SPI_FLASH_SEC_SIZE, reinterpret_cast<uint32_t*>(_data), _size);
    interrupts();

    _dirty = false; //make sure dirty is cleared in case begin() is called 2nd+ time
}

void EEPROMClass::end()
{
    if (!_size)
    {
        return;
    }

    commit();
    if (_data)
    {
        delete[] _data;
    }
    _data = 0;
    _size = 0;
    _dirty = false;
}


uint8_t EEPROMClass::read(int const address)
{
    if (address < 0 || (size_t)address >= _size)
    {
        return 0;
    }
    if (!_data)
    {
        return 0;
    }

    return _data[address];
}

void EEPROMClass::write(int const address, uint8_t const value)
{
    if (address < 0 || (size_t)address >= _size)
    {
        return;
    }
    if (!_data)
    {
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

bool EEPROMClass::commit()
{
    bool ret = false;
    if (!_size)
    {
        return false;
    }
    if (!_dirty)
    {
        return true;
    }
    if (!_data)
    {
        return false;
    }

    noInterrupts();
    if (spi_flash_erase_sector(_sector) == SPI_FLASH_RESULT_OK)
    {
        if (spi_flash_write(_sector * SPI_FLASH_SEC_SIZE, reinterpret_cast<uint32_t*>(_data), _size) == SPI_FLASH_RESULT_OK)
        {
            _dirty = false;
            ret = true;
        }
    }
    interrupts();

    return ret;
}

uint8_t * EEPROMClass::getDataPtr()
{
    _dirty = true;
    return &_data[0];
}

uint8_t const * EEPROMClass::getConstDataPtr() const
{
    return &_data[0];
}

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_EEPROM)
EEPROMClass EEPROM;
#endif
