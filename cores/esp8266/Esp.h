/*
 Esp.h - ESP8266-specific APIs
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

#ifndef ESP_H
#define ESP_H

#include <Arduino.h>
#include "core_esp8266_features.h"
#include "spi_vendors.h"

/**
 * AVR macros for WDT management
 */
typedef enum {
    WDTO_0MS    = 0,   //!< WDTO_0MS
    WDTO_15MS   = 15,  //!< WDTO_15MS
    WDTO_30MS   = 30,  //!< WDTO_30MS
    WDTO_60MS   = 60,  //!< WDTO_60MS
    WDTO_120MS  = 120, //!< WDTO_120MS
    WDTO_250MS  = 250, //!< WDTO_250MS
    WDTO_500MS  = 500, //!< WDTO_500MS
    WDTO_1S     = 1000,//!< WDTO_1S
    WDTO_2S     = 2000,//!< WDTO_2S
    WDTO_4S     = 4000,//!< WDTO_4S
    WDTO_8S     = 8000 //!< WDTO_8S
} WDTO_t;


#define wdt_enable(time)    ESP.wdtEnable(time)
#define wdt_disable()       ESP.wdtDisable()
#define wdt_reset()         ESP.wdtFeed()

#define cli()        ets_intr_lock()       // IRQ Disable
#define sei()        ets_intr_unlock()     // IRQ Enable

enum RFMode {
    RF_DEFAULT = 0, // RF_CAL or not after deep-sleep wake up, depends on init data byte 108.
    RF_CAL = 1,      // RF_CAL after deep-sleep wake up, there will be large current.
    RF_NO_CAL = 2,   // no RF_CAL after deep-sleep wake up, there will only be small current.
    RF_DISABLED = 4 // disable RF after deep-sleep wake up, just like modem sleep, there will be the smallest current.
};

#define RF_PRE_INIT() void __run_user_rf_pre_init()

// compatibility definitions
#define WakeMode RFMode
#define WAKE_RF_DEFAULT  RF_DEFAULT
#define WAKE_RFCAL       RF_CAL
#define WAKE_NO_RFCAL    RF_NO_CAL
#define WAKE_RF_DISABLED RF_DISABLED

enum ADCMode {
    ADC_TOUT = 33,
    ADC_TOUT_3V3 = 33,
    ADC_VCC = 255,
    ADC_VDD = 255
};

#define ADC_MODE(mode) int __get_adc_mode(void) { return (int) (mode); }

typedef enum {
     FM_QIO = 0x00,
     FM_QOUT = 0x01,
     FM_DIO = 0x02,
     FM_DOUT = 0x03,
     FM_UNKNOWN = 0xff
} FlashMode_t;

class EspClass {
    public:
        // TODO: figure out how to set WDT timeout
        static void wdtEnable(uint32_t timeout_ms = 0);
        // note: setting the timeout value is not implemented at the moment
        static void wdtEnable(WDTO_t timeout_ms = WDTO_0MS);

        static void wdtDisable();
        static void wdtFeed();

        static void deepSleep(uint64_t time_us);
        static void deepSleepInstant(uint64_t time_us);
        static uint64_t deepSleepMax();

        static bool forcedModemSleep(uint32_t duration_us = 0, void (*wakeupCb)() = nullptr);
        /// The prior sleep type is restored, but only as automatic.
        /// If any forced sleep mode was effective before forcedModemSleep,
        ///  it would have to be restored explicitly.
        static void forcedModemSleepOff();

        static bool forcedLightSleepBegin(uint32_t duration_us = 0, void (*wakeupCb)() = nullptr);
        /// The prior sleep type is restored, but only as automatic.
        /// If any forced sleep mode was effective before forcedLightSleepBegin,
        ///  it would have to be restored explicitly.
        static void forcedLightSleepEnd(bool cancel = false);

        static void autoModemSleep();
        static void autoLightSleep();
        /// The prior sleep type is restored, but only as automatic.
        /// If any forced sleep mode was effective before auto{Modem,Light}Sleep,
        ///  it would have to be restored explicitly.
        static void autoSleepOff();

        static void neverSleep();
        /// Any prior sleep type is restored, but only as automatic.
        /// If any forced sleep mode was effective before neverSleep,
        ///  it would have to be restored explicitly.
        static void neverSleepOff();

        static bool rtcUserMemoryRead(uint32_t offset, uint32_t *data, size_t size);
        static bool rtcUserMemoryWrite(uint32_t offset, uint32_t *data, size_t size);

        static void reset();
        static void restart();
        /**
         * @brief When calling this method the ESP8266 reboots into the UART download mode without
         * the need of any external wiring. This is the same mode which can also be entered by
         * pulling GPIO0=low, GPIO2=high, GPIO15=low and resetting the ESP8266.
         */
        [[noreturn]] static void rebootIntoUartDownloadMode();

        static uint16_t getVcc();
        static uint32_t getChipId();

        static uint32_t getFreeHeap();
#if defined(UMM_INFO)
        static uint32_t getMaxFreeBlockSize();
        static uint8_t getHeapFragmentation(); // in %
        static void getHeapStats(uint32_t* free = nullptr, uint16_t* max = nullptr, uint8_t* frag = nullptr) __attribute__((deprecated("Use 'uint32_t*' on max, 2nd argument")));
        static void getHeapStats(uint32_t* free = nullptr, uint32_t* max = nullptr, uint8_t* frag = nullptr);
#endif
        static uint32_t getFreeContStack();
        static void resetFreeContStack();

        static const char * getSdkVersion();
        static String getCoreVersion();
        static String getFullVersion();

        static uint8_t getBootVersion();
        static uint8_t getBootMode();

#if defined(F_CPU) || defined(CORE_MOCK)
        constexpr
#endif
            static inline uint8_t getCpuFreqMHz() __attribute__((always_inline))
        {
            return esp_get_cpu_freq_mhz();
        }

        static uint32_t getFlashChipId();
        static uint8_t getFlashChipVendorId();

        //gets the actual chip size based on the flash id
        static uint32_t getFlashChipRealSize();
        //gets the size of the flash as set by the compiler
        static uint32_t getFlashChipSize();
        static uint32_t getFlashChipSpeed();
        static FlashMode_t getFlashChipMode();
        static uint32_t getFlashChipSizeByChipId();

        static uint32_t magicFlashChipSize(uint8_t byte);
        static uint32_t magicFlashChipSpeed(uint8_t byte);
        static FlashMode_t magicFlashChipMode(uint8_t byte);

        static bool checkFlashConfig(bool needsEquals = false);

        static bool checkFlashCRC();

        static bool flashEraseSector(uint32_t sector);
        /**
         * @brief Write @a size bytes from @a data to flash at @a address
         * This overload requires @a data and @a size to be always 4 byte aligned and
         * @a address to be 4 byte aligned if the write crossess page boundary,
         * but guarantees no overhead (except on PUYA flashes)
         * @param address address on flash where write should start, 4 byte alignment is conditional
         * @param data input buffer, must be 4-byte aligned
         * @param size amount of data, must be a multiple of 4
         * @return bool result of operation
         * @retval true success
         * @retval false failure to write to flash or incorrect alignment of params
         */
        static bool flashWrite(uint32_t address, const uint32_t *data, size_t size);
        /**
         * @brief Write @a size bytes from @a data to flash at @a address
         * This overload handles all misalignment cases
         * @param address address on flash where write should start
         * @param data input buffer, passing unaligned memory will cause significant stack usage
         * @param size amount of data, passing not multiple of 4 will cause additional reads and writes
         * @return bool result of operation
         */
        static bool flashWrite(uint32_t address, const uint8_t *data, size_t size);
        /**
         * @brief Read @a size bytes to @a data to flash at @a address
         * This overload requires @a data and @a size to be 4 byte aligned
         * @param address address on flash where read should start
         * @param data input buffer, must be 4-byte aligned
         * @param size amount of data, must be a multiple of 4
         * @return bool result of operation
         * @retval true success
         * @retval false failure to read from flash or incorrect alignment of params
         */
        static bool flashRead(uint32_t address, uint32_t *data, size_t size);
        /**
         * @brief Read @a size bytes to @a data to flash at @a address
         * This overload handles all misalignment cases
         * @param address address on flash where read should start
         * @param data input buffer, passing unaligned memory will cause significant stack usage
         * @param size amount of data, passing not multiple of 4 will cause additional read
         * @return bool result of operation
         */
        static bool flashRead(uint32_t address, uint8_t *data, size_t size);

        static uint32_t getSketchSize();
        static String getSketchMD5();
        static uint32_t getFreeSketchSpace();
        static bool updateSketch(Stream& in, uint32_t size, bool restartOnFail = false, bool restartOnSuccess = true);

        static String getResetReason();
        static String getResetInfo();
        static struct rst_info * getResetInfoPtr();

        static bool eraseConfig();

        static uint8_t *random(uint8_t *resultArray, const size_t outputSizeBytes);
        static uint32_t random();

#if !defined(CORE_MOCK)
        static inline uint32_t getCycleCount() __attribute__((always_inline))
        {
            return esp_get_cycle_count();
        }
#else
        static uint32_t getCycleCount();
#endif // !defined(CORE_MOCK)
        /**
         * @brief Push current Heap selection and set Heap selection to DRAM.
         *
         * @param none
         * @return none
         */
        static void setDramHeap();
        /**
         * @brief Push current Heap selection and set Heap selection to IRAM.
         *
         * @param none
         * @return none
         */
        static void setIramHeap();
        /**
         * @brief Push current Heap selection and set Heap selection to External. (Experimental)
         *
         * @param none
         * @return none
         */
        static void setExternalHeap();
        /**
         * @brief Restores Heap selection back to value present when
         * setDramHeap, setIramHeap, or setExternalHeap was called.
         *
         * @param none
         * @return none
         */
        static void resetHeap();
    private:
        /**
         * @brief Write up to @a size bytes from @a data to flash at @a address
         * This function handles all cases of unaligned memory acccess; when either
         * address is not aligned, data pointer is not aligned or size is not a multiple of 4.
         * User of this function should note that @a data will be copied into a buffer allocated on stack.
         *
         * @param address address on flash where write should start
         * @param data input buffer
         * @param size amount of data
         * @return size_t amount of data written, 0 on failure
         */
        static size_t flashWriteUnalignedMemory(uint32_t address, const uint8_t *data, size_t size);
};

extern EspClass ESP;

#endif //ESP_H
