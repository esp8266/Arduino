/* ----------------------------------------------------------------------------
 *         SAM Software Package License
 * ----------------------------------------------------------------------------
 * Copyright (c) 2012, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following condition is met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */

#ifndef CONF_BOARD_H_INCLUDED
#define CONF_BOARD_H_INCLUDED

#define SAM3S_EK                       (1)
#define SAM3S_EK2                      (2)
#define SAM3N_EK                       (3)
#define SAM3U_EK                       (4)
#define SAM3X_EK                       (5)
#define SAM4S_EK                       (6)

#ifndef BOARD
#  error "BOARD is not defined"
#endif

#if BOARD == SAM3S_EK
#  define LED0_PIO		PIOA
#  define LED0_MASK		(0x01UL << 19)
#endif /* SAM3S_EK */

#if BOARD == SAM3S_EK2
#  define LED0_PIO		PIOA
#  define LED0_MASK		(0x01UL << 19)
#endif /* SAM3S_EK2 */

#if BOARD == SAM3N_EK
#  define LED0_PIO		PIOA
#  define LED0_MASK		(0x01UL << 23)
#endif /* SAM3N_EK */

#if BOARD == SAM3U_EK
#  define LED0_PIO		PIOB
#  define LED0_MASK		(0x01UL << 0)
#endif /* SAM3U_EK */

#if BOARD == SAM3X_EK
#  define LED0_PIO		PIOB
#  define LED0_MASK		(0x01UL << 12)
#endif /* SAM3X_EK */

#if BOARD == SAM4S_EK
#  define LED0_PIO		PIOA
#  define LED0_MASK		(0x01UL << 19)
#endif /* SAM4S_EK */

#endif /* CONF_BOARD_H_INCLUDED */
