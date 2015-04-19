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


enum WakeMode {
    WAKE_RF_DEFAULT = 0, // RF_CAL or not after deep-sleep wake up, depends on init data byte 108.
    WAKE_RFCAL = 1,      // RF_CAL after deep-sleep wake up, there will be large current.
    WAKE_NO_RFCAL = 2,   // no RF_CAL after deep-sleep wake up, there will only be small current.
    WAKE_RF_DISABLED = 4 // disable RF after deep-sleep wake up, just like modem sleep, there will be the smallest current.
};

class EspClass {
    public:
        EspClass();

        void wdtEnable(int timeout_ms = 0);  
        // TODO: figure out how to set WDT timeout
        void wdtDisable(void);
        void wdtFeed(void);

        void deepSleep(uint32_t time_us, WakeMode mode = WAKE_RF_DEFAULT);


        void reset(void);
        void restart(void);
        uint16_t getVCC(void);
};

extern EspClass ESP;

#endif //ESP_H
