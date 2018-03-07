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
#include "flash_utils.h"
#include "eboot_command.h"
#include <memory>
#include "interrupts.h"
#include "MD5Builder.h"

extern "C" {
#include "user_interface.h"

extern struct rst_info resetInfo;
}


//#define DEBUG_SERIAL Serial


/**
 * User-defined Literals
 *  usage:
 *
 *   uint32_t = test = 10_MHz; // --> 10000000
 */

unsigned long long operator"" _kHz(unsigned long long x) {
    return x * 1000;
}

unsigned long long operator"" _MHz(unsigned long long x) {
    return x * 1000 * 1000;
}

unsigned long long operator"" _GHz(unsigned long long x) {
    return x * 1000 * 1000 * 1000;
}

unsigned long long operator"" _kBit(unsigned long long x) {
    return x * 1024;
}

unsigned long long operator"" _MBit(unsigned long long x) {
    return x * 1024 * 1024;
}

unsigned long long operator"" _GBit(unsigned long long x) {
    return x * 1024 * 1024 * 1024;
}

unsigned long long operator"" _kB(unsigned long long x) {
    return x * 1024;
}

unsigned long long operator"" _MB(unsigned long long x) {
    return x * 1024 * 1024;
}

unsigned long long operator"" _GB(unsigned long long x) {
    return x * 1024 * 1024 * 1024;
}


EspClass ESP;

void EspClass::wdtEnable(uint32_t timeout_ms)
{
    (void) timeout_ms;
    /// This API can only be called if software watchdog is stopped
    system_soft_wdt_restart();
}

void EspClass::wdtEnable(WDTO_t timeout_ms)
{
    wdtEnable((uint32_t) timeout_ms);
}

void EspClass::wdtDisable(void)
{
    /// Please don't stop software watchdog too long (less than 6 seconds),
    /// otherwise it will trigger hardware watchdog reset.
    system_soft_wdt_stop();
}

void EspClass::wdtFeed(void)
{
    system_soft_wdt_feed();
}

extern "C" void esp_yield();

void EspClass::deepSleep(uint64_t time_us, WakeMode mode)
{
    system_deep_sleep_set_option(static_cast<int>(mode));
    system_deep_sleep(time_us);
    esp_yield();
}

//this calculation was taken verbatim from the SDK api reference for SDK 2.1.0.
//Note: system_rtc_clock_cali_proc() returns a uint32_t, even though system_deep_sleep() takes a uint64_t.
uint64_t EspClass::deepSleepMax()
{
  //cali*(2^31-1)/(2^12)
  return (uint64_t)system_rtc_clock_cali_proc()*(0x80000000-1)/(0x1000);

}

bool EspClass::rtcUserMemoryRead(uint32_t offset, uint32_t *data, size_t size)
{
    if (size + offset > 512) {
        return false;
    } else {
        return system_rtc_mem_read(64 + offset, data, size);
    }
}

bool EspClass::rtcUserMemoryWrite(uint32_t offset, uint32_t *data, size_t size)
{
    if (size + offset > 512) {
        return false;
    } else {
        return system_rtc_mem_write(64 + offset, data, size);
    }
}

extern "C" void __real_system_restart_local();
void EspClass::reset(void)
{
    __real_system_restart_local();
}

void EspClass::restart(void)
{
    system_restart();
    esp_yield();
}

uint16_t EspClass::getVcc(void)
{
    InterruptLock lock;
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

extern "C" uint32_t core_version;
extern "C" const char* core_release;

String EspClass::getCoreVersion()
{
    if (core_release != NULL) {
        return String(core_release);
    }
    char buf[12];
    snprintf(buf, sizeof(buf), "%08x", core_version);
    return String(buf);
}

const char * EspClass::getSdkVersion(void)
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

uint8_t EspClass::getCpuFreqMHz(void)
{
    return system_get_cpu_freq();
}


uint32_t EspClass::getFlashChipId(void)
{
    return spi_flash_get_id();
}

uint32_t EspClass::getFlashChipRealSize(void)
{
    return (1 << ((spi_flash_get_id() >> 16) & 0xFF));
}

uint32_t EspClass::getFlashChipSize(void)
{
    uint32_t data;
    uint8_t * bytes = (uint8_t *) &data;
    // read first 4 byte (magic byte + flash config)
    if(spi_flash_read(0x0000, &data, 4) == SPI_FLASH_RESULT_OK) {
        return magicFlashChipSize((bytes[3] & 0xf0) >> 4);
    }
    return 0;
}

uint32_t EspClass::getFlashChipSpeed(void)
{
    uint32_t data;
    uint8_t * bytes = (uint8_t *) &data;
    // read first 4 byte (magic byte + flash config)
    if(spi_flash_read(0x0000, &data, 4) == SPI_FLASH_RESULT_OK) {
        return magicFlashChipSpeed(bytes[3] & 0x0F);
    }
    return 0;
}

FlashMode_t EspClass::getFlashChipMode(void)
{
    FlashMode_t mode = FM_UNKNOWN;
    uint32_t data;
    uint8_t * bytes = (uint8_t *) &data;
    // read first 4 byte (magic byte + flash config)
    if(spi_flash_read(0x0000, &data, 4) == SPI_FLASH_RESULT_OK) {
        mode = magicFlashChipMode(bytes[2]);
    }
    return mode;
}

uint32_t EspClass::magicFlashChipSize(uint8_t byte) {
    switch(byte & 0x0F) {
        case 0x0: // 4 Mbit (512KB)
            return (512_kB);
        case 0x1: // 2 MBit (256KB)
            return (256_kB);
        case 0x2: // 8 MBit (1MB)
            return (1_MB);
        case 0x3: // 16 MBit (2MB)
            return (2_MB);
        case 0x4: // 32 MBit (4MB)
            return (4_MB);
        case 0x8: // 64 MBit (8MB)
            return (8_MB);
        case 0x9: // 128 MBit (16MB)
            return (16_MB);
        default: // fail?
            return 0;
    }
}

uint32_t EspClass::magicFlashChipSpeed(uint8_t byte) {
    switch(byte & 0x0F) {
        case 0x0: // 40 MHz
            return (40_MHz);
        case 0x1: // 26 MHz
            return (26_MHz);
        case 0x2: // 20 MHz
            return (20_MHz);
        case 0xf: // 80 MHz
            return (80_MHz);
        default: // fail?
            return 0;
    }
}

FlashMode_t EspClass::magicFlashChipMode(uint8_t byte) {
    FlashMode_t mode = (FlashMode_t) byte;
    if(mode > FM_DOUT) {
        mode = FM_UNKNOWN;
    }
    return mode;
}

/**
 * Infos from
 *  http://www.wlxmall.com/images/stock_item/att/A1010004.pdf
 *  http://www.gigadevice.com/product-series/5.html?locale=en_US
 *  http://www.elinux.org/images/f/f5/Winbond-w25q32.pdf
 */
uint32_t EspClass::getFlashChipSizeByChipId(void) {
    uint32_t chipId = getFlashChipId();
    /**
     * Chip ID
     * 00 - always 00 (Chip ID use only 3 byte)
     * 17 - ? looks like 2^xx is size in Byte ?     //todo: find docu to this
     * 40 - ? may be Speed ?                        //todo: find docu to this
     * C8 - manufacturer ID
     */
    switch(chipId) {

        // GigaDevice
        case 0x1740C8: // GD25Q64B
            return (8_MB);
        case 0x1640C8: // GD25Q32B
            return (4_MB);
        case 0x1540C8: // GD25Q16B
            return (2_MB);
        case 0x1440C8: // GD25Q80
            return (1_MB);
        case 0x1340C8: // GD25Q40
            return (512_kB);
        case 0x1240C8: // GD25Q20
            return (256_kB);
        case 0x1140C8: // GD25Q10
            return (128_kB);
        case 0x1040C8: // GD25Q12
            return (64_kB);

        // Winbond
        case 0x1640EF: // W25Q32
            return (4_MB);
        case 0x1540EF: // W25Q16
            return (2_MB);
        case 0x1440EF: // W25Q80
            return (1_MB);
        case 0x1340EF: // W25Q40
            return (512_kB);

        // BergMicro
        case 0x1640E0: // BG25Q32
            return (4_MB);
        case 0x1540E0: // BG25Q16
            return (2_MB);
        case 0x1440E0: // BG25Q80
            return (1_MB);
        case 0x1340E0: // BG25Q40
            return (512_kB);

        default:
            return 0;
    }
}

/**
 * check the Flash settings from IDE against the Real flash size
 * @param needsEquals (return only true it equals)
 * @return ok or not
 */
bool EspClass::checkFlashConfig(bool needsEquals) {
    if(needsEquals) {
        if(getFlashChipRealSize() == getFlashChipSize()) {
            return true;
        }
    } else {
        if(getFlashChipRealSize() >= getFlashChipSize()) {
            return true;
        }
    }
    return false;
}

String EspClass::getResetReason(void) {
    char buff[32];
    if (resetInfo.reason == REASON_DEFAULT_RST) { // normal startup by power on
      strcpy_P(buff, PSTR("Power on"));
    } else if (resetInfo.reason == REASON_WDT_RST) { // hardware watch dog reset
      strcpy_P(buff, PSTR("Hardware Watchdog"));
    } else if (resetInfo.reason == REASON_EXCEPTION_RST) { // exception reset, GPIO status won’t change
      strcpy_P(buff, PSTR("Exception"));
    } else if (resetInfo.reason == REASON_SOFT_WDT_RST) { // software watch dog reset, GPIO status won’t change
      strcpy_P(buff, PSTR("Software Watchdog"));
    } else if (resetInfo.reason == REASON_SOFT_RESTART) { // software restart ,system_restart , GPIO status won’t change
      strcpy_P(buff, PSTR("Software/System restart"));
    } else if (resetInfo.reason == REASON_DEEP_SLEEP_AWAKE) { // wake up from deep-sleep
      strcpy_P(buff, PSTR("Deep-Sleep Wake"));
    } else if (resetInfo.reason == REASON_EXT_SYS_RST) { // external system reset
      strcpy_P(buff, PSTR("External System"));
    } else {
      strcpy_P(buff, PSTR("Unknown"));
    }
    return String(buff);
}

String EspClass::getResetInfo(void) {
    if(resetInfo.reason != 0) {
        char buff[200];
        sprintf(&buff[0], "Fatal exception:%d flag:%d (%s) epc1:0x%08x epc2:0x%08x epc3:0x%08x excvaddr:0x%08x depc:0x%08x", resetInfo.exccause, resetInfo.reason, (resetInfo.reason == 0 ? "DEFAULT" : resetInfo.reason == 1 ? "WDT" : resetInfo.reason == 2 ? "EXCEPTION" : resetInfo.reason == 3 ? "SOFT_WDT" : resetInfo.reason == 4 ? "SOFT_RESTART" : resetInfo.reason == 5 ? "DEEP_SLEEP_AWAKE" : resetInfo.reason == 6 ? "EXT_SYS_RST" : "???"), resetInfo.epc1, resetInfo.epc2, resetInfo.epc3, resetInfo.excvaddr, resetInfo.depc);
        return String(buff);
    }
    return String("flag: 0");
}

struct rst_info * EspClass::getResetInfoPtr(void) {
    return &resetInfo;
}

bool EspClass::eraseConfig(void) {
    const size_t cfgSize = 0x4000;
    size_t cfgAddr = ESP.getFlashChipSize() - cfgSize;

    for (size_t offset = 0; offset < cfgSize; offset += SPI_FLASH_SEC_SIZE) {
        if (!flashEraseSector((cfgAddr + offset) / SPI_FLASH_SEC_SIZE)) {
            return false;
        }
    }

    return true;
}

uint32_t EspClass::getSketchSize() {
    static uint32_t result = 0;
    if (result)
        return result;

    image_header_t image_header;
    uint32_t pos = APP_START_OFFSET;
    if (spi_flash_read(pos, (uint32_t*) &image_header, sizeof(image_header))) {
        return 0;
    }
    pos += sizeof(image_header);
#ifdef DEBUG_SERIAL
    DEBUG_SERIAL.printf("num_segments=%u\r\n", image_header.num_segments);
#endif
    for (uint32_t section_index = 0;
        section_index < image_header.num_segments;
        ++section_index)
    {
        section_header_t section_header = {0, 0};
        if (spi_flash_read(pos, (uint32_t*) &section_header, sizeof(section_header))) {
            return 0;
        }
        pos += sizeof(section_header);
        pos += section_header.size;
#ifdef DEBUG_SERIAL
        DEBUG_SERIAL.printf("section=%u size=%u pos=%u\r\n", section_index, section_header.size, pos);
#endif
    }
    result = (pos + 16) & ~15;
    return result;
}

extern "C" uint32_t _SPIFFS_start;

uint32_t EspClass::getFreeSketchSpace() {

    uint32_t usedSize = getSketchSize();
    // round one sector up
    uint32_t freeSpaceStart = (usedSize + FLASH_SECTOR_SIZE - 1) & (~(FLASH_SECTOR_SIZE - 1));
    uint32_t freeSpaceEnd = (uint32_t)&_SPIFFS_start - 0x40200000;

#ifdef DEBUG_SERIAL
    DEBUG_SERIAL.printf("usedSize=%u freeSpaceStart=%u freeSpaceEnd=%u\r\n", usedSize, freeSpaceStart, freeSpaceEnd);
#endif
    return freeSpaceEnd - freeSpaceStart;
}

bool EspClass::updateSketch(Stream& in, uint32_t size, bool restartOnFail, bool restartOnSuccess) {
  if(!Update.begin(size)){
#ifdef DEBUG_SERIAL
    DEBUG_SERIAL.print("Update ");
    Update.printError(DEBUG_SERIAL);
#endif
    if(restartOnFail) ESP.restart();
    return false;
  }

  if(Update.writeStream(in) != size){
#ifdef DEBUG_SERIAL
    DEBUG_SERIAL.print("Update ");
    Update.printError(DEBUG_SERIAL);
#endif
    if(restartOnFail) ESP.restart();
    return false;
  }

  if(!Update.end()){
#ifdef DEBUG_SERIAL
    DEBUG_SERIAL.print("Update ");
    Update.printError(DEBUG_SERIAL);
#endif
    if(restartOnFail) ESP.restart();
    return false;
  }

#ifdef DEBUG_SERIAL
    DEBUG_SERIAL.println("Update SUCCESS");
#endif
    if(restartOnSuccess) ESP.restart();
    return true;
}

static const int FLASH_INT_MASK = ((B10 << 8) | B00111010);

bool EspClass::flashEraseSector(uint32_t sector) {
    ets_isr_mask(FLASH_INT_MASK);
    int rc = spi_flash_erase_sector(sector);
    ets_isr_unmask(FLASH_INT_MASK);
    return rc == 0;
}

bool EspClass::flashWrite(uint32_t offset, uint32_t *data, size_t size) {
    ets_isr_mask(FLASH_INT_MASK);
    int rc = spi_flash_write(offset, (uint32_t*) data, size);
    ets_isr_unmask(FLASH_INT_MASK);
    return rc == 0;
}

bool EspClass::flashRead(uint32_t offset, uint32_t *data, size_t size) {
    ets_isr_mask(FLASH_INT_MASK);
    int rc = spi_flash_read(offset, (uint32_t*) data, size);
    ets_isr_unmask(FLASH_INT_MASK);
    return rc == 0;
}

String EspClass::getSketchMD5()
{
    static String result;
    if (result.length()) {
        return result;
    }
    uint32_t lengthLeft = getSketchSize();
    const size_t bufSize = 512;
    std::unique_ptr<uint8_t[]> buf(new uint8_t[bufSize]);
    uint32_t offset = 0;
    if(!buf.get()) {
        return String();
    }
    MD5Builder md5;
    md5.begin();
    while( lengthLeft > 0) {
        size_t readBytes = (lengthLeft < bufSize) ? lengthLeft : bufSize;
        if (!flashRead(offset, reinterpret_cast<uint32_t*>(buf.get()), (readBytes + 3) & ~3)) {
            return String();
        }
        md5.add(buf.get(), readBytes);
        lengthLeft -= readBytes;
        offset += readBytes;
    }
    md5.calculate();
    result = md5.toString();
    return result;
}
