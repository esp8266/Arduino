/* This header file is part of the ATMEL AVR-UC3-SoftwareFramework-1.7.0 Release */

/*! \page License
 * Copyright (C) 2009, H&D Wireless AB All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of H&D Wireless AB may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY H&D WIRELESS AB ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
 * SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CLOCKS_H
#define CLOCKS_H

#include "board.h"


#if BOARD == EVK1100
# define USE_PLL
# define OSC     0
# define PLL_MUL 10
#elif BOARD == EVK1101
# define USE_PLL
# define OSC     0
# define PLL_MUL 9
#elif BOARD == EVK1104
# define USE_PLL
# define OSC     0
# define PLL_MUL 9 /* for some reason we cant use 66 MHz */
#elif BOARD == EVK1105
# define USE_PLL
# define OSC     1
# define PLL_MUL 10
#elif BOARD == ARDUINO
# define USE_PLL
# define OSC     0
# define PLL_MUL 8
#endif

#if OSC == 0
# define FOSC FOSC0 /* 12 MHz */
#else
# define FOSC FOSC1 /* 11.2896 MHz */
#endif

#ifdef USE_PLL
# define FMCK_HZ ((FOSC * (PLL_MUL + 1)) / 2)
#else
# define FMCK_HZ FOSC
#endif

#define FCPU_HZ FMCK_HZ
#define FHSB_HZ FCPU_HZ
#define FPBB_HZ FMCK_HZ
#define FPBA_HZ FMCK_HZ

void init_sys_clocks(void);

#endif
