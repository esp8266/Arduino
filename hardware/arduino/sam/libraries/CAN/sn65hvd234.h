/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support
 * ----------------------------------------------------------------------------
 * Copyright (c) 2010, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
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
 /**
   *  \file
   *
   *  Include Defines & macros for the SN65HVD234.
   */

#ifndef _CAN_SN65HVD234_
#define _CAN_SN65HVD234_

#include "board.h"

#ifdef __cplusplus
 extern "C" {
#endif


typedef struct _SSN65HVD234_Data
{
    /** PIO dedicated to Rs pin */
    Pio* pPIO_Rs ;
    /** Rs Pin on PIO */
    uint32_t dwPin_Rs ;

    /** PIO dedicated to EN pin */
    Pio* pPIO_EN ;
    /** EN Pin on PIO */
    uint32_t dwPin_EN ;
} SSN65HVD234_Data ;

extern uint32_t SN65HVD234_Init( SSN65HVD234_Data* pComponent ) ;
extern uint32_t SN65HVD234_SetRs( SSN65HVD234_Data* pComponent, Pio* pPIO_Rs, uint32_t dwPin_Rs ) ;
extern uint32_t SN65HVD234_SetEN( SSN65HVD234_Data* pComponent, Pio* pPIO_EN, uint32_t dwPin_EN ) ;

extern uint32_t SN65HVD234_Enable( SSN65HVD234_Data* pComponent ) ;
extern uint32_t SN65HVD234_Disable( SSN65HVD234_Data* pComponent ) ;

extern uint32_t SN65HVD234_EnableLowPower( SSN65HVD234_Data* pComponent ) ;
extern uint32_t SN65HVD234_DisableLowPower( SSN65HVD234_Data* pComponent ) ;

#ifdef __cplusplus
}
#endif

#endif /* _CAN_SN65HVD234_ */
