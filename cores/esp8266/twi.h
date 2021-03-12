/*
    twi.h - Software I2C library for esp8266

    Copyright (c) 2015 Hristo Gochkov. All rights reserved.
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
    Modified January 2017 by Bjorn Hammarberg (bjoham@esp8266.com) - i2c slave support
*/
#ifndef SI2C_h
#define SI2C_h
#include "Arduino.h"

#ifdef __cplusplus
extern "C" {
#endif

#define I2C_OK                      0
#define I2C_SCL_HELD_LOW            1
#define I2C_SCL_HELD_LOW_AFTER_READ 2
#define I2C_SDA_HELD_LOW            3
#define I2C_SDA_HELD_LOW_AFTER_INIT 4

#ifndef TWI_BUFFER_LENGTH
#define TWI_BUFFER_LENGTH 32
#endif

void twi_init(unsigned char sda, unsigned char scl);
void twi_setAddress(uint8_t);
void twi_stop(void);
void twi_setClock(unsigned int freq);
void twi_setClockStretchLimit(uint32_t limit);
uint8_t twi_writeTo(unsigned char address, unsigned char * buf, unsigned int len, unsigned char sendStop);
uint8_t twi_readFrom(unsigned char address, unsigned char * buf, unsigned int len, unsigned char sendStop);
uint8_t twi_status();

uint8_t twi_transmit(const uint8_t*, uint8_t);

void twi_attachSlaveRxEventWithTarget(void (*)(uint8_t*, size_t, void*));
void twi_attachSlaveTxEventWithTarget(void (*)(void*));
void twi_reply(uint8_t);
//void twi_stop(void);
void twi_releaseBus(void);

void twi_enableSlaveModeWithTarget(void* targetObject);

inline void twi_attachSlaveRxEvent(void (*cb)(uint8_t*, size_t))
{   // force cast to the previous version of the callback
    // the ESP8266 convention should be fine with that:
    // http://naberius.de/2015/05/14/esp8266-gpio-output-performance/
    // https://boredpentester.com/reversing-esp8266-firmware-part-5/
    twi_attachSlaveRxEventWithTarget((void (*)(uint8_t*, size_t, void*))(void*)cb);
}
inline void twi_attachSlaveTxEvent(void (*cb)(void))
{   // force cast to the previous version of the callback
    // the ESP8266 convention should be fine with that:
    // http://naberius.de/2015/05/14/esp8266-gpio-output-performance/
    // https://boredpentester.com/reversing-esp8266-firmware-part-5/
    twi_attachSlaveTxEventWithTarget((void (*)(void*))(void*)cb);
}
inline void twi_enableSlaveMode(void)
{
    twi_enableSlaveModeWithTarget(NULL);
}

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

// this is a C++ class, so declare it only in C++ context

class TwiMaster
{
protected:
    unsigned int preferred_si2c_clock = 100000;
    uint32_t twi_dcount = 18;
    unsigned char twi_sda = 0;
    unsigned char twi_scl = 0;
    uint32_t twi_clockStretchLimit = 0;

    // Internal use functions
    void ICACHE_RAM_ATTR busywait(unsigned int v);
    bool write_start(void);
    bool write_stop(void);
    bool write_bit(bool bit);
    bool read_bit(void);
    bool write_byte(unsigned char byte);
    unsigned char read_byte(bool nack);

    // Handle the case where a slave needs to stretch the clock with a time-limited busy wait
    inline void WAIT_CLOCK_STRETCH();

    // Generate a clock "valley" (at the end of a segment, just before a repeated start)
    void twi_scl_valley(void);

public:
    void setClock(unsigned int freq);
    void setClockStretchLimit(uint32_t limit);
    void init(unsigned char sda, unsigned char scl);
    unsigned char writeTo(unsigned char address, unsigned char * buf, unsigned int len, unsigned char sendStop);
    unsigned char readFrom(unsigned char address, unsigned char* buf, unsigned int len, unsigned char sendStop);
    uint8_t status();
};

extern TwiMaster& twiMasterSingleton;

#endif

#endif
