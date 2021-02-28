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

#include "Esp.h"
#include "flash_utils.h"
#include "eboot_command.h"
#include <memory>
#include "interrupts.h"
#include "MD5Builder.h"
#include "umm_malloc/umm_malloc.h"
#include "cont.h"
#include "flash_hal.h"
#include "coredecls.h"
#include "umm_malloc/umm_malloc.h"
// #include "core_esp8266_vm.h"
#include <pgmspace.h>
#include "reboot_uart_dwnld.h"

extern "C" {
#include "user_interface.h"

extern struct rst_info resetInfo;
}


//#define DEBUG_SERIAL Serial

#ifndef PUYA_SUPPORT
  #define PUYA_SUPPORT 1
#endif

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

void EspClass::deepSleepInstant(uint64_t time_us, WakeMode mode)
{
    system_deep_sleep_set_option(static_cast<int>(mode));
    system_deep_sleep_instant(time_us);
    esp_yield();
}

//this calculation was taken verbatim from the SDK api reference for SDK 2.1.0.
//Note: system_rtc_clock_cali_proc() returns a uint32_t, even though system_deep_sleep() takes a uint64_t.
uint64_t EspClass::deepSleepMax()
{
  //cali*(2^31-1)/(2^12)
  return (uint64_t)system_rtc_clock_cali_proc()*(0x80000000-1)/(0x1000);

}

/*
Layout of RTC Memory is as follows:
Ref: Espressif doc 2C-ESP8266_Non_OS_SDK_API_Reference, section 3.3.23 (system_rtc_mem_write)

|<------system data (256 bytes)------->|<-----------------user data (512 bytes)--------------->|

SDK function signature:
bool	system_rtc_mem_read	(
				uint32	des_addr,
				void	*	src_addr,
				uint32	save_size
)

The system data section can't be used by the user, so:
des_addr must be >=64 (i.e.: 256/4) and <192 (i.e.: 768/4)
src_addr is a pointer to data
save_size is the number of bytes to write

For the method interface:
offset is the user block number (block size is 4 bytes) must be >= 0 and <128
data is a pointer to data, 4-byte aligned
size is number of bytes in the block pointed to by data

Same for write

Note: If the Updater class is in play, e.g.: the application uses OTA, the eboot
command will be stored into the first 128 bytes of user data, then it will be
retrieved by eboot on boot. That means that user data present there will be lost.
Ref:
- discussion in PR #5330.
- https://github.com/esp8266/esp8266-wiki/wiki/Memory-Map#memmory-mapped-io-registers
- Arduino/bootloaders/eboot/eboot_command.h RTC_MEM definition
*/

bool EspClass::rtcUserMemoryRead(uint32_t offset, uint32_t *data, size_t size)
{
    if (offset * 4 + size > 512 || size == 0) {
        return false;
    } else {
        return system_rtc_mem_read(64 + offset, data, size);
    }
}

bool EspClass::rtcUserMemoryWrite(uint32_t offset, uint32_t *data, size_t size)
{
    if (offset * 4 + size > 512 || size == 0) {
        return false;
    } else {
        return system_rtc_mem_write(64 + offset, data, size);
    }
}

void EspClass::reset(void)
{
    __real_system_restart_local();
}

void EspClass::restart(void)
{
    system_restart();
    esp_yield();
}

[[noreturn]] void EspClass::rebootIntoUartDownloadMode()
{
	wdtDisable();
	/* disable hardware watchdog */
	CLEAR_PERI_REG_MASK(PERIPHS_HW_WDT, 0x1);

	esp8266RebootIntoUartDownloadMode();
}

uint16_t EspClass::getVcc(void)
{
    esp8266::InterruptLock lock;
    (void)lock;
    return system_get_vdd33();
}

uint32_t EspClass::getFreeHeap(void)
{
    return system_get_free_heap_size();
}

uint16_t EspClass::getMaxFreeBlockSize(void)
{
    return umm_max_block_size();
}

uint32_t EspClass::getFreeContStack()
{
    return cont_get_free_stack(g_pcont);
}

void EspClass::resetFreeContStack()
{
    cont_repaint_stack(g_pcont);
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

uint32_t EspClass::getFlashChipId(void)
{
    static uint32_t flash_chip_id = 0;
    if (flash_chip_id == 0) {
        flash_chip_id = spi_flash_get_id();
    }
    return flash_chip_id;
}

uint8_t EspClass::getFlashChipVendorId(void)
{
    return (getFlashChipId() & 0x000000ff);
}

uint32_t EspClass::getFlashChipRealSize(void)
{
    return (1 << ((spi_flash_get_id() >> 16) & 0xFF));
}

uint32_t EspClass::getFlashChipSize(void)
{
#if AUTOFLASHSIZE
    return getFlashChipRealSize();
#else
    uint32_t data;
    uint8_t * bytes = (uint8_t *) &data;
    // read first 4 byte (magic byte + flash config)
    if(spi_flash_read(0x0000, &data, 4) == SPI_FLASH_RESULT_OK) {
        return magicFlashChipSize((bytes[3] & 0xf0) >> 4);
    }
    return 0;
#endif
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

#if !AUTOFLASHSIZE
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
#endif

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
        case 0x1840EF: // W25Q128
            return (16_MB);
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

        // XMC - Wuhan Xinxin Semiconductor Manufacturing Corp
        case 0x164020: // XM25QH32B
            return (4_MB);

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

// These are defined in the linker script, and filled in by the elf2bin.py util
extern "C" uint32_t __crc_len;
extern "C" uint32_t __crc_val;

bool EspClass::checkFlashCRC() {
    // Dummy CRC fill
    uint32_t z[2];
    z[0] = z[1] = 0;

    uint32_t firstPart = (uintptr_t)&__crc_len - 0x40200000; // How many bytes to check before the 1st CRC val

    // Start the checksum
    uint32_t crc = crc32((const void*)0x40200000, firstPart, 0xffffffff);
    // Pretend the 2 words of crc/len are zero to be idempotent
    crc = crc32(z, 8, crc);
    // Finish the CRC calculation over the rest of flash
    crc = crc32((const void*)(0x40200000 + firstPart + 8), __crc_len - (firstPart + 8), crc);
    return crc == __crc_val;
}


String EspClass::getResetReason(void) {
    const __FlashStringHelper* buff;

    switch(resetInfo.reason) {
        // normal startup by power on
        case REASON_DEFAULT_RST:      buff = F("Power On"); break;
        // hardware watch dog reset
        case REASON_WDT_RST:          buff = F("Hardware Watchdog"); break;
        // exception reset, GPIO status won’t change
        case REASON_EXCEPTION_RST:    buff = F("Exception"); break;
        // software watch dog reset, GPIO status won’t change
        case REASON_SOFT_WDT_RST:     buff = F("Software Watchdog"); break;
        // software restart ,system_restart , GPIO status won’t change
        case REASON_SOFT_RESTART:     buff = F("Software/System restart"); break;
        // wake up from deep-sleep
        case REASON_DEEP_SLEEP_AWAKE: buff = F("Deep-Sleep Wake"); break;
        // // external system reset
        case REASON_EXT_SYS_RST:      buff = F("External System"); break;
        default:                      buff = F("Unknown"); break;
    }
    return String(buff);
}

String EspClass::getResetInfo(void) {
    if (resetInfo.reason >= REASON_WDT_RST && resetInfo.reason <= REASON_SOFT_WDT_RST) {
        char buff[200];
        sprintf_P(buff, PSTR("Fatal exception:%d flag:%d (%s) epc1:0x%08x epc2:0x%08x epc3:0x%08x excvaddr:0x%08x depc:0x%08x"),
            resetInfo.exccause, resetInfo.reason, getResetReason().c_str(),
            resetInfo.epc1, resetInfo.epc2, resetInfo.epc3, resetInfo.excvaddr, resetInfo.depc);
        return String(buff);
    }
    return getResetReason();
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

uint8_t *EspClass::random(uint8_t *resultArray, const size_t outputSizeBytes) const
{
  /**
   * The ESP32 Technical Reference Manual v4.1 chapter 24 has the following to say about random number generation (no information found for ESP8266):
   *
   * "When used correctly, every 32-bit value the system reads from the RNG_DATA_REG register of the random number generator is a true random number.
   * These true random numbers are generated based on the noise in the Wi-Fi/BT RF system.
   * When Wi-Fi and BT are disabled, the random number generator will give out pseudo-random numbers.
   *
   * When Wi-Fi or BT is enabled, the random number generator is fed two bits of entropy every APB clock cycle (normally 80 MHz).
   * Thus, for the maximum amount of entropy, it is advisable to read the random register at a maximum rate of 5 MHz.
   * A data sample of 2 GB, read from the random number generator with Wi-Fi enabled and the random register read at 5 MHz,
   * has been tested using the Dieharder Random Number Testsuite (version 3.31.1).
   * The sample passed all tests."
   *
   * Since ESP32 is the sequal to ESP8266 it is unlikely that the ESP8266 is able to generate random numbers more quickly than 5 MHz when run at a 80 MHz frequency.
   * A maximum random number frequency of 0.5 MHz is used here to leave some margin for possibly inferior components in the ESP8266.
   * It should be noted that the ESP8266 has no Bluetooth functionality, so turning the WiFi off is likely to cause RANDOM_REG32 to use pseudo-random numbers.
   *
   * It is possible that yield() must be called on the ESP8266 to properly feed the hardware random number generator new bits, since there is only one processor core available.
   * However, no feeding requirements are mentioned in the ESP32 documentation, and using yield() could possibly cause extended delays during number generation.
   * Thus only delayMicroseconds() is used below.
   */

  constexpr uint8_t cooldownMicros = 2;
  static uint32_t lastCalledMicros = micros() - cooldownMicros;

  uint32_t randomNumber = 0;

  for(size_t byteIndex = 0; byteIndex < outputSizeBytes; ++byteIndex)
  {
    if(byteIndex % 4 == 0)
    {
      // Old random number has been used up (random number could be exactly 0, so we can't check for that)

      uint32_t timeSinceLastCall = micros() - lastCalledMicros;
      if(timeSinceLastCall < cooldownMicros)
        delayMicroseconds(cooldownMicros - timeSinceLastCall);

      randomNumber = RANDOM_REG32;
      lastCalledMicros = micros();
    }

    resultArray[byteIndex] = randomNumber;
    randomNumber >>= 8;
  }

  return resultArray;
}

uint32_t EspClass::random() const
{
  union { uint32_t b32; uint8_t b8[4]; } result;
  random(result.b8, 4);
  return result.b32;
}

uint32_t EspClass::getSketchSize() {
    static uint32_t result = 0;
    if (result)
        return result;

    image_header_t image_header;
    uint32_t pos = APP_START_OFFSET;
    if (spi_flash_read(pos, (uint32_t*) &image_header, sizeof(image_header)) != SPI_FLASH_RESULT_OK) {
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
        if (spi_flash_read(pos, (uint32_t*) &section_header, sizeof(section_header)) != SPI_FLASH_RESULT_OK) {
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

uint32_t EspClass::getFreeSketchSpace() {

    uint32_t usedSize = getSketchSize();
    // round one sector up
    uint32_t freeSpaceStart = (usedSize + FLASH_SECTOR_SIZE - 1) & (~(FLASH_SECTOR_SIZE - 1));
    uint32_t freeSpaceEnd = (uint32_t)FS_start - 0x40200000;

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
    int rc = spi_flash_erase_sector(sector);
    return rc == 0;
}

#if PUYA_SUPPORT
static SpiFlashOpResult spi_flash_write_puya(uint32_t offset, uint32_t *data, size_t size) {
    if (data == nullptr) {
      return SPI_FLASH_RESULT_ERR;
    }
    if (size % 4 != 0) {
      return SPI_FLASH_RESULT_ERR;
    }
    // PUYA flash chips need to read existing data, update in memory and write modified data again.
    static uint32_t *flash_write_puya_buf = nullptr;

    if (flash_write_puya_buf == nullptr) {
        flash_write_puya_buf = (uint32_t*) malloc(FLASH_PAGE_SIZE);
        // No need to ever free this, since the flash chip will never change at runtime.
        if (flash_write_puya_buf == nullptr) {
            // Memory could not be allocated.
            return SPI_FLASH_RESULT_ERR;
        }
    }

    SpiFlashOpResult rc = SPI_FLASH_RESULT_OK;
    uint32_t* ptr = data;
    size_t bytesLeft = size;
    uint32_t pos = offset;
    while (bytesLeft > 0 && rc == SPI_FLASH_RESULT_OK) {
        size_t bytesNow = bytesLeft;
        if (bytesNow > FLASH_PAGE_SIZE) {
            bytesNow = FLASH_PAGE_SIZE;
            bytesLeft -= FLASH_PAGE_SIZE;
        } else {
            bytesLeft = 0;
        }
        rc = spi_flash_read(pos, flash_write_puya_buf, bytesNow);
        if (rc != SPI_FLASH_RESULT_OK) {
            return rc;
        }
        for (size_t i = 0; i < bytesNow / 4; ++i) {
            flash_write_puya_buf[i] &= *ptr;
            ++ptr;
        }
        rc = spi_flash_write(pos, flash_write_puya_buf, bytesNow);
        pos += bytesNow;
    }
    return rc;
}
#endif

bool EspClass::flashReplaceBlock(uint32_t address, const uint8_t *value, uint32_t byteCount) {
    uint32_t alignedAddress = (address & ~3);
    uint32_t alignmentOffset = address - alignedAddress;

    if (alignedAddress != ((address + byteCount - 1) & ~3)) {
        // Only one 4 byte block is supported
        return false;
    }
#if PUYA_SUPPORT
    if (getFlashChipVendorId() == SPI_FLASH_VENDOR_PUYA) {
        uint8_t tempData[4] __attribute__((aligned(4)));
        if (spi_flash_read(alignedAddress, (uint32_t *)tempData, 4) != SPI_FLASH_RESULT_OK) {
            return false;
        }
        for (size_t i = 0; i < byteCount; i++) {
            tempData[i + alignmentOffset] &= value[i];
        }
        if (spi_flash_write(alignedAddress, (uint32_t *)tempData, 4) != SPI_FLASH_RESULT_OK) {
            return false;
        }
    }
    else
#endif // PUYA_SUPPORT
    {
        uint32_t tempData;
        if (spi_flash_read(alignedAddress, &tempData, 4) != SPI_FLASH_RESULT_OK) {
            return false;
        }
        memcpy((uint8_t *)&tempData + alignmentOffset, value, byteCount);
        if (spi_flash_write(alignedAddress, &tempData, 4) != SPI_FLASH_RESULT_OK) {
            return false;
        }
    }
    return true;
}

size_t EspClass::flashWriteUnalignedMemory(uint32_t address, const uint8_t *data, size_t size) {
    size_t sizeLeft = (size & ~3);
    size_t currentOffset = 0;
    // Memory is unaligned, so we need to copy it to an aligned buffer
    uint32_t alignedData[FLASH_PAGE_SIZE / sizeof(uint32_t)] __attribute__((aligned(4)));
    // Handle page boundary
    bool pageBreak = ((address % 4) != 0) && ((address / FLASH_PAGE_SIZE) != ((address + sizeLeft - 1) / FLASH_PAGE_SIZE));

    if (pageBreak) {
        size_t byteCount = 4 - (address % 4);

        if (!flashReplaceBlock(address, data, byteCount)) {
            return 0;
        }
        // We will now have aligned address, so we can cross page boundaries
        currentOffset += byteCount;
        // Realign size to 4
        sizeLeft = (size - byteCount) & ~3;
    }

    while (sizeLeft) {
        size_t willCopy = std::min(sizeLeft, sizeof(alignedData));
        memcpy(alignedData, data + currentOffset, willCopy);
        // We now have address, data and size aligned to 4 bytes, so we can use aligned write
        if (!flashWrite(address + currentOffset, alignedData, willCopy))
        {
            return 0;
        }
        sizeLeft -= willCopy;
        currentOffset += willCopy;
    }

    return currentOffset;
}

bool EspClass::flashWritePageBreak(uint32_t address, const uint8_t *data, size_t size) {
    if (size > 4) {
        return false;
    }
    size_t pageLeft = FLASH_PAGE_SIZE - (address % FLASH_PAGE_SIZE);
    size_t offset = 0;
    size_t sizeLeft = size;
    if (pageLeft > 3) {
        return false;
    }

    if (!flashReplaceBlock(address, data, pageLeft)) {
        return false;
    }
    offset += pageLeft;
    sizeLeft -= pageLeft;
    // We replaced last 4-byte block of the page, now we write the remainder in next page
    if (!flashReplaceBlock(address + offset, data + offset, sizeLeft)) {
        return false;
    }
    return true;
}

bool EspClass::flashWrite(uint32_t address, const uint32_t *data, size_t size) {
    SpiFlashOpResult rc = SPI_FLASH_RESULT_OK;
    bool pageBreak = ((address % 4) != 0 && (address / FLASH_PAGE_SIZE) != ((address + size - 1) / FLASH_PAGE_SIZE));

    if ((uintptr_t)data % 4 != 0 || size % 4 != 0 || pageBreak) {
        return false;
    }
#if PUYA_SUPPORT
    if (getFlashChipVendorId() == SPI_FLASH_VENDOR_PUYA) {
        rc = spi_flash_write_puya(address, const_cast<uint32_t *>(data), size);
    }
    else
#endif // PUYA_SUPPORT
    {
        rc = spi_flash_write(address, const_cast<uint32_t *>(data), size);
    }
    return rc == SPI_FLASH_RESULT_OK;
}

bool EspClass::flashWrite(uint32_t address, const uint8_t *data, size_t size) {
    if (size == 0) {
        return true;
    }

    size_t sizeLeft = size & ~3;
    size_t currentOffset = 0;

    if (sizeLeft) {
        if ((uintptr_t)data % 4 != 0) {
            size_t written = flashWriteUnalignedMemory(address, data, size);
            if (!written) {
                return false;
            }
            currentOffset += written;
            sizeLeft -= written;
        } else {
            bool pageBreak = ((address % 4) != 0 && (address / FLASH_PAGE_SIZE) != ((address + sizeLeft - 1) / FLASH_PAGE_SIZE));

            if (pageBreak) {
                while (sizeLeft) {
                    // We cannot cross page boundary, but the write must be 4 byte aligned,
                    // so this is the maximum amount we can write
                    size_t pageBoundary = (FLASH_PAGE_SIZE - ((address + currentOffset) % FLASH_PAGE_SIZE)) & ~3;

                    if (sizeLeft > pageBoundary) {
                        // Aligned write up to page boundary
                        if (!flashWrite(address + currentOffset, (uint32_t *)(data + currentOffset), pageBoundary)) {
                            return false;
                        }
                        currentOffset += pageBoundary;
                        sizeLeft -= pageBoundary;
                        // Cross the page boundary
                        if (!flashWritePageBreak(address + currentOffset, data + currentOffset, 4)) {
                            return false;
                        }
                        currentOffset += 4;
                        sizeLeft -= 4;
                    } else {
                        // We do not cross page boundary
                        if (!flashWrite(address + currentOffset, (uint32_t *)(data + currentOffset), sizeLeft)) {
                            return false;
                        }
                        currentOffset += sizeLeft;
                        sizeLeft = 0;
                    }
                }
            } else {
                // Pointer is properly aligned and write does not cross page boundary,
                // so use aligned write
                if (!flashWrite(address, (uint32_t *)data, sizeLeft)) {
                    return false;
                }
                currentOffset = sizeLeft;
                sizeLeft = 0;
            }
        }
    }
    sizeLeft = size - currentOffset;
    if (sizeLeft > 0) {
        // Size was not aligned, so we have some bytes left to write, we also need to recheck for
        // page boundary crossing
        bool pageBreak = ((address % 4) != 0 && (address / FLASH_PAGE_SIZE) != ((address + sizeLeft - 1) / FLASH_PAGE_SIZE));

        if (pageBreak) {
            // Cross the page boundary
            if (!flashWritePageBreak(address + currentOffset, data + currentOffset, sizeLeft)) {
                return false;
            }
        } else {
            // Just write partial block
            flashReplaceBlock(address + currentOffset, data + currentOffset, sizeLeft);
        }
    }

    return true;
}

bool EspClass::flashRead(uint32_t address, uint8_t *data, size_t size) {
    size_t sizeAligned = size & ~3;
    size_t currentOffset = 0;

    if ((uintptr_t)data % 4 != 0) {
        uint32_t alignedData[FLASH_PAGE_SIZE / sizeof(uint32_t)] __attribute__((aligned(4)));
        size_t sizeLeft = sizeAligned;

        while (sizeLeft) {
            size_t willCopy = std::min(sizeLeft, sizeof(alignedData));
            // We read to our aligned buffer and then copy to data
            if (!flashRead(address + currentOffset, alignedData, willCopy))
            {
                return false;
            }
            memcpy(data + currentOffset, alignedData, willCopy);
            sizeLeft -= willCopy;
            currentOffset += willCopy;
        }
    } else {
        // Pointer is properly aligned, so use aligned read
        if (!flashRead(address, (uint32_t *)data, sizeAligned)) {
            return false;
        }
        currentOffset = sizeAligned;
    }

    if (currentOffset < size) {
        uint32_t tempData;
        if (spi_flash_read(address + currentOffset, &tempData, 4) != SPI_FLASH_RESULT_OK) {
            return false;
        }
        memcpy((uint8_t *)data + currentOffset, &tempData, size - currentOffset);
    }

    return true;
}

bool EspClass::flashRead(uint32_t address, uint32_t *data, size_t size) {
    if ((uintptr_t)data % 4 != 0 || size % 4 != 0) {
        return false;
    }
    return (spi_flash_read(address, data, size) == SPI_FLASH_RESULT_OK);
}

String EspClass::getSketchMD5()
{
    static String result;
    if (result.length()) {
        return result;
    }
    uint32_t lengthLeft = getSketchSize();
    const size_t bufSize = 512;
    std::unique_ptr<uint8_t[]> buf(new (std::nothrow) uint8_t[bufSize]);
    uint32_t offset = 0;
    if(!buf.get()) {
        return emptyString;
    }
    MD5Builder md5;
    md5.begin();
    while( lengthLeft > 0) {
        size_t readBytes = (lengthLeft < bufSize) ? lengthLeft : bufSize;
        if (!flashRead(offset, reinterpret_cast<uint32_t*>(buf.get()), (readBytes + 3) & ~3)) {
            return emptyString;
        }
        md5.add(buf.get(), readBytes);
        lengthLeft -= readBytes;
        offset += readBytes;
    }
    md5.calculate();
    result = md5.toString();
    return result;
}

void EspClass::enableVM()
{
#ifdef UMM_HEAP_EXTERNAL
    if (!vmEnabled)
        install_vm_exception_handler();
    vmEnabled = true;
#endif
}

void EspClass::setExternalHeap()
{
#ifdef UMM_HEAP_EXTERNAL
    if (vmEnabled) {
        if (!umm_push_heap(UMM_HEAP_EXTERNAL)) {
            panic();
        }
    }
#endif
}

void EspClass::setIramHeap()
{
#ifdef UMM_HEAP_IRAM
    if (!umm_push_heap(UMM_HEAP_IRAM)) {
        panic();
    }
#endif
}

void EspClass::setDramHeap()
{
#if defined(UMM_HEAP_EXTERNAL) && !defined(UMM_HEAP_IRAM)
    if (vmEnabled) {
        if (!umm_push_heap(UMM_HEAP_DRAM)) {
            panic();
        }
    }
#elif defined(UMM_HEAP_IRAM)
    if (!umm_push_heap(UMM_HEAP_DRAM)) {
        panic();
    }
#endif
}

void EspClass::resetHeap()
{
#if defined(UMM_HEAP_EXTERNAL) && !defined(UMM_HEAP_IRAM)
    if (vmEnabled) {
        if (!umm_pop_heap()) {
            panic();
        }
    }
#elif defined(UMM_HEAP_IRAM)
    if (!umm_pop_heap()) {
        panic();
    }
#endif
}
