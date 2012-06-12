/* ----------------------------------------------------------------------------
 *         SAM Software Package License
 * ----------------------------------------------------------------------------
 * Copyright (c) 2011-2012, Atmel Corporation
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

#ifndef UDPHS_H_INCLUDED
#define UDPHS_H_INCLUDED

#define NUM_IT_MAX 3

#define EP_SINGLE_64 0x32	// EP0
#define EP_DOUBLE_64 0x36	// Other endpoints


// Endpoint 0:
#define EP_TYPE_CONTROL				UDPHS_EPTCFG_EPT_SIZE_64 \
                                  | UDPHS_EPTCFG_EPT_TYPE_CTRL8 \
                                  | UDPHS_EPTCFG_BK_NUMBER_1
#ifdef CDC_ENABLED
#define EP_TYPE_BULK_IN				UDPHS_EPTCFG_EPT_SIZE_512 \
                                  | UDPHS_EPTCFG_EPT_DIR \
                                  | UDPHS_EPTCFG_EPT_TYPE_BULK \
                                  | UDPHS_EPTCFG_BK_NUMBER_2
#define EP_TYPE_BULK_OUT			UDPHS_EPTCFG_EPT_SIZE_512 \
                                  | UDPHS_EPTCFG_EPT_TYPE_BULK \
                                  | UDPHS_EPTCFG_BK_NUMBER_2
#define EP_TYPE_INTERRUPT_IN		UDPHS_EPTCFG_EPT_SIZE_64 \
                                  | UDPHS_EPTCFG_EPT_DIR \
                                  | UDPHS_EPTCFG_EPT_TYPE_INT \
                                  | UDPHS_EPTCFG_BK_NUMBER_2
#endif

#ifdef HID_ENABLED
#define EP_TYPE_INTERRUPT_IN_HID	UDPHS_EPTCFG_EPT_SIZE_64 \
                                  | UDPHS_EPTCFG_EPT_DIR \
                                  | UDPHS_EPTCFG_EPT_TYPE_INT \
                                  | UDPHS_EPTCFG_BK_NUMBER_2
#endif

#define EP_TYPE_INTERRUPT_OUT		UDPHS_EPTCFG_EPT_SIZE_64 \
                                  | UDPHS_EPTCFG_EPT_TYPE_INT \
                                  | UDPHS_EPTCFG_EPT_TYPE_INT \
                                  | UDPHS_EPTCFG_BK_NUMBER_1
#define EP_TYPE_ISOCHRONOUS_IN		UDPHS_EPTCFG_EPT_SIZE_1024 \
                                  | UDPHS_EPTCFG_EPT_DIR \
                                  | UDPHS_EPTCFG_EPT_TYPE_ISO \
                                  | UDPHS_EPTCFG_BK_NUMBER_3
#define EP_TYPE_ISOCHRONOUS_OUT		UDPHS_EPTCFG_EPT_SIZE_1024 \
                                  | UDPHS_EPTCFG_EPT_TYPE_ISO \
                                  | UDPHS_EPTCFG_BK_NUMBER_3


#ifndef TXLED1
#define TXLED0
#define RXLED0
#define TXLED1
#define RXLED1
#endif


#define UDFNUML ((UDPHS->UDPHS_FNUM & UDPHS_FNUM_FRAME_NUMBER_Msk)>>3)

#define USB_RECV_TIMEOUT

#define UDPHS_EPTFIFO (0x20180000) // (UDPHS_EPTFIFO) Base Address

#endif /* UDPHS_H_INCLUDED */
