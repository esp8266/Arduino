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

#include "Arduino.h"

extern "C" {
#include "user_interface.h"
}


//extern "C" void ets_wdt_init(uint32_t val);
extern "C" void ets_wdt_enable(void);
extern "C" void ets_wdt_disable(void);
extern "C" void wdt_feed(void);

EspClass ESP;

EspClass::EspClass()
{

}

void EspClass::wdtEnable(uint32_t timeout_ms)
{
    //todo find doku for ets_wdt_init may set the timeout
	ets_wdt_enable();
}

void EspClass::wdtEnable(WDTO_t timeout_ms)
{
    wdtEnable((uint32_t) timeout_ms);
}

void EspClass::wdtDisable(void)
{
	ets_wdt_disable();
}

void EspClass::wdtFeed(void)
{
	wdt_feed();
}

void EspClass::deepSleep(uint32_t time_us, WakeMode mode)
{
	system_deep_sleep_set_option(static_cast<int>(mode));
 	system_deep_sleep(time_us);
}

void EspClass::reset(void)
{
	((void (*)(void))0x40000080)();
}

void EspClass::restart(void)
{
    system_restart();
}

uint16_t EspClass::getVCC(void)
{
    return system_get_vdd33();
}

uint32_t EspClass::getFreeHeap(void)
{
    return system_get_free_heap_size();
}

uint32_t EspClass::getChipId(void)
{
    return system_get_chip_id();
}

const char * EspClass::getSDKversion(void)
{
    return system_get_sdk_version();
}

uint8_t EspClass::getBootVersion(void)
{
    return system_get_boot_version();
}

uint8_t EspClass::getBootMode(void)
{
    return system_get_boot_mode();
}

uint8_t EspClass::getCPUfreqMHz(void)
{
    return system_get_cpu_freq();
}


uint32_t EspClass::getFlashChipId(void)
{
    return spi_flash_get_id();
}

uint32_t EspClass::getFlashChipSize(void)
{
    uint32_t data;
    uint8_t * bytes = (uint8_t *) &data;
    // read first 4 byte (magic byte + flash config)
    if(spi_flash_read(0x0000, &data, 4) == SPI_FLASH_RESULT_OK) {
        switch((bytes[3] & 0xF0) >> 4) {
            case 0x0: // 4 Mbit (512KB)
                return (512 * kB);
            case 0x1: // 2 MBit (256KB)
                return (256 * kB);
            case 0x2: // 8 MBit (1MB)
                return (1 * MB);
            case 0x3: // 16 MBit (2MB)
                return (2 * MB);
            case 0x4: // 32 MBit (4MB)
                return (4 * MB);
            default: // fail?
                return 0;
        }
    }
    return 0;
}

uint32_t EspClass::getFlashChipSpeed(void)
{
    uint32_t data;
    uint8_t * bytes = (uint8_t *) &data;
    // read first 4 byte (magic byte + flash config)
    if(spi_flash_read(0x0000, &data, 4) == SPI_FLASH_RESULT_OK) {
        switch(bytes[3] & 0x0F) {
            case 0x0: // 40 MHz
                return (40 * MHz);
            case 0x1: // 26 MHz
                return (26 * MHz);
            case 0x2: // 20 MHz
                return (20 * MHz);
            case 0xF: // 80 MHz
                return (80 * MHz);
            default: // fail?
                return 0;
        }
    }
    return 0;
}

FlashMode_t EspClass::getFlashChipMode(void)
{
    FlashMode_t mode = FM_FAILD;
    uint32_t data;
    uint8_t * bytes = (uint8_t *) &data;
    // read first 4 byte (magic byte + flash config)
    if(spi_flash_read(0x0000, &data, 4) == SPI_FLASH_RESULT_OK) {
        mode = (FlashMode_t) bytes[2];
        if(mode > FM_DOUT) {
            mode = FM_FAILD;
        }
    }
    return mode;
}
