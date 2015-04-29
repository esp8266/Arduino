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

uint32_t EspClass::getFlashChipId(void)
{
    return spi_flash_get_id();
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
