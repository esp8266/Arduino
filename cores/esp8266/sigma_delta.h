/* 
 sigma_delta.h - esp8266 sigma-delta source

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
 
 /*******************************************************************************
 * Info Sigma delta module

This module controls the esp8266 internal sigma delta source
Each pin can be connected to the sigma delta source
The target duty and frequency can be modified via the register GPIO_SIGMA_DELTA

THE TARGET FREQUENCY IS DEFINED AS:

FREQ = 80,000,000/prescaler * target /256  HZ,     0<target<128
FREQ = 80,000,000/prescaler * (256-target) /256  HZ,     128<target<256
target: duty cycle,range 0-255
prescaler: is a clock divider, range 0-255
so the target and prescale will both affect the freq.
CPU_FREQ has no influence on the sigma delta frequency.

Usage :
1. sigmaDeltaSetup(0,f) : activate the sigma delta source with frequency f and default duty cycle (0)
2. sigmaDeltaAttachPin(pin), any pin 0..15, TBC if gpio16 supports sigma-delta source
     This will set the pin to NORMAL output mode (pinMode(pin,OUTPUT))
3. sigmaDeltaWrite(0,dc) : set the output signal duty cycle, duty cycle = dc/256

*******************************************************************************/

#ifndef SIGMA_DELTA_H
#define SIGMA_DELTA_H

#ifdef __cplusplus
extern "C" {
#endif

//channel parameter is unused (only for ESP32 compatibility) freq 1220-312500 duty 0-255
void        sigmaDeltaEnable(void);
void        sigmaDeltaDisable(void);
uint32_t    sigmaDeltaSetup(uint8_t channel, uint32_t freq);
void        sigmaDeltaWrite(uint8_t channel, uint8_t duty);
uint8_t     sigmaDeltaRead(uint8_t channel = 0);
void        sigmaDeltaAttachPin(uint8_t pin, uint8_t channel = 0);
void        sigmaDeltaDetachPin(uint8_t pin);
bool        sigmaDeltaIsPinAttached(uint8_t pin);

// alternative way to control the sigma delta generator frequency
uint8_t     sigmaDeltaGetPrescaler(void);
void        sigmaDeltaSetPrescaler(uint8_t prescaler);


#ifdef __cplusplus
}
#endif

#endif//SIGMA_DELTA_H
