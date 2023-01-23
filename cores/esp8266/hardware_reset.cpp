/*
  Make the reset look like an EXT_RST reset by:
    * Set INTLEVEL to 15 blocking NMI Software WDT interference
    * set "restart reason" to REASON_EXT_SYS_RST
    * Config Hardware WDT for 1.6ms
    * Disable Hardware WDT Level-1 interrupt option
    * wait, ...

  Inspired by RTOS SDK hardware_restart in panic.c
*/

#include "Arduino.h"
#include <user_interface.h>
#include <ets_sys.h>
#include "hardware_reset.h"


// Extracted from RTOS_SDK eagle_soc.h
/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2015 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */
#define REG_WRITE(_r, _v)       (*(volatile uint32_t *)(_r)) = (_v)
#define REG_READ(_r)            (*(volatile uint32_t *)(_r))

//Watchdog reg {{
#define PERIPHS_WDT_BASEADDR        0x60000900

#define WDT_CTL_ADDRESS             0
#define WDT_OP_ADDRESS              0x4
#define WDT_OP_ND_ADDRESS           0x8
#define WDT_RST_ADDRESS             0x14

#define WDT_CTL_RSTLEN_MASK         0x38
#define WDT_CTL_RSPMOD_MASK         0x6
#define WDT_CTL_EN_MASK             0x1

#define WDT_CTL_RSTLEN_LSB          0x3
#define WDT_CTL_RSPMOD_LSB          0x1
#define WDT_CTL_EN_LSB              0

#define WDT_FEED_VALUE              0x73

#define WDT_REG_READ(_reg)                  REG_READ(PERIPHS_WDT_BASEADDR + _reg)
#define WDT_REG_WRITE(_reg, _val)           REG_WRITE(PERIPHS_WDT_BASEADDR + _reg, _val)
#define CLEAR_WDT_REG_MASK(_reg, _mask)     WDT_REG_WRITE(_reg, WDT_REG_READ(_reg) & (~_mask))
#define SET_WDT_REG_MASK(_reg, _mask, _val) SET_PERI_REG_BITS((PERIPHS_WDT_BASEADDR + _reg), _mask, _val, 0)
#undef WDT_FEED
#define WDT_FEED()                          WDT_REG_WRITE(WDT_RST_ADDRESS, WDT_FEED_VALUE)
//}}

// Inspired by RTOS SDK task_wdt.c and hardware_restart in panic.c

// Copyright 2018-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

extern "C" {
  [[noreturn]] void hardware_reset(void) {
    volatile uint32_t* const rtc_mem = (volatile uint32_t *)0x60001100u;

    // Block NMI WDT from disturbing out restart reason
    xt_rsil(15);

    // SDK restart reason location
    rtc_mem[0] = REASON_EXT_SYS_RST;

    // Disable WDT
    CLEAR_WDT_REG_MASK(WDT_CTL_ADDRESS, WDT_CTL_EN_MASK);

    // Set Reset pulse to maximum
    // Select Reset only - no level-1 interrupt
    SET_WDT_REG_MASK(WDT_CTL_ADDRESS,
                     WDT_CTL_RSTLEN_MASK       | WDT_CTL_RSPMOD_MASK,
                     (7 << WDT_CTL_RSTLEN_LSB) | (2 << WDT_CTL_RSPMOD_LSB));

    // Set WDT Reset timer to 1.6 ms.
    WDT_REG_WRITE(WDT_OP_ADDRESS, 1);  // 2^n * 0.8ms, mask 0xf, n = 1 -> (2^1 = 2) * 0.8 * 0.001 = 0.0016

    // Enable WDT
    SET_WDT_REG_MASK(WDT_CTL_ADDRESS, WDT_CTL_EN_MASK, 1 << WDT_CTL_EN_LSB);

    while (true);
  }
};
