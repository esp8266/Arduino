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
/**
 * AVR macros for WDT managment
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

enum WakeMode {
    WAKE_RF_DEFAULT = 0, // RF_CAL or not after deep-sleep wake up, depends on init data byte 108.
    WAKE_RFCAL = 1,      // RF_CAL after deep-sleep wake up, there will be large current.
    WAKE_NO_RFCAL = 2,   // no RF_CAL after deep-sleep wake up, there will only be small current.
    WAKE_RF_DISABLED = 4 // disable RF after deep-sleep wake up, just like modem sleep, there will be the smallest current.
};

typedef enum {
     FM_QIO = 0x00,
     FM_QOUT = 0x01,
     FM_DIO = 0x02,
     FM_DOUT = 0x03,
     FM_UNKNOWN = 0xff
} FlashMode_t;

class EspClass {
    public:
        EspClass();

        // TODO: figure out how to set WDT timeout
        void wdtEnable(uint32_t timeout_ms = 0);
        // note: setting the timeout value is not implemented at the moment
        void wdtEnable(WDTO_t timeout_ms = WDTO_0MS);

        void wdtDisable(void);
        void wdtFeed(void);

        void deepSleep(uint32_t time_us, WakeMode mode = WAKE_RF_DEFAULT);

        void reset(void);
        void restart(void);

        uint16_t getVcc(void);
        uint32_t getFreeHeap(void);

        uint32_t getChipId(void);

        const char * getSdkVersion(void);

        uint8_t getBootVersion(void);
        uint8_t getBootMode(void);

        uint8_t getCpuFreqMHz(void);

        uint32_t getFlashChipId(void);
        uint32_t getFlashChipSize(void);
        uint32_t getFlashChipSpeed(void);
        FlashMode_t getFlashChipMode(void);
        uint32_t getFlashChipSizeByChipId(void);

        inline uint32_t getCycleCount(void);
};

uint32_t EspClass::getCycleCount(void)
{
    uint32_t ccount;
    __asm__ __volatile__("rsr %0,ccount":"=a" (ccount));
    return ccount;
}

extern EspClass ESP;

#endif //ESP_H
