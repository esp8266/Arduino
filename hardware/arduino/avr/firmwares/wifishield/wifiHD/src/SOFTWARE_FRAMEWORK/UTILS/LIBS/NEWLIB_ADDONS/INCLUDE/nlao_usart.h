/* This header file is part of the ATMEL AVR-UC3-SoftwareFramework-1.7.0 Release */

/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief NEWLIB_ADDONS USART include file for AVR32.
 *
 * - Compiler:           GNU GCC for AVR32
 * - Supported devices:  All AVR32 devices can be used.
 * - AppNote:
 *
 * \author               Atmel Corporation: http://www.atmel.com \n
 *                       Support and FAQ: http://support.atmel.no/
 *
 ******************************************************************************/

/* Copyright (c) 2009 Atmel Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an Atmel
 * AVR product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE
 *
 */

#ifndef __AVR32_NEWLIB_ADDONS_USART_H__
#define __AVR32_NEWLIB_ADDONS_USART_H__

#include <_ansi.h>

#include "nlao_io.h"

_BEGIN_STD_C

struct usart3 {
	volatile u32	us_cr;
	volatile u32	us_mr;
	volatile u32	us_ier;
	volatile u32	us_idr;
	volatile u32	us_imr;
	volatile u32	us_csr;
	volatile u32	us_rhr;
	volatile u32	us_thr;
	volatile u32	us_brgr;
	volatile u32	us_rtor;
	volatile u32	us_ttgr;
	volatile u32	us_reserved[5];
	volatile u32	us_fidi;
	volatile u32	us_ner;
	volatile u32	us_xxr;
	volatile u32	us_if;
};

/* Register offsets */
#define US_CR			0x0000
#define US_MR			0x0004
#define US_IER			0x0008
#define US_IDR			0x000c
#define US_IMR			0x0010
#define US_CSR			0x0014
#define US_RHR			0x0018
#define US_THR			0x001c
#define US_BRGR			0x0020
#define US_RTOR			0x0024
#define US_TTGR			0x0028

#define US_FIDI			0x0040
#define US_NER			0x0044
#define US_XXR			0x0048
#define US_IF			0x004c

#define US_RPR                  0x0100
#define US_RCR                  0x0104
#define US_TPR                  0x0108
#define US_TCR                  0x010c
#define US_RNPR                 0x0110
#define US_RNCR                 0x0114
#define US_TNPR                 0x0118
#define US_TNCR                 0x011c
#define US_PTCR                 0x0120
#define US_PTSR                 0x0124




/* USART3 Control Register */
#define US_CR_RSTRX		(1 <<  2)
#define US_CR_RSTTX		(1 <<  3)
#define US_CR_RXEN		(1 <<  4)
#define US_CR_RXDIS		(1 <<  5)
#define US_CR_TXEN		(1 <<  6)
#define US_CR_TXDIS		(1 <<  7)
#define US_CR_RSTSTA		(1 <<  8)
#define US_CR_STTBRK		(1 <<  9)
#define US_CR_STPBRK		(1 << 10)

#define US_CR_DTREN		(1 << 16)
#define US_CR_DTRDIS		(1 << 17)
#define US_CR_RTSEN		(1 << 18)
#define US_CR_RTSDIS		(1 << 19)

/* USART3 Mode Register */
#define US_MR_MODE		(15 <<  0)
#define US_MR_MODE_NORMAL	( 0 <<  0)
#define US_MR_MODE_HWFLOW	( 2 <<  0)
#define US_MR_CLKS		( 3 <<  4)
#define US_MR_CLKS_CLOCK	( 0 <<  4)
#define US_MR_CLKS_FDIV1	( 1 <<  4)
#define US_MR_CLKS_SLOW		( 2 <<  4)
#define US_MR_CLKS_EXT		( 3 <<  4)
#define US_MR_CHRL_5BITS	( 0 <<  6)
#define US_MR_CHRL_6BITS	( 1 <<  6)
#define US_MR_CHRL_7BITS	( 2 <<  6)
#define US_MR_CHRL_8BITS	( 3 <<  6)
#define US_MR_SYNC		( 1 <<  8)
#define US_MR_PAR_EVEN		( 0 <<  9)
#define US_MR_PAR_ODD		( 1 <<  9)
#define US_MR_PAR_SPACE		( 2 <<  9)
#define US_MR_PAR_MARK		( 3 <<  9)
#define US_MR_PAR_NONE		( 4 <<  9)
#define US_MR_PAR_MDROP		( 6 <<  9)
#define US_MR_NBSTOP_1BIT	( 0 << 12)
#define US_MR_NBSTOP_1_5BIT	( 1 << 12)
#define US_MR_NBSTOP_2BITS	( 2 << 12)
#define US_MR_OVER		( 1 << 19)
#define US_MR_OVER_X16		( 0 << 19)
#define US_MR_OVER_X8		( 1 << 19)

/* USART3 Channel Status Register */
#define US_CSR_RXRDY		(1 <<  0)
#define US_CSR_TXRDY		(1 <<  1)
#define US_CSR_RXBRK		(1 <<  2)
#define US_CSR_ENDRX		(1 <<  3)
#define US_CSR_ENDTX		(1 <<  4)


#define US_CSR_OVRE		(1 <<  5)
#define US_CSR_FRAME		(1 <<  6)
#define US_CSR_PARE		(1 <<  7)

#define US_CSR_TXEMPTY		(1 <<  9)

#define US_CSR_TXBUFE           (1 << 11)
#define US_CSR_RXBUFF           (1 << 12)
#define US_CSR_RIIC		(1 << 16)
#define US_CSR_DSRIC		(1 << 17)
#define US_CSR_DCDIC		(1 << 18)
#define US_CSR_CTSIC		(1 << 19)
#define US_CSR_RI		(1 << 20)
#define US_CSR_DSR		(1 << 21)
#define US_CSR_DCD		(1 << 22)
#define US_CSR_CTS		(1 << 23)

/* USART3 Baud Rate Generator Register */
#define US_BRGR_CD_OFFSET	0
#define US_BRGR_FP_OFFSET	16

#define US_BRGR_CD_SIZE		16
#define US_BRGR_FP_SIZE		3

#define US_BRGR_CD		(0xFFFF <<  0)
#define US_BRGR_FP		(     7 << 16)

/*USART3 PDC Transfer Control Register */
#define US_PTCR_RXTEN           (1 << 0)
#define US_PTCR_RXTDIS          (1 << 1)
#define US_PTCR_TXTEN           (1 << 8)
#define US_PTCR_TXTDIS          (1 << 9)

/*USART3 PDC Transfer Status Register */
#define US_PTSR_RXTEN           (1 << 0)
#define US_PTSR_TXTEN           (1 << 8)


int usart_init(int baudrate);
void usart_putc(char c);
void usart_puts(const char *s);
int usart_getc(void);
int usart_tstc(void);
void usart_setbrg(int baudrate, int cpu_clock);
void set_usart_base(void *usart_base);


_END_STD_C

#endif /* MERLIN_USART3_H */
